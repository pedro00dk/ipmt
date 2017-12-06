#pragma once

#include <map>
#include <sstream>
#include "Indexer.h"


class Node {

public:
    int id;
    int leftPos, rightPos;
    map<char, Node *> children{};
    Node *slink;

    Node(int id, int leftPos, int rightPos) :
            id(id),
            leftPos(leftPos),
            rightPos(rightPos),
            children(map<char, Node *>()),
            slink(nullptr) {}

};

typedef tuple<Node *, int, int> NodeData;

class SuffixTree : public Indexer {
public:
    int nodeIdGenerator;
    Node *grnd;
    Node *root;

    vector<char> chars;

    SuffixTree() :
            nodeIdGenerator(0),
            grnd(nullptr),
            root(nullptr),
            chars(vector<char>()) {}

    void buildIndex(const vector<char> &chars) override {
        nodeIdGenerator = 0;
        grnd = new Node(nodeIdGenerator++, -1, -1);
        root = new Node(nodeIdGenerator++, -1, 0);
        this->chars = chars;

        for (int i = 0; i < ISO_SIZE; i++)
            grnd->children[i] = root;
        root->slink = grnd;

        NodeData starter = {root, 0, 0};
        for (int i = 0; i < chars.size(); i++) {
            starter = update(starter, chars, i);
            starter = canonise({get<0>(starter), get<1>(starter), get<2>(starter) + 1}, chars);
        }
    }

    void search(const vector<string> &patterns, bool count, bool print) {

        string pattern = patterns[0];
        int patternSize = patterns[0].size();

        int patternCursor = 0;
        Node *nodeCursor = root;

        bool patternNotExists = false;

        while (!patternNotExists) {

            for (
                    int i = max(nodeCursor->leftPos, 0);
                    patternCursor < patternSize && i < min(nodeCursor->rightPos, (int) chars.size());
                    i++) {

                if (chars[i] == pattern[patternCursor]) patternCursor++;
                else {
                    patternNotExists = true;
                    break;
                }
            }

            if (patternCursor == patternSize) {
                // count leafs
                break;
            }

            if (!patternNotExists) {
                if (nodeCursor->children.count(pattern[patternCursor]) != 0)
                    nodeCursor = nodeCursor->children[pattern[patternCursor]];
                else patternNotExists = true;
            }

            if (patternNotExists) {
                cout << "pattern " << pattern << " not found" << endl;
                break;
            }
        }
    }

    vector<char> serialize() override {
        vector<char> bytes;
        serializeNodes(root, bytes);
        bytes.push_back('=');
        bytes.push_back('\n');
        copy(chars.begin(), chars.end(), back_inserter(bytes));
        return bytes;
    }

    void deserialize(const vector<char> &bytes) override {
        stringstream stream;
        stream.write(&bytes[0], bytes.size());

        std::map<int, Node *> nodes;

        bool inHeader = true;
        Node *currentNode = nullptr; // the root node is the last to be added (post order in serialize)
        for (string line; getline(stream, line);) {
            if (inHeader) {
                if (line.size() == 1) {
                    break;
                }
                int idEnd = line.find(';');
                int lrSplit = line.find(',');

                int id = stoll(line.substr(0, idEnd));
                int leftPos = stoll(line.substr(idEnd + 1, lrSplit - (idEnd + 1)));
                int rightPos = stoll(line.substr(lrSplit + 1, line.size()));
                rightPos = rightPos == -1 ? INT_MAX : rightPos;

                currentNode = new Node(id, leftPos, rightPos);
                nodes[id] = currentNode;
                inHeader = false;
            } else {
                if (line.size() == 0) {
                    inHeader = true;
                    continue;
                }
                int chIdSplit = line.find(',');

                char ch = (char) stoi(line.substr(0, chIdSplit));
                int childId = stoll(line.substr(chIdSplit + 1, line.size()));

                currentNode->children[ch] = nodes[childId];
            }
        }
        stream >> std::noskipws;
        chars = vector<char>();
        chars = vector<char>((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());

        nodeIdGenerator = nodes.size();
        grnd = new Node(0, -1, -1);
        root = currentNode;
        for (int i = 0; i < ISO_SIZE; i++)
            grnd->children[i] = root;
    }

private:
    NodeData update(NodeData nodeData, const vector<char> &chars, int i) {
        Node *previousChild = nullptr;
        tuple<bool, Node *> isTerm_child = testSplit(nodeData, chars, i);
        bool isTerm = get<0>(isTerm_child);
        Node *child = get<1>(isTerm_child);
        while (!isTerm) {
            Node *leaf = new Node(nodeIdGenerator++, i, INT_MAX);
            child->children[chars[i]] = leaf;
            if (previousChild != nullptr) previousChild->slink = child;
            previousChild = child;

            nodeData = canonise({get<0>(nodeData)->slink, get<1>(nodeData), get<2>(nodeData)}, chars);

            isTerm_child = testSplit(nodeData, chars, i);
            isTerm = get<0>(isTerm_child);
            child = get<1>(isTerm_child);
        }
        if (previousChild != nullptr) previousChild->slink = child;
        return nodeData;
    }

    NodeData canonise(NodeData nodeData, const vector<char> &chars) {
        int leftPos = get<1>(nodeData);
        int rightPos = get<2>(nodeData);
        if (leftPos >= rightPos) return nodeData;

        Node *current = get<0>(nodeData);
        Node *child = current->children[chars[leftPos]];
        while (child->rightPos - child->leftPos <= rightPos - leftPos) {
            current = child;
            leftPos += child->rightPos - child->leftPos;
            if (leftPos < rightPos) child = current->children[chars[leftPos]];
        }
        return {current, leftPos, rightPos};
    }

    tuple<bool, Node *> testSplit(NodeData nodeData, const vector<char> &chars, int i) {
        int leftPos = get<1>(nodeData);
        int rightPos = get<2>(nodeData);
        Node *current = get<0>(nodeData);
        if (leftPos >= rightPos) return {current->children.count(chars[i]) != 0, current};

        Node *child = current->children[chars[leftPos]];
        if (chars[child->leftPos + (rightPos - leftPos)] == chars[i]) return {true, current};

        Node *newChild = new Node(nodeIdGenerator++, child->leftPos, child->leftPos + (rightPos - leftPos));
        child->leftPos += (rightPos - leftPos);
        current->children[chars[leftPos]] = newChild;
        newChild->children[chars[child->leftPos]] = child;
        return {false, newChild};
    }

    void serializeNodes(Node *node, vector<char> &bytes) {
        if (!node->children.empty()) {
            map<char, Node *>::iterator itr = node->children.begin();
            map<char, Node *>::iterator end = node->children.end();
            while (itr != end) {
                serializeNodes(itr->second, bytes);
                ++itr;
            }
        }

        string header = to_string(node->id) + ";" +
                        to_string(node->leftPos) + "," +
                        to_string(node->rightPos == INT_MAX ? -1 : node->rightPos) +
                        "\n";
        bytes.insert(end(bytes), begin(header), end(header));

        map<char, Node *>::iterator itr = node->children.begin();
        map<char, Node *>::iterator end = node->children.end();
        while (itr != end) {
            string childInfo = to_string(itr->first) + "," + to_string(itr->second->id) + "\n";
            copy(childInfo.begin(), childInfo.end(), back_inserter(bytes));
            ++itr;
        }
        bytes.push_back('\n');
    }
};
