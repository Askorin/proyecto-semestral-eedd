#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <tuple>
#include <string>

class Node {
    char symbol;
    size_t freq;
    Node* left;
    Node* right;
    Node* parent;
};

std::tuple<std::string, Node> codificar(std::string&);

std::string decodificar(std::string&, Node&);

#endif
