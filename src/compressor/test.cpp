#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include "Compressor.h"
#include "Lz77.h"

using namespace std;

#define DEBUG 0

vector<char> readBytes(string filename) {
    ifstream ifs(filename, ios::binary | ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}

void writeBytes(string filename, vector<char> bytes) {
    ofstream outputBuffer(filename, ios::out | ios::binary);

    outputBuffer.write(&bytes[0], bytes.size());

    outputBuffer.close();
}

// LZ77
int main() {
//    int bufferSize = 255;
//    int lookSize = 255;

    Compressor *compressor = nullptr;
    compressor = new Lz77();

    string input;
    vector<char> encoded;
    vector<char> decoded;

    cin >> input;

    vector<char> inputVector;
    //inputVector = vector<char>(input.begin(), input.end());
    inputVector = readBytes(input);

    encoded = compressor->encode(&inputVector[0], inputVector.size());
    decoded = compressor->decode(&encoded[0], encoded.size());


    assert(inputVector.size() == decoded.size());
    for (int i = 0; i < decoded.size(); i++) assert(inputVector[i] == decoded[i]);
    writeBytes("out.txt", encoded);

    return 0;
}