#ifndef HUFFMAN_H
#define HUFFMAN_H
#include "../inc/BitField.h"
#include <bitset>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#define CHAR_NUM 256
#define MAX_CODE_SIZE 255

/*
 * Tamaño máximo permitido para codificar: 2^32 - 1 (se ocupan 4 bytes para
 * guardar longitud del mensaje
 */

class Node {
  public:
    unsigned char symbol;
    size_t freq;
    Node* left;
    Node* right;
    Node* parent;
    /* Constructor normal de nodos */
    Node(unsigned char, size_t, Node*, Node*, Node*);
    /* Constructor de combinación de nodos */
    Node(Node*, Node*);

    bool is_leaf();
};

struct Code {
    std::bitset<MAX_CODE_SIZE> code;
    unsigned char length;
    Code(std::bitset<MAX_CODE_SIZE>, unsigned char);
    Code();
    Code get_reversed();
};

std::ostream& operator<<(std::ostream&, const Node&);

/*
 * Para comparar nodos de huffman a la hora de insertaros a la prio_queue en la
 * generación del arbol
 */
class CompareHuffmanNodes {
  public:
    bool operator()(Node*, Node*);
};

/*
 * Para comparar a la hora de generar los códigos canónicos de Huffman,
 * compara un símbolo y la longitud del codigo asignado por Huffman
 * convencional.
 */
class CompareCodes {
  public:
    bool operator()(std::pair<unsigned char, unsigned char>,
                    std::pair<unsigned char, unsigned char>);
};

void encode_file(std::string&, std::string&);

void save_header(std::ofstream&, std::array<Code, CHAR_NUM>&,
                 std::array<unsigned char, CHAR_NUM>&,
                 std::vector<unsigned char>&, size_t, int, size_t);

void save_code(std::ofstream&, std::ifstream& fin, std::array<Code, CHAR_NUM>&);

std::array<Code, CHAR_NUM>
get_canonical_codes(std::array<unsigned char, CHAR_NUM>&,
                    std::vector<unsigned char>&);

std::string decode(std::string&, Node&);

void decode_file(std::string&, std::string&);

std::unordered_map<unsigned char, size_t> calculate_frequencies(std::string&);

std::unordered_map<unsigned char, size_t>
calculate_frequencies_from_file(std::string&, size_t&);

Node* generate_huffman_tree(std::unordered_map<unsigned char, size_t>&);

Node* generate_huffman_tree(std::array<Code, CHAR_NUM>&);

void traverse_huffman_tree(Node*, std::array<unsigned char, CHAR_NUM>&, size_t&,
                           std::array<unsigned char, CHAR_NUM>&);

void _generate_huffman_tree(unsigned char, Code&, Node*, size_t, size_t&);

void increment(std::bitset<MAX_CODE_SIZE>&);

#endif
