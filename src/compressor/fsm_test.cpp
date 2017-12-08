#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include "Compressor.h"
#include "Lz77.h"

using namespace std;

#define DEBUG 0


// LZ77
int main() {

    Lz77 lz77;

    string str = "abacaba";
    vector<vector<int>> v = lz77.build_fsm(str.c_str(), 7);

    for (int i = 0; i < str.size(); i++) {
        printf("i:%d ",i);
        for (int c = 'a'; c <= 'd'; c++) {
            printf("(%c -> %d) ", c, v[i][c]);
        }
        puts("");
    }

    return 0;
}