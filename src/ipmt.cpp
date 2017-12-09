#include <iostream>
#include "cli/Cli.h"
#include "compressor/Compressor.h"
#include "compressor/Lz77.h"
#include "indexer/Indexer.h"
#include "indexer/SuffixArray.h"
#include "indexer/SuffixTree.h"
#include "util/FileUtils.h"

using namespace std;

int main(int argc, char **argv) {
    CliOptions options = parseCommand(argc, argv);

    Indexer *indexer = nullptr;
    if (options.indexAlgorithm == "suffixtree") indexer = new SuffixTree();
    if (options.indexAlgorithm == "suffixarray") indexer = new SuffixArray();
    else {
        cerr << "ERR: indexer not implemented" << endl;
        exit(1);
    }

    Compressor *compressor = nullptr;
    if (options.compressionAlgorithm == "lz77") compressor = new Lz77();
    else if (options.compressionAlgorithm == "uncompressed") compressor = nullptr;
    else {
        cerr << "ERR: compressor not implemented" << endl;
        exit(1);
    }

    // Processing files
    for (string &file : options.files) {
        ifstream fileStream(file);
        if (!fileStream.is_open()) {
            cerr << "ERR: can not open file " << file << endl;
            continue;
        }

        if (options.isIndex) {
            vector<char> fileBytes = FileUtils::readBytes(fileStream);
            indexer->buildIndex(fileBytes, options.verbose);
            vector<char> indexBytes = indexer->serialize(options.verbose);
            string indexFile = file + ".idx";
            ofstream fileOutStream = ofstream(indexFile);
            if (compressor != nullptr) {
                vector<char> encodedIndexBytes = compressor->encode(&indexBytes[0], indexBytes.size());
                FileUtils::writeBytes(fileOutStream, encodedIndexBytes);
            } else {
                FileUtils::writeBytes(fileOutStream, indexBytes);
            }
        } else if (options.isSearch) {
            vector<char> fileBytes = FileUtils::readBytes(fileStream);
            vector<char> indexBytes;
            if (compressor != nullptr) {
                indexBytes = compressor->decode(&fileBytes[0], fileBytes.size());
            } else {
                indexBytes = fileBytes;
            }
            indexer->deserialize(indexBytes, options.verbose);
            indexer->search(options.patterns, options.count, !options.count);
        }
    }
}
