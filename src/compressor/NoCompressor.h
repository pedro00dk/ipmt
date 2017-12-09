#pragma once

#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include "Compressor.h"

using namespace std;

class NoCompressor : public Compressor {
public:

    vector<char> encode(char *str, unsigned long strSize) override {
        vector<char> encoded(str, str + strSize);
        return encoded;
    }

    vector<char> decode(char *encoded, unsigned long encodedSize) override {
      vector<char> decoded(encoded, encoded + encodedSize);
      return decoded;
    }

    int getCompressorCode() override {
        return 0;
    }
};
