#pragma once

#include <map>
#include "Indexer.h"


class Node {
public:
    int id;

    int leftPos, rightPos;
    map<char, Node *> children;
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

    SuffixTree() :
            nodeIdGenerator(0),
            grnd(nullptr),
            root(nullptr) {}

    void buildIndex(char *str, int strSize) override {
        nodeIdGenerator = 0;
        grnd = new Node(nodeIdGenerator++, -1, -1);
        root = new Node(nodeIdGenerator++, -1, 0);
        for (int i = 0; i < ISO_SIZE; i++)
            grnd->children[i] = root;
        root->slink = grnd;

        NodeData starter = {root, 0, 0};
        for (long i = 0; i < strSize; i++) {
            starter = update(starter, str, i);
            starter = canonise({get<0>(starter), get<1>(starter), get<2>(starter) + 1}, str);
        }
    }

    vector<char> getIndexBytes() override {
        vector<char> bytes;
        posOrderNodeSerialize(root, bytes);
        return bytes;
    }

    void rebuildIndexFromBytes(const vector<char> &indexBytes) override {

    }

    void search(const vector<string> &patterns, bool count, bool print) override {

    }

private:
    NodeData update(NodeData nodeData, char *str, long i) {
        Node *previousChild = nullptr;
        tuple<bool, Node *> isTerm_child = testSplit(nodeData, str, i);
        bool isTerm = get<0>(isTerm_child);
        Node *child = get<1>(isTerm_child);
        while (!isTerm) {
            Node *leaf = new Node(nodeIdGenerator++, i, LONG_MAX);
            child->children[str[i]] = leaf;
            if (previousChild != nullptr) previousChild->slink = child;
            previousChild = child;

            nodeData = canonise({get<0>(nodeData)->slink, get<1>(nodeData), get<2>(nodeData)}, str);

            isTerm_child = testSplit(nodeData, str, i);
            isTerm = get<0>(isTerm_child);
            child = get<1>(isTerm_child);
        }
        if (previousChild != nullptr) previousChild->slink = child;
        return nodeData;
    }

    NodeData canonise(NodeData nodeData, char *str) {
        int leftPos = get<1>(nodeData);
        int rightPos = get<2>(nodeData);
        if (leftPos >= rightPos) return nodeData;

        Node *current = get<0>(nodeData);
        Node *child = current->children[str[leftPos]];
        while (child->rightPos - child->leftPos <= rightPos - leftPos) {
            current = child;
            leftPos += child->rightPos - child->leftPos;
            if (leftPos < rightPos) child = current->children[str[leftPos]];
        }
        return {current, leftPos, rightPos};
    }

    tuple<bool, Node *> testSplit(NodeData nodeData, char *str, long i) {
        int leftPos = get<1>(nodeData);
        int rightPos = get<2>(nodeData);
        Node *current = get<0>(nodeData);
        if (leftPos >= rightPos) return {current->children.count(str[i]) != 0, current};

        Node *child = current->children[str[leftPos]];
        if (str[child->leftPos + (rightPos - leftPos)] == str[i]) return {true, current};

        Node *newChild = new Node(nodeIdGenerator++, child->leftPos, child->leftPos + (rightPos - leftPos));
        child->leftPos += (rightPos - leftPos);
        current->children[str[leftPos]] = newChild;
        newChild->children[str[child->leftPos]] = child;
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

        string header = to_string(node->id) + "," +
                        to_string(node->leftPos) + "," +
                        to_string(node->rightPos == LONG_MAX ? -1 : node->rightPos) +
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
