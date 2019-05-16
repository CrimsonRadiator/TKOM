
#ifndef TKOM_TREE_H
#define TKOM_TREE_H

#include <memory>
#include <vector>

#include "Token.h"

class Node
{
public:

    Node(const Token &token_) :  newline{false}, token{token_}
    {
    }


    bool newline;
    Token token;
    std::vector<std::unique_ptr<Node>> children;

    void add(std::unique_ptr<Node> &node)
    {
        children.emplace_back(std::move(node));
    }

};


#endif //TKOM_TREE_H
