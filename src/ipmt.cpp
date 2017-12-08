#include <iostream>
#include "cli/Cli.h"
#include "compressor/Compressor.h"
#include "compressor/CompressorMapper.h"
#include "compressor/Lz77.h"
#include "compressor/Lz78.h"
#include "indexer/Indexer.h"
#include "indexer/SuffixTree.h"
#include "util/FileUtils.h"

using namespace std;

int main(int argc, char **argv) {
    CliOptions options = parseCommand(argc, argv);

    Indexer *indexer = nullptr;
    if (options.indexAlgorithm == "suffixtree") indexer = new SuffixTree();
    else {
        cerr << "ERR: indexer not implemented" << endl;
        exit(1);
    }

    Compressor *compressor = nullptr;

    // Processing files
    for (string &file : options.files) {
        ifstream fileStream(file);
        if (!fileStream.is_open()) {
            cerr << "ERR: can not open file " << file << endl;
            exit(1);
        }
        vector<char> fileBytes = FileUtils::readBytes(fileStream);

        if (options.isIndex) {
            if (options.compressionAlgorithm == "lz77") compressor = new Lz77();
            else if (options.compressionAlgorithm == "lz78") compressor = new Lz78();
            else if (options.compressionAlgorithm == "uncompressed") compressor = nullptr;
            else {
                cerr << "ERR: compressor not implemented" << endl;
                exit(1);
            }

            indexer->buildIndex(fileBytes, options.verbose);
            vector<char> indexBytes = indexer->serialize(options.verbose);
            string indexFile = file + ".idx";
            ofstream fileOutStream = ofstream(indexFile);
            if (compressor != nullptr) {
                vector<char> encodedIndexBytes = compressor->encode(&indexBytes[0], indexBytes.size());
                int compressorCode = compressor->getCompressorCode();
                encodedIndexBytes.push_back(compressorCode);
                printf("last written byte:%d\n", compressorCode);
                FileUtils::writeBytes(fileOutStream, encodedIndexBytes);
            } else {
                printf("last written byte:%d\n", 0);
                indexBytes.push_back(0);
                FileUtils::writeBytes(fileOutStream, indexBytes);
            }
        } else if (options.isSearch) {
            puts("is search");
            vector<char> indexBytes;
            printf("last read byte:%d\n", fileBytes[fileBytes.size() - 1]);
            compressor = CompressorMapper::getCompressorFromCode(fileBytes[fileBytes.size() - 1]);
            if (compressor != nullptr) {
                puts("decoding");
                indexBytes = compressor->decode(&fileBytes[0], fileBytes.size());
                puts("decoded");
            } else {
                indexBytes = fileBytes;
            }
            puts("deserializing");
            indexer->deserialize(indexBytes, options.verbose);
            puts("searching");
            indexer->search(options.patterns, options.count, !options.count);
        }
    }
}
