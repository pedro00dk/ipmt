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

    vector<char> encode(char *str, int strSize) override {
        vector<char> encoded(str, str + strSize);
        return encoded;
    }

    vector<char> decode(char *encoded, int encodedSize) override {
      vector<char> decoded(encoded, encoded + encodedSize);
      return decoded;
    }

    int getCompressorCode() override {
        return 0;
    }
};
