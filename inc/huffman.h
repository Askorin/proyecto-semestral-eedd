#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

class Node {
public:
    char symbol;
    size_t freq;
    Node* left;
    Node* right;
    Node* parent;
    Node(char, size_t, Node*, Node*, Node*);
    
};

std::ostream& operator<<(std::ostream&, const Node&);

class Compare {
public:
    bool operator()(Node&, Node&);
};

std::tuple<std::string, Node> encode(std::string&);

std::string decode(std::string&, Node&);

std::unordered_map<char, size_t> calculate_frequencies(std::string&);

void generate_huffman_tree(std::unordered_map<char, size_t>&);

#endif
