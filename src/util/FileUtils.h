#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define ISO_SIZE 256

class FileUtils {

public:
    static vector<char> readBytes(string filename) {
        ifstream ifs(filename, ios::binary | ios::ate);
        return readBytes(ifs);
    }

    static vector<char> readBytes(ifstream ifs) {
        ifstream::pos_type pos = ifs.tellg();

        std::vector<char> result(pos);

        ifs.seekg(0, ios::beg);
        ifs.read(&result[0], pos);

        return result;
    }

    static void writeBytes(string filename, vector<char> bytes) {
        ofstream outputBuffer(filename, ios::out | ios::binary);

        outputBuffer.write(&bytes[0], bytes.size());

        outputBuffer.close();
    }
};
