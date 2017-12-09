#pragma once

#include <iostream>
#include "Lz77.h"
#include "Lz78.h"

using namespace std;

class CompressorMapper {

public:
    static int getCodeFromCompressor(Compressor *compressor) {
      return compressor->getCompressorCode();
    }

    static Compressor * getCompressorFromCode(int code) {
        switch (code){
          case (0):
            return nullptr;
            break;
          case (77):
            return new Lz77();
            break;
          case (78):
            return new Lz78();
            break;
        }
        cerr << "invalid compressor code";
        exit(1);
    }
};
