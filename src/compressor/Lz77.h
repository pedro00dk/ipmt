#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include "Compressor.h"

using namespace std;

class Lz77 : public Compressor {
private:
    int bufferSize = 65535;
    int lookaheadSize = 255;
public:
    Lz77() {}

    vector<char> encode(char *str, int strSize) override {
        vector<char> encoded;

        int i = 0;
        tuple<int, int, char> occurrence;
        while (i < strSize) {
            occurrence = patternMatch(str, strSize, i, bufferSize, lookaheadSize);
            encoded.push_back((get<0>(occurrence) & 0xFF00) >> 8);
            encoded.push_back((get<0>(occurrence) & 0x00FF));
            encoded.push_back(get<1>(occurrence));
            encoded.push_back(get<2>(occurrence));
            i += get<1>(occurrence) + 1;
        }

        return encoded;
    }

    vector<char> decode(char *encoded, int encodedSize) override {
        vector<char> decoded;

        int pos = 0;
        for (int i = 0; i < encodedSize; i += 4) {
            int size = (unsigned char) encoded[i + 2];
            int st = pos - (unsigned char) encoded[i+1] - ((unsigned char) encoded[i] << 8);
            int en = st + size;

            for (int j = st; j < en; j++) {
                decoded.push_back(decoded[j]);
            }

            decoded.push_back(encoded[i + 3]);
            pos += size + 1;
        }

        return decoded;
    }

private:
    tuple<int, int, char> patternMatch(char *str, int strSize, int lookStart, int bufferSize, int lookSize) {
        int lookEnd = min(int(strSize - 1), lookStart + lookSize);
        int bufferStart = max(0, lookStart - bufferSize);
        int bufferEnd = lookStart;

        bufferSize = bufferEnd - bufferStart;
        lookSize = lookEnd - lookStart;

        int biggestMatchSize = 0;
        int biggestMatchPos = lookStart;

        for (int i = 0; i < bufferSize; i++) {
            int size = 0;
            while (size < lookSize && str[i + size + bufferStart] == str[size + lookStart]) size++;

            if (size > biggestMatchSize) {
                biggestMatchSize = size;
                biggestMatchPos = i;
            }
        }

        return make_tuple(bufferSize - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }
};
