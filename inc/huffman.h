#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>
#include <tuple>
#include <unordered_map>
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

std::ostream& operator<<(std::ostream&, const Node&);

class Compare {
  public:
    bool operator()(Node*, Node*);
};

std::tuple<std::string, Node*> encode(std::string&);

std::string decode(std::string&, Node&);

std::unordered_map<char, size_t> calculate_frequencies(std::string&);

Node* generate_huffman_tree(std::unordered_map<char, size_t>&);

void traverse_huffman_tree(Node*, std::string,
                           std::unordered_map<char, std::string>&);

#endif
