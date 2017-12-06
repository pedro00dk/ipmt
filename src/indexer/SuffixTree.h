#pragma once

#include <map>
#include <sstream>
#include "Indexer.h"


class Node {

public:
    long long id;
    long long leftPos, rightPos;
    map<char, Node *> children{};
    Node *slink;

    Node(long long id, long long leftPos, long long rightPos) :
            id(id),
            leftPos(leftPos),
            rightPos(rightPos),
            children(map<char, Node *>()),
            slink(nullptr) {}

};

typedef tuple<Node *, long long, long long> NodeData;

class SuffixTree : public Indexer {
public:
    long long nodeIdGenerator;
    Node *grnd;
    Node *root;

    SuffixTree() :
            nodeIdGenerator(0),
            grnd(nullptr),
            root(nullptr) {}

    void buildIndex(const vector<char> &chars) override {
        nodeIdGenerator = 0;
        grnd = new Node(nodeIdGenerator++, -1, -1);
        root = new Node(nodeIdGenerator++, -1, 0);

        for (long long i = 0; i < ISO_SIZE; i++)
            grnd->children[i] = root;
        root->slink = grnd;

        NodeData starter = {root, 0, 0};
        for (long long i = 0; i < chars.size(); i++) {
            starter = update(starter, chars, i);
            starter = canonise({get<0>(starter), get<1>(starter), get<2>(starter) + 1}, chars);
        }
    }

    void search(const vector<string> &patterns, bool count, bool print) {
    }

    vector<char> serialize() override {
        vector<char> bytes;
        posOrderNodeSerialize(root, bytes);
        return bytes;
    }

    void deserialize(const vector<char> &bytes) override {
        stringstream stream;
        stream.write(&bytes[0], bytes.size());

        std::map<long long, Node *> nodes;

        bool inHeader = true;
        Node *currentNode = nullptr; // the root node is the last to be added (post order in serialize)
        for (string line; getline(stream, line);) {
            if (inHeader) {
                long long idEnd = line.find(';');
                long long lrSplit = line.find(',');

                long long id = stoll(line.substr(0, idEnd));
                long long leftPos = stoll(line.substr(idEnd + 1, lrSplit - (idEnd + 1)));
                long long rightPos = stoll(line.substr(lrSplit + 1, line.size()));
                rightPos = rightPos == -1 ? LONG_LONG_MAX : rightPos;

                currentNode = new Node(id, leftPos, rightPos);
                nodes[id] = currentNode;
                inHeader = false;
            } else {
                if (line.size() == 0) {
                    inHeader = true;
                    continue;
                }
                long long chIdSplit = line.find(',');

                char ch = (char) stoi(line.substr(0, chIdSplit));
                long long childId = stoll(line.substr(chIdSplit + 1, line.size()));

                currentNode->children[ch] = nodes[childId];
            }
        }
        nodeIdGenerator = nodes.size();
        grnd = new Node(0, -1, -1);
        root = currentNode;
        for (long long i = 0; i < ISO_SIZE; i++)
            grnd->children[i] = root;
    }

private:
    NodeData update(NodeData nodeData, const vector<char> &chars, long long i) {
        Node *previousChild = nullptr;
        tuple<bool, Node *> isTerm_child = testSplit(nodeData, chars, i);
        bool isTerm = get<0>(isTerm_child);
        Node *child = get<1>(isTerm_child);
        while (!isTerm) {
            Node *leaf = new Node(nodeIdGenerator++, i, LONG_LONG_MAX);
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
        long long leftPos = get<1>(nodeData);
        long long rightPos = get<2>(nodeData);
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

    tuple<bool, Node *> testSplit(NodeData nodeData, const vector<char> &chars, long long i) {
        long long leftPos = get<1>(nodeData);
        long long rightPos = get<2>(nodeData);
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

    void posOrderNodeSerialize(Node *node, vector<char> &bytes) {
        if (!node->children.empty()) {
            map<char, Node *>::iterator itr = node->children.begin();
            map<char, Node *>::iterator end = node->children.end();
            while (itr != end) {
                posOrderNodeSerialize(itr->second, bytes);
                ++itr;
            }
        }

        string header = to_string(node->id) + ";" +
                        to_string(node->leftPos) + "," +
                        to_string(node->rightPos == LONG_LONG_MAX ? -1 : node->rightPos) +
                        "\n";
        bytes.insert(end(bytes), begin(header), end(header));

        map<char, Node *>::iterator itr = node->children.begin();
        map<char, Node *>::iterator end = node->children.end();
        while (itr != end) {
            string childInfo = to_string(itr->first) + "," + to_string(itr->second->id) + "\n";
            std::copy(childInfo.begin(), childInfo.end(), back_inserter(bytes));
            ++itr;
        }
        bytes.push_back('\n');
    }
};
