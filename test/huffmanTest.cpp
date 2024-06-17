#include "../inc/huffman.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void encode_test(string& input_file,string& encoded_output_file) {

    // Iniciamos cronometro
    auto start_encode = chrono::high_resolution_clock::now();

    // Codificar archivo de entrada
    cout << input_file.substr(8)<< ";";
    encode_file(input_file, encoded_output_file);

    /* Calculamos el tiempo transcurrido */
    auto end_encode = chrono::high_resolution_clock::now();
    auto encode_duration = chrono::duration_cast<chrono::milliseconds>(end_encode - start_encode).count();

    // Calcular tamaño del archivo codificado
    ifstream encoded_file(encoded_output_file, ios::binary | ios::ate);
    if (!encoded_file.is_open()) {
        cerr << "Error al abrir archivo codificado: " << encoded_output_file << endl;
        return;
    }
    size_t encoded_size = encoded_file.tellg();
    encoded_file.close();

    //imprimimos el tamaño del archivo codificado y el tiempo de codificación
    cout << encoded_size  << ";" << encode_duration << std::endl;
}

int main(int argc, char* argv[]) {

     if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    
    // Archivos a codificar obtenidos de truncate.cpp
    vector<string> filenames = {
        "../data/" + input_filename + "_1kB",
        "../data/" + input_filename + "_10kB",
        "../data/" + input_filename + "_100kB",
        "../data/" + input_filename + "_500kB",
        "../data/" + input_filename + "_1MB",
        "../data/" + input_filename + "_5MB",
        "../data/" + input_filename + "_10MB",
        "../data/" + input_filename + "_50MB",
        "../data/" + input_filename + "_100MB"
    };

    for (auto& name : filenames) {
        string file = name + ".txt";
        string encode_output_file = name + "_compressed.txt";
        
        encode_test(file, encode_output_file);
    }

    return 0;
}
