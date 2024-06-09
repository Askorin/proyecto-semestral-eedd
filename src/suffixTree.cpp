#include "../inc/suffixTree.h"
#include <iostream>
#include <string>

childNode::childNode() {
    this->pos = -1;
}

childNode::childNode(int pos, std::string string) : pos(pos), string(string) {}

void suffixTree::insert(std::string text) {
    for (int i = 0; i < text.length(); i++) {
        if (this->first_child == NULL) {
            this->first_child = new childNode(0, text);
            continue;
        }
        childNode* actual_node = this->first_child;
        childNode* parent_node = NULL;
        std::string substring;
        bool inserted = 0;

        do {
            std::string local_substring;
            while (actual_node->string[local_substring.length()] == text[i + substring.length() + local_substring.length()]) {
                local_substring.push_back(actual_node->string[local_substring.length()]);
                
                if (local_substring == actual_node->string) {
                    parent_node = actual_node;
                    actual_node = actual_node->child;
                    substring.append(local_substring);
                    continue;
                }

                if ((i + substring.length() + local_substring.length()) >= text.length()) break;
            }
            
            if (!local_substring.empty()) {
                std::string prev_string = actual_node->string;
                childNode* prev_child = actual_node->child;
                actual_node->string = local_substring;
                actual_node->child = new childNode(actual_node->pos, prev_string.substr(local_substring.length()));
                actual_node->pos = i;
                if (i + substring.length() + local_substring.length() < text.length()) {
                    actual_node->child->next_sibling = new childNode(i, text.substr(i + substring.length() + local_substring.length()));
                }
                actual_node->child->child = prev_child;
                inserted = 1;
                break;
            }

            actual_node = actual_node->next_sibling;
        } while (actual_node != NULL);

        if (inserted) continue;

        if (parent_node != NULL) {
            childNode* prev_child = parent_node->child;
            parent_node->child = new childNode(i, text.substr(i + substring.length()));
            parent_node->child->next_sibling = prev_child;
        } else {
            childNode* prev_child = this->first_child;
            this->first_child = new childNode(i, text.substr(i));
            this->first_child->next_sibling = prev_child;
        }
    } 
}

void suffixTree::print_node_recursive(childNode* node, std::string prev) {
    std::string new_str = prev.append(node->string);
    std::cout << new_str << ", pos: " << node->pos << "\n";
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

suffixTree::suffixTree(std::string text) {
    std::cout << text << "\n";
    this->insert(text);
}

int main() {
    suffixTree tree(std::string("ABACA")); 

    //tree.print_stored_substrings();
}
