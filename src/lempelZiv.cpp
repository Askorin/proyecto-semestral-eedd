#include "../inc/lempelZiv.h"
#include "../inc/suffixTree.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>

void comprimir(std::string string, int window_size, std::string filename) {
    const bool VERBOSE = false;
    const bool TREE_VERBOSE = false;
    std::ofstream output;
    int suffix_tree_pos = 0;
    suffixTree* tree = NULL;
    if ((int) string.length() > window_size) {
        if (VERBOSE) std::cout << "Creando nuevo árbol que empieza en 0 y termina en " << window_size << "\n";
        tree = new suffixTree(string.substr(0, window_size), TREE_VERBOSE);
    } else {
        if (VERBOSE) std::cout << "Creando nuevo árbol que usa el string entero de tamaño " << string.length() << "\n";
        tree = new suffixTree(string, TREE_VERBOSE);
    }
    output.open(filename, std::ios_base::trunc);
    output << (int) string[0] << " 0\n";
    int index = 1;
    while (index < (int) string.length()) {
        std::string repeated_text("");
        int starts_at;
        do {
            repeated_text.push_back(string[index + repeated_text.length()]);
            if (VERBOSE) std::cout << "Buscando substring: " << repeated_text << "\n";
            auto search = tree->search_substring(repeated_text);
            if (VERBOSE) std::cout << "SUMA: " << search.first + suffix_tree_pos << ", INDEX:  " << index << "\n";
            if (search.first + suffix_tree_pos >= index || (int) (search.first + suffix_tree_pos + repeated_text.length()) > index || index + (int) repeated_text.length() >= suffix_tree_pos + window_size) {
                repeated_text.pop_back();
                if (repeated_text.empty()) {
                    if (VERBOSE) std::cout << "Caracter " << string[index] << " en índice " << index << " no aparece antes\n";
                    output << (int) string[index] << " 0\n"; 
                    index++;
                    break;
                }
                if (VERBOSE) std::cout << "No aparece antes, se usará " << repeated_text << " que empieza en indice " << starts_at << "\n";
                output << starts_at << " " << repeated_text.length() << "\n"; 
                index += repeated_text.length();
                break;
            }
            if (VERBOSE) std::cout << "Si aparece antes, en pos " << search.first + suffix_tree_pos << "\n";
            starts_at = search.first + suffix_tree_pos;
            if (index + repeated_text.length() >= string.length()) {
                output << starts_at << " " << repeated_text.length() << "\n"; 
                index += repeated_text.length();
                break; 
            }
        } while (index < (int) string.length());
        if (index >= suffix_tree_pos + window_size) {
            delete tree;
            suffix_tree_pos += window_size / 4;
            if (VERBOSE) std::cout << "Creando nuevo árbol que empieza en " << suffix_tree_pos << " y termina en " << window_size + suffix_tree_pos << "\n";
            if (VERBOSE) std::cout << "Substring: " << string.substr(suffix_tree_pos, window_size) << "\n";
            tree = new suffixTree(string.substr(suffix_tree_pos, window_size), TREE_VERBOSE);
        }
    }
    delete tree;
}

std::string descomprimir(std::string filename) {
    std::string string;
    std::ifstream compressed_file;
    compressed_file.open(filename);

    char buffer[100];
    compressed_file.getline(buffer, 100);

    while (compressed_file.good()) {
        char* pos_array = strtok(buffer, " ");
        char* length_array = strtok(NULL, " ");

        int pos = atoi(pos_array);
        int length = atoi(length_array);
        if (length) {
            string.append(string.substr(pos, length));
        } else {
            string.push_back((char) pos);
        }

        compressed_file.getline(buffer, 100);
    }
    return string;
}

int main() {
    const int CHAR_COUNT = 1000000;

    std::ifstream input;
    input.open("data/english100MB.txt");
    
    std::string text;

    char buffer[1000];

    input.get(buffer, 1000, char(200));

    while (input.good()) {
        text.append(buffer);
        input.get(buffer, 1000, char(200));
    }

    text = text.substr(0, CHAR_COUNT);

    comprimir(text, 100000, "compresion.txt");
}
