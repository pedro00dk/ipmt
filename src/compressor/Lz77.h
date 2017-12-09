#pragma once

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
    int bufferMaxSize = 4095;
    int lookaheadMaxSize = 15;
    vector<vector<int>> fsm;
public:
    Lz77() {
        initFsm();
    }

    vector<char> encode(char *str, int strSize) override {
        vector<char> encoded;

        int i = 0;
        tuple<int, int, char> occurrence;
        while (i < strSize) {
            occurrence = patternMatch(str, strSize, i);
//            occurrence = prefixMatch(str, strSize, i);
            int matchPos = get<0>(occurrence);
            int matchSize = get<1>(occurrence);
            char firstByte = (char) (matchPos >> 4);
            char secondByte = (char) ((((0x000F & matchPos) << 4)) | matchSize);

            encoded.push_back(firstByte);
            encoded.push_back(secondByte);
            encoded.push_back((char) get<2>(occurrence));

            i += get<1>(occurrence) + 1;
        }

        return encoded;
    }

    vector<char> decode(char *encoded, int encodedSize) override {
        vector<char> decoded;

        int pos = 0;
        for (int i = 0; i < encodedSize; i += 3) {
            int firstByte = (unsigned char) encoded[i];
            int secondByte = (unsigned char) encoded[i + 1];

            int size = (unsigned char) secondByte & 0x000F;
            int st = pos - (((unsigned char) firstByte << 4) | ((unsigned char) secondByte >> 4));
            int en = st + size;

            for (int j = st; j < en; j++) {
                decoded.push_back(decoded[j]);
            }

            decoded.push_back(encoded[i + 2]);
            pos += size + 1;
        }

        return decoded;
    }

    int getCompressorCode() override {
        return 77;
    }

private:
    tuple<int, int, char> patternMatch(char *str, int strSize, int lookStart) {
        int lookEnd = min(strSize, lookStart + lookaheadMaxSize);
//        int lookEnd = min(int(strSize), lookStart + lookSize); //TODO: pq o cast muda o tamanho da compressao?
        int bufferStart = max(0, lookStart - bufferMaxSize);
        int bufferEnd = lookStart;

        int bufferSize = bufferEnd - bufferStart;
        int lookaheadSize = lookEnd - lookStart;

        int biggestMatchSize = 0;
        int biggestMatchPos = lookStart;

        for (int i = 0; i < bufferSize; i++) {
            int size = 0;
            while (size < lookaheadSize && str[i + size + bufferStart] == str[size + lookStart]) size++;

            if (size > biggestMatchSize) {
                biggestMatchSize = size;
                biggestMatchPos = i;
            }

            if (size == lookaheadSize) {
                break;
            }
        }

        return make_tuple(bufferSize - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }

    tuple<int, int, char> prefixMatch(char *str, int strSize, int lookStart) {
        int lookEnd = min(strSize, lookStart + lookaheadMaxSize);
        int bufferStart = max(0, lookStart - bufferMaxSize);
        int bufferEnd = lookStart;

        int bufferSize = bufferEnd - bufferStart;
        int lookaheadSize = lookEnd - lookStart;

        build_fsm(str, strSize, lookStart);
        int fsmPos = 0;
        int biggestMatchSize = 0;
        int biggestMatchPos = lookStart;

        for (int i = 0; i < bufferSize; i++) {
            fsmPos = fsm[fsmPos][(unsigned char) str[bufferStart + i]];

            if (fsmPos > biggestMatchSize) {
                biggestMatchSize = fsmPos;
                biggestMatchPos = i - fsmPos + 1;
            }

            if (fsmPos == lookaheadSize) break;
        }

        return make_tuple(bufferSize - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }

public:
    vector<vector<int>> build_fsm(const char *str, int strSize, int startPos = 0) {
        for (int c = 0; c <= ALPHABET_SIZE; c++) {
            fsm[0][c] = 0;
        }

        fsm[0][(unsigned char) str[startPos]] = 1;

        int border = 0;
        int lim = min(strSize - startPos, lookaheadMaxSize);
        for (int i = 1; i < lim; i++) {
            for (int c = 0; c <= ALPHABET_SIZE; c++) {
                fsm[i][c] = fsm[border][c];
            }

            fsm[i][(unsigned char) str[i + startPos]] = i + 1;
            border = fsm[border][(unsigned char) str[i + startPos]];
        }

        return fsm;
    }

    void initFsm() {
        for (int i = 0; i < lookaheadMaxSize; i++) {
            fsm.push_back(vector<int>(ALPHABET_SIZE + 1));
        }
    }
};
