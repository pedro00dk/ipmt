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
public:
    Lz78() {}

    vector<char> encode(char *str, int strSize) override {
        puts("encoding");
        vector<char> encoded;
        map<string, int> dict;
        int nextDictPos;

        dict.clear();
        for (char c = 'a'; c <= 'z'; c++) {
            string tmp = "";
            tmp.push_back(c);
            dict[tmp] = c - 'a' + 1;
        }
        dict[""] = 0;
        nextDictPos = 'z' - 'a' + 2;

        string currentStr = "";
        for (int i = 0; i < strSize; i++) {
            currentStr.push_back(str[i]);

            if (dict.find(currentStr) == dict.end()) {
                currentStr.pop_back();
                encoded.push_back(dict[currentStr]);
                encoded.push_back(str[i]);

                if (dict.size() == dictMaxSize) {
                    dict.clear();
                    for (char c = 'a'; c <= 'z'; c++) {
                        string tmp = "";
                        tmp.push_back(c);
                        dict[tmp] = c - 'a' + 1;
                    }
                    dict[""] = 0;
                    nextDictPos = 'z' - 'a' + 2;
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

        string reverseDict[dictMaxSize];
        int nextDictPos;

        reverseDict[0] = "";
        for (char c = 'a'; c <= 'z'; c++) {
            string tmp = "";
            tmp.push_back(c);
            reverseDict[c - 'a' + 1] = tmp;
        }
        nextDictPos = 'z' - 'a' + 2;

        for (int i = 0; i < encodedSize; i += 2) {
            int dictPos = (unsigned char) encoded[i];
            char nextChar = encoded[i + 1];

            for (char c : reverseDict[dictPos]) {
                decoded.push_back(c);
            }
            decoded.push_back(nextChar);

            if (nextDictPos == dictMaxSize) {
                nextDictPos = 'z' - 'a' + 2;
                for (int j = nextDictPos; j < dictMaxSize; j++) {
                    reverseDict[j].clear();
                }
            } else {
                //TODO check if copy necessary
                string newEntry = "";
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
};
