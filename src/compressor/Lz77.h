#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include "Compressor.h"

using namespace std;

class Lz77 : public Compressor {
private:
    int bufferSize = 4095;
    int lookaheadSize = 15;
public:
    Lz77() {}

    vector<char> encode(char *str, int strSize) override {
        vector<char> encoded;

        int i = 0;
        tuple<int, int, char> occurrence;
        while (i < strSize) {
            occurrence = patternMatch(str, strSize, i, bufferSize, lookaheadSize);
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

private:
    tuple<int, int, char> patternMatch(char *str, int strSize, int lookStart, int bufferSize, int lookSize) {
        int lookEnd = min(strSize - 1, lookStart + lookSize);
//        int lookEnd = min(int(strSize - 1), lookStart + lookSize); // pq o cast muda o tamanho da compressao?
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

            if (size == lookSize) {
                break;
            }
        }

        return make_tuple(bufferSize - biggestMatchPos, biggestMatchSize, str[lookStart + biggestMatchSize]);
    }
};
