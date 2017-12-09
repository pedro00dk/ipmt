#pragma once

#include <fstream>
#include <vector>

using namespace std;

class FileUtils {

public:
    static vector<char> readBytes(ifstream &fileStream) {
        vector<char> bytes((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>());
        fileStream.close();
        return bytes;
    }

    static void writeBytes(ofstream &fileStream, vector<char> &bytes) {
        fileStream.write(&bytes[0], bytes.size());
        fileStream.close();
    }

    static void checkFile(ifstream &fileStream, string filename) {
      if (!fileStream.is_open()) {
          cerr << "ERR: can not open file " << filename << endl;
          exit(1);
      }
    }
};
