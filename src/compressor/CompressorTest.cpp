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
        //error
    }

    string filename = argv[2];
    cout << filename << endl;
    vector<char> encoded;
    vector<char> decoded;

    int t0 = time(NULL);
    vector<char> inputVector;
    inputVector = FileUtils::readBytes(filename);

    encoded = compressor->encode(&inputVector[0], inputVector.size());
    decoded = compressor->decode(&encoded[0], encoded.size());

    assert(inputVector.size() == decoded.size());
    for (int i = 0; i < decoded.size(); i++) assert(inputVector[i] == decoded[i]);
    FileUtils::writeBytes(filename + ".myzip", encoded);

    int t1 = time(NULL);
    printf("encoded size:%d decoded size:%d compression:%lf\n",
           encoded.size(),
           decoded.size(),
           double(encoded.size()) / decoded.size());
    printf("time = %d secs\n", t1 - t0);

    return 0;
}