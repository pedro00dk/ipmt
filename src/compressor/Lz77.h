#pragma once

#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <limits.h>
#include "Compressor.h"

using namespace std;

#define ALPHABET_SIZE UCHAR_MAX + 1

class Lz77 : public Compressor {
private:
    int bufferMaxSize = 4023;
    int lookaheadMaxSize = 15;
    deque<unsigned long> charStartPositions[ALPHABET_SIZE];
    unsigned long startPositionsBegin, startPositionsEnd;
public:
    vector<char> encode(char *str, unsigned long strSize) override {
        vector<char> encoded;

        unsigned long i = 0;
        tuple<int, int, char> occurrence;
        while (i < strSize) {
            occurrence = prefixMatch(str, strSize, i);
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

    vector<char> decode(char *encoded, unsigned long encodedSize) override {
        vector<char> decoded;

        unsigned long pos = 0;
        for (unsigned long i = 0; i < encodedSize; i += 3) {
            int firstByte = (unsigned char) encoded[i];
            int secondByte = (unsigned char) encoded[i + 1];

            int size = (unsigned char) secondByte & 0x000F;
            unsigned long st = pos - (((unsigned char) firstByte << 4) | ((unsigned char) secondByte >> 4));
            unsigned long en = st + size;

            for (unsigned long j = st; j < en; j++) {
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
    tuple<int, int, char> prefixMatch(char *str, unsigned long strSize, unsigned long lookStart) {
        unsigned long lookEnd = min(strSize, lookStart + lookaheadMaxSize);
//        int lookEnd = min(int(strSize), lookStart + lookSize); //TODO: pq o cast muda o tamanho da compressao?
        unsigned long bufferStart = lookStart > bufferMaxSize ? lookStart - bufferMaxSize : 0UL;
        unsigned long bufferEnd = lookStart;

        int lookaheadSize = lookEnd - lookStart;

        updateCharStartPositions(str, bufferStart, bufferEnd);

        int biggestMatchSize = 0;
        unsigned long biggestMatchPos = lookStart;

        deque <unsigned long> &d = charStartPositions[(unsigned char) str[lookStart]];
        for (int i = 0; i < int(d.size()); i++) {
            unsigned long startPos = d[i];
            int size = 1;
            while (size < lookaheadSize && str[startPos + size] == str[lookStart + size]) {
              size++;
            }

            if (size > biggestMatchSize) {
                biggestMatchSize = size;
                biggestMatchPos = startPos;
            }

            if (size == lookaheadSize) {
                break;
            }
        }

        return make_tuple(lookStart - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }

    void updateCharStartPositions(char *str, unsigned long startPos, unsigned long endPos) {
      for (unsigned long i = startPositionsBegin; i < startPos; i++) {
          charStartPositions[(unsigned char) str[i]].pop_front();
      }

      for (unsigned long i = startPositionsEnd; i < endPos; i++) {
          charStartPositions[(unsigned char) str[i]].push_back(i);
      }

      startPositionsBegin = startPos;
      startPositionsEnd = endPos;
    }
};
