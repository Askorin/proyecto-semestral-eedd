#ifndef LEMPEL_ZIV_H
#define LEMPEL_ZIV_H

#include <string>

struct encodedPair {
    int pos;
    int length;
    encodedPair* next;
    encodedPair(int, int, encodedPair*);
};

encodedPair* comprimir(std::string);
std::string descomprimir(encodedPair*);

#endif
