#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include "Compressor.h"

using namespace std;

#define ALPHABET_SIZE UCHAR_MAX

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
//            occurrence = patternMatch(str, strSize, i);
            occurrence = prefixMatch(str, strSize, i);
            printf("(%d,%d,%c)\n", get<0>(occurrence), get<1>(occurrence), get<2>(occurrence));
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
            int st = pos - (unsigned char) encoded[i + 1] - ((unsigned char) encoded[i] << 8);
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
    tuple<int, int, char> patternMatch(char *str, int strSize, int lookStart) {
        int lookEnd = min(strSize - 1, lookStart + lookaheadSize);
        int bufferStart = max(0, lookStart - bufferSize);
        int bufferEnd = lookStart;

        bufferSize = bufferEnd - bufferStart;
        lookaheadSize = lookEnd - lookStart;

        int biggestMatchSize = 0;
        int biggestMatchPos = lookStart;

        for (int i = 0; i < bufferSize; i++) {
            int size = 0;
            while (size < lookaheadSize && str[i + size + bufferStart] == str[size + lookStart]) size++;

            if (size > biggestMatchSize) {
                biggestMatchSize = size;
                biggestMatchPos = i;
            }
        }

        return make_tuple(bufferSize - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }

    tuple<int, int, char> prefixMatch(char *str, int strSize, int lookStart) {
        int lookEnd = min(strSize - 1, lookStart + lookaheadSize);
        int bufferStart = max(0, lookStart - bufferSize);
        int bufferEnd = lookStart;

        bufferSize = bufferEnd - bufferStart;
        lookaheadSize = lookEnd - lookStart;

        vector<vector<int>> fsm = build_fsm(str, strSize, lookStart);
        int fsmPos = 0;
        int biggestMatchSize = 0;
        int biggestMatchPos = lookStart;

        for (int i = 0; i < bufferSize; i++) {
            fsmPos = fsm[fsmPos][str[i]];

            if (fsmPos > biggestMatchSize) {
                biggestMatchSize = fsmPos;
                biggestMatchPos = i;
            }

            if (fsmPos == lookaheadSize) break;
        }

        return make_tuple(bufferSize - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }

public:
    //TODO: check memory problem
    vector<vector<int>> build_fsm(const char *str, int strSize, int startPos = 0) {
        vector<vector<int>> fsm;
        for (int i = 0; i < strSize; i++) {
            fsm.push_back(vector<int>(ALPHABET_SIZE));
        }

        for (int c = 0; c < ALPHABET_SIZE; c++) {
            fsm[0][c] = 0;
        }

        fsm[0][str[0]] = 1;

        int border = 0;
        for (int i = 1; i < strSize; i++) {
            for (int c = 0; c < ALPHABET_SIZE; c++) {
                fsm[i][c] = fsm[border][c];
            }
            if (i != strSize) {
                fsm[i][str[i]] = i + 1;
                border = fsm[border][str[i]];
            }
        }

        return fsm;
    }
};