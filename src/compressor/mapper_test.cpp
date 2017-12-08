#include <ctime>
#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include "Compressor.h"
#include "CompressorMapper.h"
#include "Lz77.h"
#include "Lz78.h"
#include "../util/FileUtils.h"

using namespace std;

#define DEBUG 0

int main(int argc, char *argv[]) {
    Compressor *lz77 = new Lz77();
    Compressor *lz78 = new Lz78();

    assert(CompressorMapper::getCodeFromCompressor(lz77) == 77);
    assert(CompressorMapper::getCodeFromCompressor(lz78) == 78);

    return 0;
}
