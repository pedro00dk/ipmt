#include "SuffixTree.h"

int main() {
    char *str = const_cast<char *>("hello world!!");
    SuffixTree st;
    st.buildIndex(str, 13);
    printf("finished");
}