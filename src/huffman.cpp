#include "../inc/huffman.h"
#include <iostream>
#include <ostream>
#include <queue>
#include <unordered_map>

bool Compare::operator()(Node& node_1, Node& node_2) {
    return node_1.freq > node_2.freq;
}

Node::Node(char symbol, size_t freq, Node* left, Node* right, Node* parent)
    : symbol(symbol), freq(freq), left(left), right(right), parent(parent) {}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    return (os << node.symbol << " : " << node.freq);
}

std::tuple<std::string, Node> encode(std::string& message) {
    auto frecuencias = calculate_frequencies(message);
    generate_huffman_tree(frecuencias);
    return std::tuple("", Node('a', 0, nullptr, nullptr, nullptr));
}

std::string decode(std::string&, Node&);

std::unordered_map<char, size_t> calculate_frequencies(std::string& message) {
    std::unordered_map<char, size_t> frequencies;
    for (char c : message) {
        frequencies[c]++;
    }
    return frequencies;
}

void generate_huffman_tree(std::unordered_map<char, size_t>& frequencies) {

    std::priority_queue<Node, std::vector<Node>, Compare> prio_queue;
    for (auto entry : frequencies) {
        prio_queue.push(
            Node(entry.first, entry.second, nullptr, nullptr, nullptr));
    }

    while (!prio_queue.empty()) {
        std::cout << prio_queue.top() << std::endl;
        prio_queue.pop();
    }
}
