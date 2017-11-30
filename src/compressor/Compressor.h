#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define ISO_SIZE 256

class Compressor {

public:
    //virtual void encode(const string &textName, istream &text) = 0;
    virtual vector<char> encode(char* str, int strSize) = 0;
    virtual vector<char> decode(char* str, int strSize) = 0;
};
