#pragma once

#include <algorithm>
#include <cmath>
#include <sstream>
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
        if (verbose) cout << "SUFFIX ARRAY Indexer" << endl << "size: " << chars.size() << endl;

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
            if (verbose) cout << "\r  at: " << k << flush;

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

        if (verbose) cout << endl << "Indexer finished" << endl;

        suffixArray = vector<int>(chars.size());
        for (int i = 0; i < chars.size(); i++)
            suffixArray[i] = suffixes[i].index;

        buildLcp();
    }

    void search(const vector<string> &patterns, bool count, bool print) override {
        int charsSize = chars.size();
        for (int i = 0; i < patterns.size(); i++) {
            string pattern = patterns[i];
            int patternSize = pattern.size();

            //
            int sucessor;

            if (partialLexComp(&chars[0], suffixArray[charsSize - 1], charsSize, &pattern[0], 0, patternSize,
                               patternSize) <= 0) {
                sucessor = charsSize - 1;
            } else if (partialLexComp(&pattern[0], 0, patternSize, &chars[0], suffixArray[0], charsSize,
                                      patternSize) < 0) {
                sucessor = -1;
            } else {
                int l = 0;
                int r = charsSize - 1;
                while (r - l > 1) {
                    int h = (l + r) / 2;
                    if (partialLexComp(&chars[0], suffixArray[h], charsSize, &pattern[0], 0, patternSize,
                                       patternSize) <= 0) {
                        l = h;
                    } else {
                        r = h;
                    }
                }
                sucessor = l;
            }

            int predecessor;
            if (partialLexComp(&pattern[0], 0, patternSize, &chars[0], suffixArray[0], charsSize, patternSize) <= 0) {
                predecessor = 0;
            } else if (partialLexComp(&chars[0], suffixArray[charsSize - 1], charsSize, &pattern[0], 0, patternSize,
                                      patternSize) < 0) {
                predecessor = charsSize;
            } else {
                int l = 0;
                int r = charsSize - 1;
                while (r - l > 1) {
                    int h = (l + r) / 2;
                    if (partialLexComp(&pattern[0], 0, patternSize, &chars[0], suffixArray[h], charsSize,
                                       patternSize) <= 0) {
                        r = h;
                    } else {
                        l = h;
                    }
                }
                predecessor = r;
            }

            if (sucessor < predecessor) {
                if (count) cout << pattern << ": " << 0 << endl;
            } else {
                int occurrences = sucessor - predecessor + 1;
                if (print) inspectPrintOccurrences(predecessor, sucessor);
                if (count) cout << pattern << ": " << occurrences << endl;
            }

        }
    }

    int partialLexComp2(const string &pattern, int charsFrom, int charsTo, int max) {
        int minimumMax = min((int) pattern.size(), (charsTo - charsFrom));
        minimumMax = min(max, minimumMax);
        for (int i = 0; i < minimumMax; i++) {
            if (pattern[i] < chars[charsFrom + i]) return -1;
            if (pattern[i] > chars[charsFrom + i]) return 1;
        }
        if (minimumMax == max || pattern.size() == charsTo - charsFrom) return 0;
        else if (pattern.size() < charsTo - charsFrom) return -1;
        else return 1;
    }

    int partialLexComp(const char *s0, int f0, int t0, const char *s1, int f1, int t1, int max) {
        int minimumMax = min(max, min(t0 - f0, t1 - f1));
        for (int i = 0; i < minimumMax; i++) {
            if (s0[f0 + i] < s1[f1 + i]) return -1;
            if (s0[f0 + i] > s1[f1 + i]) return 1;
        }
        if (minimumMax == max) return 0;
        if (t0 - f0 < t1 - f1) return -1;
        if (t0 - f0 > t1 - f1) return 1;
        else return 0;
    }

    void inspectPrintOccurrences(int left, int right) {
        for (int i = left; i <= right; i++) {
            int patternStartPosition = suffixArray[i];
            int lineBegin = patternStartPosition;

            while (lineBegin > 0) {
                if (chars[lineBegin] == '\n') {
                    lineBegin++;
                    break;
                }
                lineBegin--;
            }
            int lineEnd = patternStartPosition;
            while (lineEnd < chars.size()) {
                if (chars[lineEnd] == '\n') {
                    lineEnd--;
                    break;
                }
                lineEnd++;
            }
            string line(chars.begin() + lineBegin, chars.begin() + lineEnd + 1);
            cout << line << endl;
        }
    }

    vector<char> serialize(bool verbose) override {
        if (verbose)
            cout << "SUFFIX ARRAY Serializer" << endl << "size: " << chars.size() << endl;

        vector<char> bytes;

        string size = to_string(chars.size()) + "\n";
        bytes.insert(end(bytes), begin(size), end(size));

        bytes.push_back('=');
        bytes.push_back('\n');

        if (verbose) cout << "serializing suffix array" << endl;
        for (int i = 0; i < chars.size(); i++) {
            string value = to_string(suffixArray[i]) + "\n";
            bytes.insert(end(bytes), begin(value), end(value));
        }
        bytes.push_back('=');
        bytes.push_back('\n');
        if (verbose) cout << "serializing lcp" << endl;
        for (int i = 0; i < chars.size(); i++) {
            string value = to_string(lcp[i]) + "\n";
            bytes.insert(end(bytes), begin(value), end(value));
        }
        bytes.push_back('=');
        bytes.push_back('\n');

        if (verbose) cout << "serializing text" << endl;
        copy(chars.begin(), chars.end(), back_inserter(bytes));

        if (verbose) cout << "Serializer finished - bytes: " << bytes.size() << endl;

        return bytes;
    }

    void deserialize(const vector<char> &bytes, bool verbose) override {
        stringstream stream;
        stream.write(&bytes[0], bytes.size());

        string line;
        getline(stream, line);
        int size = stoi(line);
        getline(stream, line); // =

        suffixArray = vector<int>(size);
        lcp = vector<int>(size);
        chars = vector<char>(size);

        bool inSuffixArray = true;
        int i = 0;
        for (; getline(stream, line);) {
            if (inSuffixArray) {
                if (i < size) suffixArray[i++] = stoi(line);
                else {
                    inSuffixArray = false;
                    i = 0;
                }
            } else {
                if (i < size) lcp[i++] = stoi(line);
                else {
                    break;
                }
            }
        }

        stream >> std::noskipws;
        chars = vector<char>((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
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