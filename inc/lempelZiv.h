#ifndef LEMPEL_ZIV_H
#define LEMPEL_ZIV_H

struct encodedPair {
    int pos;
    int length;
    encodedPair* next;
    encodedPair();
    encodedPair(int);
    encodedPair(int, int);
    encodedPair(int, int, encodedPair*);
};

encodedPair* comprimir(char*);
char* descomprimir(encodedPair*);

#endif
