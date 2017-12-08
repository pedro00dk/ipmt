#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <list>
#include "Compressor.h"

using namespace std;

//TODO: use trie instead of unordered_map
class Lz78 : public Compressor {
private:
    int dictMaxSize = 256;
public:
    Lz78() {}

    vector<char> encode(char *str, int strSize) override {
        puts("encoding");
        vector<char> encoded;
        unordered_map<string, int> dict;
        list <unordered_map<string, int>::iterator> dictPos;
        int nextDictPos = 1;

        dict.clear();
        dict[""] = 0;
        dictPos.push_back(dict.begin());

        string currentStr = "";
        for (int i = 0; i < strSize; i++) {
            currentStr.push_back(str[i]);

            if (dict.find(currentStr) == dict.end()) {
                if (nextDictPos == 256) nextDictPos = 1;

                currentStr.pop_back();
                encoded.push_back(dict[currentStr]);
                encoded.push_back(str[i]);

                if (dict.size() == dictMaxSize) {
                    auto dictPosIt = dictPos.begin();
                    dictPosIt++;

                    dict.erase(*dictPosIt);
                    dictPos.erase(dictPosIt);
                }

                currentStr.push_back(str[i]);
                dict[currentStr] = nextDictPos;
                dictPos.push_back(dict.find(currentStr));

                nextDictPos++;
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

        reverseDict[0] = "";

        int nextDictPos = 1;

        for (int i = 0; i < encodedSize; i += 2) {
            int dictPos = (unsigned char) encoded[i];
            char nextChar = encoded[i + 1];

            for (char c : reverseDict[dictPos]) {
                decoded.push_back(c);
            }
            decoded.push_back(nextChar);

            if (nextDictPos == 256) nextDictPos = 1;

            string newEntry = "";
            for (char c : reverseDict[dictPos]) {
                newEntry.push_back(c);
            }
            newEntry.push_back(nextChar);
            reverseDict[nextDictPos] = newEntry;
            nextDictPos++;
        }

        return decoded;
    }
};
