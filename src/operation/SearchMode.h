#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../util/FileUtils.h"
#include "../indexer/Indexer.h"

using namespace std;

class SearchMode {

public:
    static void searchPatterns(istream &compressedIndexFile, vector<string> patterns, bool count, bool print) {
        vector<char> compressedIndexBytes = FileUtils::readBytes(compressedIndexFile);
        vector<char> indexBytes = compressor.decode(&compressedIndexBytes[0], compressedIndexBytes.size());
        Indexer *indexer; // = new SuffixArray();
        indexer->rebuildIndex(indexBytes);
        indexer->search(patterns, count, print);
    }
};
