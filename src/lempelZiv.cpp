#include "../inc/lempelZiv.h"
#include "../inc/suffixTree.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>

/*
Función que acepta un string (texto a comprimir), 
un int window_size que indica cuantos caracteres abarca la búsqueda de substrings 
y un string con nombre de archivo.
*/ 

void guardar_par(position_type pos, length_type length, std::ofstream& stream) {
    stream.write((char*) &pos, sizeof(position_type));
    stream.write((char*) &length, sizeof(length_type));
}

void comprimir(std::string string, int window_size, std::string filename) {
    // VERBOSE activa el texto de debug para la compresión, y TREE_VERBOSE el texto debug de la creación de árboles.
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
    output.open(filename, std::ios_base::binary | std::ios_base::trunc);
    guardar_par((position_type) string[0], 0, output);
    //output << (int) string[0] << " 0\n";
    int index = 1;
    while (index < (int) string.length()) {
        // Por cada substring crear repeated_text, que indica el substring repetido.
        std::string repeated_text("");
        position_type starts_at;
        do {
            // Agregar el siguiente caracter a repeated_text
            repeated_text.push_back(string[index + repeated_text.length()]);
            if (VERBOSE) std::cout << "Buscando substring: " << repeated_text << "\n";
            auto search = tree->search_substring(repeated_text);
            if (VERBOSE) std::cout << "SUMA: " << search.first + suffix_tree_pos << ", INDEX:  " << index << "\n";
            // Si la posición en donde se encuentra tal substring es la misma o mayor que el índice actual, o el substring sobrepasa el índice actual o el fin del texto;
            if (search.first + suffix_tree_pos >= index || (int) (search.first + suffix_tree_pos + repeated_text.length()) > index || index + (int) repeated_text.length() >= suffix_tree_pos + window_size) {
                // Eliminar el caracter de repeated_text;
                repeated_text.pop_back();
                // Si está vacío significa que el caracter anterior no aparece antes, por tanto se agrega.
                if (repeated_text.empty()) {
                    if (VERBOSE) std::cout << "Caracter " << string[index] << " en índice " << index << " no aparece antes\n";
                    guardar_par((position_type) string[index], 0, output);
                    //output << (int) string[index] << " 0\n"; 
                    index++;
                    break;
                }
                // De otra forma, se indica que se repite el substring repeated_text.
                if (VERBOSE) std::cout << "No aparece antes, se usará " << repeated_text << " que empieza en indice " << starts_at << "\n";
                guardar_par(starts_at, (length_type) repeated_text.length(), output);
                //output << starts_at << " " << repeated_text.length() << "\n"; 
                index += repeated_text.length();
                break;
            }
            // Como encontró el substring antes en el texto, modifica starts_at con su posición y se repite el loop.
            if (VERBOSE) std::cout << "Si aparece antes, en pos " << search.first + suffix_tree_pos << "\n";
            starts_at = search.first + suffix_tree_pos;
            // Si sobrepasa el texto sale del loop y guarda el substring.
            if (index + repeated_text.length() >= string.length()) {
                guardar_par(starts_at, (length_type) repeated_text.length(), output);
                //output << starts_at << " " << repeated_text.length() << "\n"; 
                index += repeated_text.length();
                break; 
            }
        } while (index < (int) string.length());
        // Si el indice sobrepasa el "alcance" del suffix tree se crea otro con un rango más adelante.
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

void descomprimir(std::string filename, std::string output_filename) {
    std::string string;
    std::ifstream compressed_file;
    compressed_file.open(filename, std::ios_base::binary);
    
    position_type pos;
    length_type length;

    compressed_file.read((char*) &pos, sizeof(position_type));
    compressed_file.read((char*) &length, sizeof(length_type));

    // Mientras lea el archivo, obtener las variables.
    while (compressed_file.good()){
        if (length) {
            string.append(string.substr(pos, length));
        } else {
            string.push_back((char) pos);
        }
        compressed_file.read((char*) &pos, sizeof(position_type));
        compressed_file.read((char*) &length, sizeof(length_type));
    }
    std::ofstream fout(output_filename, std::ofstream::out |
                                             std::ofstream::trunc |
                                             std::ofstream::binary);
    fout << string;
    fout.close();
}

/*
int main() {
    const int CHAR_COUNT = 1000000;

    std::ifstream input;
    input.open("data/english.100MB");
    
    std::string text;

    char buffer[1000];

    input.get(buffer, 1000, char(200));

    while (input.good()) {
        text.append(buffer);
        input.get(buffer, 1000, char(200));
    }

    text = text.substr(0, CHAR_COUNT);

    comprimir(text, 125000, "compresion.txt");

    std::string descom = descomprimir("compresion.txt");

    std::cout << descom << "\n";
}
*/
