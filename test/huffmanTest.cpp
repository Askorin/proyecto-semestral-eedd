#include "../inc/huffman.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool hufmann_functional_test(string& input_file, string& encode_output_file,
                             string& decode_output_file) {
    std::cout << "\t- Testeando con " << input_file << '\n';
    encode_file(input_file, encode_output_file);
    decode_file(encode_output_file, decode_output_file);

    std::fstream fin(input_file, std::fstream::in | std::fstream::binary);
    std::fstream decoded_fin(decode_output_file,
                             std::fstream::in | std::fstream::binary);
    unsigned char c_in;
    unsigned char c_in_decoded;
    /* Hasta 4GB */
    size_t byte_count_in = 0;
    size_t byte_count_decoded = 0;
    while (fin.read(reinterpret_cast<char*>(&c_in), sizeof(c_in))) {

        ++byte_count_in;
        if (decoded_fin.read(reinterpret_cast<char*>(&c_in_decoded),
                             sizeof(c_in_decoded))) {
            ++byte_count_decoded;
        }

        if (c_in != c_in_decoded) {
            std::cout << "Byte número " << byte_count_in
                      << " del input difiere\n";
            return false;
        }
    }

    if (byte_count_in != byte_count_decoded) {
        std::cout << "Tamaño de los archivos difiere\n";
        return false;
    }

    return true;
}

void encode_test(string& input_file, string& encoded_output_file) {

    // Iniciamos cronometro
    auto start_encode = chrono::high_resolution_clock::now();

    // Codificar archivo de entrada
    cout << input_file.substr(8) << ";";
    encode_file(input_file, encoded_output_file);

    /* Calculamos el tiempo transcurrido */
    auto end_encode = chrono::high_resolution_clock::now();
    auto encode_duration =
        chrono::duration_cast<chrono::milliseconds>(end_encode - start_encode)
            .count();

    // Calcular tamaño del archivo codificado
    ifstream encoded_file(encoded_output_file, ios::binary | ios::ate);
    if (!encoded_file.is_open()) {
        cerr << "Error al abrir archivo codificado: " << encoded_output_file
             << endl;
        return;
    }
    size_t encoded_size = encoded_file.tellg();
    encoded_file.close();

    // imprimimos el tamaño del archivo codificado y el tiempo de codificación
    cout << encoded_size << ";" << encode_duration << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];

    // Archivos a codificar obtenidos de truncate.cpp
    vector<string> filenames = {"../data/" + input_filename + "_1kB",
                                "../data/" + input_filename + "_10kB",
                                "../data/" + input_filename + "_100kB",
                                "../data/" + input_filename + "_500kB",
                                "../data/" + input_filename + "_1MB",
                                "../data/" + input_filename + "_5MB",
                                "../data/" + input_filename + "_10MB",
                                "../data/" + input_filename + "_50MB",
                                "../data/" + input_filename + "_100MB"};

    std::cout << "### Testeo Funcional de Huffman ###\n";
    bool huffman_works = true;

    /* Probamos casos borde para huffman */
    vector<string> edge_cases = {"../data/one_byte", "../data/zero_byte"};
    for (auto& name : edge_cases) {
        string encode_output_file = name + "_compressed";
        string decode_output_file = name + "_decompressed";
        bool test_results = hufmann_functional_test(name, encode_output_file,
                                                    decode_output_file);

        if (!test_results)
            huffman_works = test_results;
    }

    /* Hasta 1MB porque da lata esperar */
    size_t count = 0;
    for (auto& name : filenames) {
        if (count == 5)
            break;
        string encode_output_file = name + "_compressed";
        string file = name + ".txt";
        string decode_output_file = name + "_decompressed.txt";
        bool test_results = hufmann_functional_test(file, encode_output_file,
                                                    decode_output_file);
        ++count;
        if (!test_results)
            huffman_works = test_results;
    }

    if (huffman_works)
        std::cout << "\n### [ O ] Tests Funcionales de Huffman Aprobados ###\n";
    else {
        std::cout << "\n### [ X ] Error! Tests Funcionales de Huffman "
                     "REPROBADOS ###\n";
        return 1;
    }

    for (auto& name : filenames) {
        string encode_output_file = name + "_compressed";
        string file = name + ".txt";

        encode_test(file, encode_output_file);
    }

    return 0;
}
