#pragma once

#include <iostream>
#include <tuple>
#include <climits>
#include <string>
#include <vector>

using namespace std;

#define ISO_SIZE 256

class Indexer {

public:
    virtual void buildIndex(char* str, int strSize) = 0;
    virtual vector<char> getIndexBytes() = 0;
    virtual void rebuildIndexFromBytes(const vector<char> &indexBytes) = 0;
    virtual void search(const vector<string> &patterns, bool count, bool print) = 0;
};
