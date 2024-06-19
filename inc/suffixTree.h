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
    childNode(int, std::string);
};

class suffixTree {
private:
    childNode* first_child;
    int length_text;
public:
    void delete_node_recursive(childNode*);
    void insert(std::string, bool);
    std::pair<int, int> search_substring(std::string);
    suffixTree(std::string, bool);
    ~suffixTree();
};

#endif
