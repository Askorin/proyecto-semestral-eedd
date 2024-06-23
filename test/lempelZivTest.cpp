#include "../inc/lempelZiv.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

bool lempelZiv_functional_test(string& input_file, int window_size, string&  compress_output_file,
                             string& decompress_output_file) {

    std::cout << "\t- Testeando con " << input_file << '\n';
    // Abrir el archivo
    std::ifstream archivo(input_file);
    std::string content;

    std::stringstream buffer;
    buffer << archivo.rdbuf();
    content = buffer.str();

    comprimir(content, window_size, compress_output_file);
    descomprimir(compress_output_file, decompress_output_file);

    std::fstream fin(input_file, std::fstream::in | std::fstream::binary);
    std::fstream decompressed_fin(decompress_output_file,
                             std::fstream::in | std::fstream::binary);
    unsigned char c_in;
    unsigned char c_in_decompressed;
    /* Hasta 4GB */
    size_t byte_count_in = 0;
    size_t byte_count_decompressed = 0;
    while (fin.read(reinterpret_cast<char*>(&c_in), sizeof(c_in))) {

        ++byte_count_in;
        if (decompressed_fin.read(reinterpret_cast<char*>(&c_in_decompressed), sizeof(c_in_decompressed))) {
            ++byte_count_decompressed;
        }

        if (c_in != c_in_decompressed) {
            std::cout << "Byte número " << byte_count_in << " del input difiere\n";
            return false;
        }
    }
    
    if (byte_count_in != byte_count_decompressed) {
        std::cout << "Tamaño de los archivos difiere\n";
        return false;
    }

    return true;
}

void compress_test(string input_file, int window_size, string compressed_output_file) {

    // Abrir el archivo
    std::ifstream archivo(input_file);
    std::string content;

    std::stringstream buffer;
    buffer << archivo.rdbuf();
    content = buffer.str();

    // std::cout << "Largo:" << content.length() << std::endl;
    
    // Iniciamos cronometro
    auto start_compress = chrono::high_resolution_clock::now();

    // Comprimir archivo de entrada
    comprimir(content, window_size, compressed_output_file);

    /* Calculamos el tiempo transcurrido */
    auto end_compress = chrono::high_resolution_clock::now();
    auto compress_duration =
        chrono::duration_cast<chrono::milliseconds>(end_compress - start_compress)
            .count();

    // Calcular tamaño del archivo codificado
    ifstream compressed_file(compressed_output_file, ios::binary | ios::ate);
    if (!compressed_file.is_open()) {
        cerr << "Error al abrir archivo codificado: " << compressed_output_file
             << endl;
        return;
    }
    size_t compressed_size = compressed_file.tellg();
    compressed_file.close();

    // imprimimos el nombnre del archivo, el tamaño del archivo comprimido y el tiempo de compresión
    cout << input_file.substr(5) << ";" << compressed_size << ";" << compress_duration << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];

    // Archivos a codificar obtenidos de truncate.cpp + windowsize de busqueda
    vector<pair<string, int>> filenames = {
        {"data/" + input_filename + "_1kB", 512},
        {"data/" + input_filename + "_10kB", 512},
        {"data/" + input_filename + "_100kB", 512},
        {"data/" + input_filename + "_500kB", 512},
        {"data/" + input_filename + "_1MB", 32768},
        {"data/" + input_filename + "_5MB", 32768},
        {"data/" + input_filename + "_10MB", 32768},
        {"data/" + input_filename + "_50MB", 32768},
        {"data/" + input_filename + "_100MB", 32768}
    };

    // std::cout << "###  Testeo Funcional de LempelZiv ###\n";
    // bool lempelZiv_works = true;
    // /* Hasta 1MB porque da lata esperar */
    // size_t count = 0;
    // for (auto& file : filenames) {
    //     if (count == 5)
    //         break;
    //     string name = file.first + ".txt";
    //     string compress_output_file = file.first + "_compressed";
    //     string decompress_output_file = file.first + "_decompressed.txt";
    //     bool test_results = lempelZiv_functional_test(name, file.second, compress_output_file,
    //                                                 decompress_output_file);
    //     ++count;
    //     if (!test_results)
    //         lempelZiv_works = test_results;
    // }

    // if (lempelZiv_works)
    //     std::cout << "\n### [ O ] Tests Funcionales de lempelZiv Aprobados ###\n";
    // else {
    //     std::cout << "\n### [ X ] Error! Tests Funcionales de lempelZiv "
    //                  "REPROBADOS ###\n";
    //     return 1;
    // }

    for (auto& name : filenames) {
        string file = name.first + ".txt";
        string compress_output_file = name.first + "_compressed";

        compress_test(file, name.second, compress_output_file);
    }

    return 0;
}

