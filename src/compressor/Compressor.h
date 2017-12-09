#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define ISO_SIZE 256

class Compressor {

public:
    virtual vector<char> encode(char* str, unsigned long strSize) = 0;
    virtual vector<char> decode(char* str, unsigned long strSize) = 0;
    virtual int getCompressorCode() = 0;
};
