#include <ctime>
#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include "Compressor.h"
#include "Lz77.h"
#include "Lz78.h"
#include "../util/FileUtils.h"

using namespace std;

#define DEBUG 0

// LZ77
int main(int argc, char *argv[]) {
    Compressor *compressor = nullptr;
    string compressorName = argv[1];
    if (compressorName.compare("lz77") == 0) compressor = new Lz77();
    else if (compressorName.compare("lz78") == 0) compressor = new Lz78();
    else {
        puts("invalid compressor");
        return 1;
    }

    string filename = argv[2];
    cout << filename << endl;

    if (string(argv[3]).compare("compress") == 0) {
        int t0 = time(NULL);
        ifstream fileStream(filename);
        vector<char> inputVector = FileUtils::readBytes(fileStream);
        vector<char> encoded = compressor->encode(&inputVector[0], inputVector.size());

        ofstream fileOutStream = ofstream(filename + ".myzip");
        FileUtils::writeBytes(fileOutStream, encoded);

        int t1 = time(NULL);
        printf("encoded size:%d original size:%d compression:%lf\n",
               encoded.size(),
               inputVector.size(),
               double(encoded.size()) / inputVector.size());
        printf("time = %d secs\n", t1 - t0);

//    vector<char> decoded = compressor->decode(&encoded[0], encoded.size());
//    printf("decoded size:%d\n", decoded.size());
//    FileUtils::writeBytes(fileOutStream, decoded);
    } else if (string(argv[3]).compare("decompress") == 0) {
        int t0 = time(NULL);
        ifstream fileStream(filename + ".myzip");
        vector<char> inputVector = FileUtils::readBytes(fileStream);
        vector<char> decoded = compressor->decode(&inputVector[0], inputVector.size());
        int t1 = time(NULL);

        printf("time = %d secs\n", t1 - t0);
    } else {
        puts("invalid mode");
        return 1;
    }
    return 0;
}