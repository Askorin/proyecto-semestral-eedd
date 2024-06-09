#include "../inc/suffixTree.h"
#include <iostream>
#include <string>

childNode::childNode() {
    this->pos = -1;
}

childNode::childNode(int suffix_pos, std::string suffix_string) : pos(suffix_pos), string(suffix_string) {}

void suffixTree::insert(std::string text, bool verbose = 0) {
    for (int i = 0; i < text.length(); i++) {
        if (verbose) std::cout << "----------------------------------------\nInsertando palabra " << text.substr(i) << "\n"; 
        if (this->first_child == NULL) {
            this->first_child = new childNode(0, text);
            if (verbose) std::cout << "No existe hijo en root, ahora la palabla es el first child\n";
            continue;
        }
        childNode* actual_node = this->first_child;
        childNode* parent_node = NULL;
        std::string substring;
        bool inserted = 0;
        bool already_present = 0;
        do {
            if (verbose) std::cout << "Nodo actual: " << actual_node->string << "\n"; 
            std::string local_substring;
            while (actual_node->string[local_substring.length()] == text[i + substring.length() + local_substring.length()]) {
                local_substring.push_back(actual_node->string[local_substring.length()]);
                if (verbose) std::cout << "Se encontró similitud: " << local_substring << "\n";

                if ((i + substring.length() + local_substring.length()) >= text.length()) {
                    if (verbose) std::cout << "Se llegó al final de text y es semejante en todos sus caracteres; ya se encuentra guardado\n";
                    already_present = 1;
                    break;
                }

                if (local_substring == actual_node->string) {
                    if (verbose) std::cout << "Inicio de text es igual a todo el nodo, buscando semejanzas en hijo\n";
                    parent_node = actual_node;
                    actual_node = actual_node->child;
                    substring.append(local_substring);
                    continue;
                }
            }
            
            if (already_present) break;

            
            if (!local_substring.empty()) {
                if (verbose) std::cout << "Difieren en el siguiente caracter. Semejanza: " << local_substring << "\n";
                int textpos = i + substring.length() + local_substring.length();

                if (verbose) std::cout << "Dividiendo nodo " << actual_node->string << " en nodos " << 
                    actual_node->string.substr(local_substring.length()) << " y " << text.substr(textpos) << " hijos de " << local_substring <<"\n";

                std::string prev_string = actual_node->string;
                childNode* prev_child = actual_node->child;
                actual_node->string = local_substring;
                actual_node->child = new childNode(actual_node->pos, prev_string.substr(local_substring.length()));
                actual_node->child->next_sibling = new childNode(i, text.substr(textpos));
                actual_node->child->child = prev_child;
                inserted = 1;
                break;
            }

            if (verbose) std::cout << "No tienen ninguna semejanza, pasando al siguiente nodo\n";

            actual_node = actual_node->next_sibling;
        } while (actual_node != NULL);

        if (inserted || already_present) continue;

        if (parent_node != NULL) {
            if (verbose) std::cout << "Creando hijo en nodo " << parent_node->string << " al no encontrar semejanzas en ningún sibling\n";
            childNode* prev_child = parent_node->child;
            parent_node->child = new childNode(i, text.substr(i + substring.length()));
            parent_node->child->next_sibling = prev_child;
        } else {
            if (verbose) std::cout << "Creando nodo en root al no encontrar semejanzas en ningún sibling\n";
            childNode* prev_child = this->first_child;
            this->first_child = new childNode(i, text.substr(i));
            this->first_child->next_sibling = prev_child;
        }
    } 
}

void suffixTree::print_node_recursive(childNode* node, std::string prev) {
    std::string new_str = std::string(prev).append(node->string);
    std::cout << "Nodo: " << node->string << ", Sufijo: " << new_str << ", pos: " << node->pos << "\n";
    if (node->child != NULL) {
        print_node_recursive(node->child, new_str);
    }
    if (node->next_sibling != NULL) {
        print_node_recursive(node->next_sibling, prev);
    }
}

void suffixTree::print_stored_substrings() {
    print_node_recursive(this->first_child, std::string());
}

suffixTree::suffixTree(std::string text, bool verbose = 0) {
    this->first_child = NULL;
    this->insert(text, verbose);
}

int main() {
    suffixTree tree(std::string("HOLA_ANTONIO")); 

    tree.print_stored_substrings();
}
