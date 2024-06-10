#include "../inc/huffman.h"
#include <iostream>
#include <ostream>
#include <queue>
#include <unordered_map>

bool Compare::operator()(Node* node_l, Node* node_r) {
    return node_l->freq > node_r->freq;
}

Node::Node(char symbol, size_t freq, Node* left, Node* right, Node* parent)
    : symbol(symbol), freq(freq), left(left), right(right), parent(parent) {}

Node::Node(Node* node_l, Node* node_r)
    : symbol('\0'), freq(node_l->freq + node_r->freq), left(node_l),
      right(node_r), parent(nullptr) {

    node_l->parent = this;
    node_r->parent = this;
}

bool Node::is_leaf() { return !(left || right); }

std::ostream& operator<<(std::ostream& os, const Node& node) {
    return (os << node.symbol << " : " << node.freq);
}

std::tuple<std::string, Node*> encode(std::string& message) {
    auto frecuencias = calculate_frequencies(message);
    Node* root = generate_huffman_tree(frecuencias);

    /* Ahora que tenemos el arbol, lo recorremos */
    std::unordered_map<char, std::string> code_map;
    traverse_huffman_tree(root, "", code_map);

    /* Sanity check */
    //for (auto entry : code_map) {
    //    std::cout << entry.first << ": " << entry.second << std::endl;
    //}

    std::string coded_message("");
    /* Codificamos */
    for (char c : message) {
        coded_message += code_map[c];
    }

    return std::tuple(coded_message, root);
}

/* Para mensajes muy grandes puede que sea necesario no usar recursividad */
void traverse_huffman_tree(Node* node, std::string code,
                           std::unordered_map<char, std::string>& code_map) {

    /* Estamos en un nodo combinado. */
    if (node->is_leaf()) {
        code_map.insert({node->symbol, code});
    }
    /* Tenemos hijo izquierdo, lo visitamos */
    if (node->left) {
        traverse_huffman_tree(node->left, code + "0", code_map);
    }
    /* Tenemos hijo derecho, lo visitamos */
    if (node->right) {
        traverse_huffman_tree(node->right, code + "1", code_map);
    }
}

std::string decode(std::string& coded_message, Node& tree) {

    /* El mensaje es la repeticion de un mismo caracter */
    if (tree.is_leaf()) {
        return std::string(coded_message.size(), tree.symbol);
    }

    std::string decoded_message("");

    /* Decodificamos */
    Node* v = &tree;
    for (char c : coded_message) {
        /* Si es 0, vamos al hijo izquierdo, de lo contrario (es 1), vamos a la derecha */
        if (c == '0') {
            v = v->left;
        } else {
            v = v->right;
        }
        /* 
         * Si el nodo es una hoja, hemos encontrado el simbolo correspondiente, la agregamos al mensaje decodificado
         * y volvemos a la raíz.
         */
        if (v->is_leaf()) {
            decoded_message += v->symbol;
            v = &tree;
        }
    }
    return decoded_message;
}

std::unordered_map<char, size_t> calculate_frequencies(std::string& message) {
    std::unordered_map<char, size_t> frequencies;
    for (char c : message) {
        frequencies[c]++;
    }
    return frequencies;
}

/* TODO: Manejar edge case de un carácter */
Node* generate_huffman_tree(std::unordered_map<char, size_t>& frequencies) {

    std::priority_queue<Node*, std::vector<Node*>, Compare> prio_queue;
    /*
     * El orden en el que el iterador itere sobre las entries del mapa influye
     * en la naturaleza de la codificación, pero sigue siendo la misma en
     * verdad, no asustarse.
     */
    for (auto entry : frequencies) {
        prio_queue.push(
            new Node(entry.first, entry.second, nullptr, nullptr, nullptr));
    }

    while (prio_queue.size() > 1) {
        /* Extraemos los dos nodos de menor peso */
        Node* node_l = prio_queue.top();
        prio_queue.pop();
        Node* node_r = prio_queue.top();
        prio_queue.pop();

        /* Los combinamos */
        Node* node_combined = new Node(node_l, node_r);

        /* Lo reingresamos a la priority queue */
        prio_queue.push(node_combined);
    }

    Node* root = prio_queue.top();
    return root;
}