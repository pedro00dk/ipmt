#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "Compressor.h"

using namespace std;

//TODO: use trie instead of map
class Lz78 : public Compressor {
private:
    int dictMaxSize = 256;
    map<vector<char>, int> dict;
    int nextDictPos;
public:
    Lz78() {}

    vector<char> encode(char *str, int strSize) override {
        puts("encoding");
        vector<char> encoded;

        dict.clear();
        for (char c = 'a'; c <= 'z'; c++) {
            vector<char> tmp;
            tmp.push_back(c);
            dict[tmp] = c - 'a';
        }
//        dict[vector<char>()] = 0;
        nextDictPos = 'z' - 'a' + 1;

        vector<char> currentStr;
        for (int i = 0; i < strSize; i++) {
            currentStr.push_back(str[i]);

            if (dict.find(currentStr) == dict.end()) {
                currentStr.pop_back();
                encoded.push_back(dict[currentStr]);
                encoded.push_back(str[i]);

                if (dict.size() == dictMaxSize) {
                    dict.clear();
                    for (char c = 'a'; c <= 'z'; c++) {
                        vector<char> tmp;
                        tmp.push_back(c);
                        dict[tmp] = c - 'a';
                    }
                    nextDictPos = 'z' - 'a' + 1;
                } else {
                    currentStr.push_back(str[i]);
                    dict[currentStr] = nextDictPos;
                    nextDictPos++;
                }
                currentStr.clear();
            }
        }

        if (currentStr.size()) {
            encoded.push_back(dict[currentStr]);
            encoded.push_back(0);
        }

        return encoded;
    }

    vector<char> decode(char *encoded, int encodedSize) override {
        puts("decoding");
        vector<char> decoded;

        vector<char> reverseDict[dictMaxSize];
        for (char c = 'a'; c <= 'z'; c++) {
            vector<char> tmp;
            tmp.push_back(c);
            reverseDict[c - 'a'] = tmp;
        }
        nextDictPos = 'z' - 'a' + 1;

        for (int i = 0; i < encodedSize; i += 2) {
            int dictPos = (unsigned char) encoded[i];
            char nextChar = encoded[i + 1];

            for (char c : reverseDict[dictPos]) {
                decoded.push_back(c);
            }
            decoded.push_back(nextChar);

            if (nextDictPos == dictMaxSize) {
                nextDictPos = 'z' - 'a' + 1;
                for (int j = nextDictPos; j < dictMaxSize; j++) {
                    reverseDict[j].clear();
                }
            } else {
                //TODO check if copy necessary
                vector<char> newEntry;
                for (char c : reverseDict[dictPos]) {
                    newEntry.push_back(c);
                }
                newEntry.push_back(nextChar);
                reverseDict[nextDictPos] = newEntry;
                nextDictPos++;
            }
        }

        return decoded;
    }

private:
};
