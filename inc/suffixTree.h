#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <string>
#include <utility>

class childNode {
public:
    childNode* next_sibling;
    childNode* child;
    int pos;
    std::string string;
    childNode();
    childNode(int, std::string);
};

class suffixTree {
private:
    childNode* first_child;
public:
    void insert(std::string, bool);
    std::pair<int, int> search_substring(std::string);
    void print_node_recursive(childNode*, std::string);
    void print_stored_substrings();
    suffixTree(std::string, bool);
};

#endif
