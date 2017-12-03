#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../util/FileUtils.h"
#include "../compressor/Compressor.h"
#include "../indexer/Indexer.h"

using namespace std;

class IndexMode {

public:
    static void index(string filename, istream &inputFile, Compressor compressor) {
        vector<char> inputFileBytes = FileUtils::readBytes(compressedIndexFile);
        Indexer *indexer; // = new SuffixArray();
        indexer->buildIndex(&inputFileBytes[0], inputFileBytes.size());
        vector<char> indexBytes = indexer->getIndexBytes();
        vector<char> encodedIndexBytes = compressor.encode(&indexBytes[0], indexBytes.size());
        FileUtils::writeBytes(filename, encodedIndexBytes);
    }
};
