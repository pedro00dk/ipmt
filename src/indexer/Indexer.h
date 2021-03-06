#pragma once

#include <iostream>
#include <string>
#include <vector>

#define ISO_SIZE 256

using namespace std;

class Indexer {

public:
    virtual void buildIndex(const vector<char> &chars, bool verbose) = 0;

    virtual void search(const vector<string> &patterns, bool count, bool print) = 0;

    virtual vector<char> serialize(bool verbose) = 0;

    virtual void deserialize(const vector<char> &bytes, bool verbose) = 0;
};
