#include <ctime>
#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include "Compressor.h"
#include "Lz77.h"
#include "Lz78.h"
#include "NoCompressor.h"
#include "../util/FileUtils.h"

using namespace std;

#define DEBUG 0

int main(int argc, char *argv[]) {
    Compressor *compressor = nullptr;
    string compressorName = argv[1];
    if (compressorName.compare("lz77") == 0) compressor = new Lz77();
    else if (compressorName.compare("lz78") == 0) compressor = new Lz78();
    else if (compressorName.compare("uncompressed") == 0) compressor = new NoCompressor();
    else {
        puts("invalid compressor");
        return 1;
    }

    string filename = argv[2];
    cout << filename << endl;

    if (string(argv[3]).compare("compress") == 0) {
        int t0 = time(NULL);
        ifstream fileStream(filename);
        FileUtils::checkFile(fileStream, filename);

        vector<char> inputVector = FileUtils::readBytes(fileStream);
        vector<char> encoded = compressor->encode(&inputVector[0], inputVector.size());

        ofstream fileOutStream = ofstream(filename + ".myzip");
        FileUtils::writeBytes(fileOutStream, encoded);

        int t1 = time(NULL);
        printf("encoded size:%lu original size:%lu compression:%lf\n",
               encoded.size(),
               inputVector.size(),
               double(encoded.size()) / inputVector.size());
        printf("compress time = %d secs\n", t1 - t0);

    } else if (string(argv[3]).compare("decompress") == 0) {
        int t0 = time(NULL);
        ifstream fileStream(filename);
        FileUtils::checkFile(fileStream, filename);

        vector<char> inputVector = FileUtils::readBytes(fileStream);
        vector<char> decoded = compressor->decode(&inputVector[0], inputVector.size());
        int t1 = time(NULL);

        printf("decompress time = %d secs\n", t1 - t0);
    } else if (string(argv[3]).compare("validate") == 0) {
        int t0 = time(NULL);
        ifstream fileStream(filename);
        FileUtils::checkFile(fileStream, filename);

        vector<char> inputVector = FileUtils::readBytes(fileStream);
        vector<char> encoded = compressor->encode(&inputVector[0], inputVector.size());
        vector<char> decoded = compressor->decode(&encoded[0], encoded.size());

        //ofstream fileOutStream = ofstream(filename + ".myzip");
        //FileUtils::writeBytes(fileOutStream, decoded);

        for (int i = 0; i < decoded.size(); i++) {
            assert(decoded[i] == inputVector[i]);
        }

        int t1 = time(NULL);
        printf("original size:%lu decoded size:%lu encoded size:%lu compression rate:%lf time:%d\n",
        inputVector.size(), decoded.size(), encoded.size(), encoded.size()/double(inputVector.size()), t1 - t0);
    } else {
        puts("invalid mode");
        return 1;
    }
    return 0;
}
