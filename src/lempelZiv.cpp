#include "../inc/lempelZiv.h"
#include "../inc/suffixTree.h"

#include <iostream>

encodedPair::encodedPair(int pos, int length = 0, encodedPair* pair = NULL) {
    this->pos = pos;
    this->length = length;
    this->next = pair;
}

std::ostream& operator<<(std::ostream& os, encodedPair* pair){
    encodedPair* actual_pair = pair;
    do {
        if (actual_pair->length) {
            std::cout << "(" << actual_pair->pos << ", " << actual_pair->length << ") ";    
        } else {
            std::cout << "(" << char(actual_pair->pos) << ", " << actual_pair->length << ") ";
        }
        actual_pair = actual_pair->next;
    } while (actual_pair != NULL);
    return os;
}

encodedPair* comprimir(std::string string) {
    const bool VERBOSE = false;
    suffixTree tree(string, false);
    int index = 1;
    encodedPair* first = new encodedPair((int) string[0], 0); 
    encodedPair* actual_pair = first;     
    while (index < (int) string.length()) {
        std::string repeated_text("");
        int starts_at;
        do {
            repeated_text.push_back(string[index + repeated_text.length()]);
            if (VERBOSE) std::cout << "Buscando substring: " << repeated_text << "\n";
            auto search = tree.search_substring(repeated_text);
            if (search.first >= index || (int) (search.first + repeated_text.length()) > index) {
                repeated_text.pop_back();
                if (repeated_text.empty()) {
                    if (VERBOSE) std::cout << "Caracter " << string[index] << " en índice " << index << " no aparece antes\n";
                    encodedPair* new_pair = new encodedPair((int) string[index], 0);
                    actual_pair->next = new_pair;
                    actual_pair = new_pair;
                    index++;
                    break;
                }
                if (VERBOSE) std::cout << "No aparece antes, se usará " << repeated_text << " que empieza en indice " << starts_at << "\n";
                encodedPair* new_pair = new encodedPair(starts_at, repeated_text.length());
                actual_pair->next = new_pair;
                actual_pair = new_pair;
                index += repeated_text.length();
                break;
            }
            if (VERBOSE) std::cout << "Si aparece antes, en pos " << search.first << "\n";
            starts_at = search.first;
            if (index + repeated_text.length() >= string.length()) {
                encodedPair* new_pair = new encodedPair(starts_at, repeated_text.length());
                actual_pair->next = new_pair;
                actual_pair = new_pair;
                index += repeated_text.length();
                break; 
            }
        } while (index < (int) string.length());

    }
    return first;
}
