#ifndef HUFFMAN_H
#define HUFFMAN_H
#include "../inc/BitField.h"
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

class Node {
  public:
    char symbol;
    size_t freq;
    Node* left;
    Node* right;
    Node* parent;
    /* Constructor normal de nodos */
    Node(char, size_t, Node*, Node*, Node*);
    /* Constructor de combinación de nodos */
    Node(Node*, Node*);

    bool is_leaf();
};

struct Code {
    std::bitset<255> code;
    unsigned char length;
    Code(std::bitset<255>, unsigned char);
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
    bool operator()(std::pair<char, unsigned char>,
                    std::pair<char, unsigned char>);
};

void encode_file(std::string&, std::string&);

void save_header(std::ofstream&, std::unordered_map<char, Code>&,
                 std::unordered_map<unsigned char, size_t>&, std::vector<char>&,
                 size_t);

void save_code(std::ofstream&, std::ifstream& fin,
               std::unordered_map<char, Code>&,
               std::unordered_map<unsigned char, size_t>&, std::vector<char>&,
               size_t);

std::unordered_map<char, Code>
get_canonical_codes(std::unordered_map<char, unsigned char>&,
                    std::vector<char>&);

std::string decode(std::string&, Node&);

void decode_file(std::string&, std::string&);

std::unordered_map<char, size_t> calculate_frequencies(std::string&);

std::unordered_map<char, size_t> calculate_frequencies_from_file(std::string&,
                                                                 size_t&);

Node* generate_huffman_tree(std::unordered_map<char, size_t>&);

Node* generate_huffman_tree(std::unordered_map<char, Code>&);

void traverse_huffman_tree(Node*, std::unordered_map<char, unsigned char>&,
                           size_t&, std::unordered_map<unsigned char, size_t>&,
                           size_t&);

void _generate_huffman_tree(char, Code&, Node*, size_t, size_t&);

/* Boost no tiene una operación de adición, así que aquí está. */
void increment(std::bitset<255>&);

#endif
