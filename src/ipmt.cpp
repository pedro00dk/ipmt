#include <iostream>
#include "cli/Cli.h"
#include "compressor/Compressor.h"
#include "compressor/Lz77.h"

using namespace std;

int main(int argc, char **argv) {

    CliOptions options = parseCommand(argc, argv);

    Compressor *compressor = nullptr;
    if (options.compressionAlgorithm == "lz77") compressor = new Lz77();
    else {
        cerr << "ERR: matcher not implemented" << endl;
        exit(1);
    }

    // Processing files
    for (auto &file : options.files) {
        ifstream inputFileStream(file);
        if (!inputFileStream.is_open()) {
            cerr << "ERR: can not open file " << file << endl;
            continue;
        }

        if (options.isIndex) {

        } else if (options.isSearch) {
//            matcher->searchPatterns(file, inputFileStream, options.count, !options.count);
        }
    }
}
