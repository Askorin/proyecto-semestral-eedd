#include "../inc/BitField.h"
#include "../inc/huffman.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <iostream>

using namespace std;
int main() {
    //string encode_file_name_input = "../data/sample.txt";
    //string encode_file_name_output = "../data/sample_compressed";
    //string decode_file_name_output = "../data/sample_decompressed.txt";

    string encode_file_name_input = "../data/sample.txt";
    string encode_file_name_output = "../data/compressed";
    string decode_file_name_output = "../data/decompressed.txt";


    encode_file(encode_file_name_input, encode_file_name_output);
    decode_file(encode_file_name_output, decode_file_name_output);
    return 0;
}
