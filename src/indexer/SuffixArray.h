#pragma once

#include <algorithm>
#include <cmath>
#include "Indexer.h"


struct Suffix {
    int index;
    int rank[2];

};

bool compareLT(Suffix s0, Suffix s1) {
    return s0.rank[0] < s1.rank[0] ? true : s0.rank[0] > s1.rank[0] ? false : s0.rank[1] < s1.rank[1];
}

class SuffixArray : public Indexer {

public:
    vector<char> chars;
    vector<int> suffixArray;
    vector<int> lcp;

    void buildIndex(const vector<char> &chars, bool verbose) override {
        this->chars = chars;
        vector<Suffix> suffixes(chars.size());
        for (int i = 0; i < chars.size(); i++) {
            suffixes[i].index = i;
            suffixes[i].rank[0] = chars[i];
            suffixes[i].rank[1] = ((i + 1) < chars.size()) ? (chars[i + 1]) : -1;
        }

        int indices[chars.size()];
        sort(suffixes.begin(), suffixes.end(), compareLT);

        for (int k = 4; k < 2 * chars.size(); k *= 2) {
            int rank = 0;
            int previousRank = suffixes[0].rank[0];
            suffixes[0].rank[0] = rank;
            indices[suffixes[0].index] = 0;

            for (int i = 1; i < chars.size(); i++) {
                if (suffixes[i].rank[0] == previousRank && suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                    previousRank = suffixes[i].rank[0];
                    suffixes[i].rank[0] = rank;
                } else {
                    previousRank = suffixes[i].rank[0];
                    suffixes[i].rank[0] = ++rank;
                }
                indices[suffixes[i].index] = i;
            }

            for (int i = 0; i < chars.size(); i++) {
                int nextIndex = suffixes[i].index + k / 2;
                suffixes[i].rank[1] = (nextIndex < chars.size()) ?
                                      suffixes[indices[nextIndex]].rank[0] : -1;
            }

            sort(suffixes.begin(), suffixes.end(), compareLT);
        }
        suffixArray = vector<int>(chars.size());
        for (int i = 0; i < chars.size(); i++)
            suffixArray[i] = suffixes[i].index;

        buildLcp();
    }

    void search(const vector<string> &patterns, bool count, bool print) override {

    }

    vector<char> serialize(bool verbose) override {
        return vector<char>();
    }

    void deserialize(const vector<char> &bytes, bool verbose) override {

    }

private:
    void buildLcp() {
        lcp = vector<int>(chars.size(), 0);
        vector<int> inverseSuffixArray(chars.size(), 0);

        for (int i = 0; i < chars.size(); i++)
            inverseSuffixArray[suffixArray[i]] = i;

        int k = 0;

        for (int i = 0; i < chars.size(); i++) {
            if (inverseSuffixArray[i] == chars.size() - 1) {
                k = 0;
                continue;
            }
            int j = suffixArray[inverseSuffixArray[i] + 1];
            while (i + k < chars.size() && j + k < chars.size() && chars[i + k] == chars[j + k])
                k++;

            lcp[inverseSuffixArray[i]] = k;

            if (k > 0) k--;
        }
    }
};