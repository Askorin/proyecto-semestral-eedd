#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <string>
#include <utility>

class childNode {
public:
    childNode* next_sibling;
    childNode* child;
    int pos;
    char* string;
    childNode();
    childNode(int, char*);
};

class suffixTree {
private:
    childNode* first_child;
public:
    void insert(char*);
    std::pair<int, int> search_substring(char*);
    suffixTree();
    suffixTree(char*);
};

#endif
