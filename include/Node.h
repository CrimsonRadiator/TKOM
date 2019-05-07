
#ifndef TKOM_TREE_H
#define TKOM_TREE_H

#include <memory>
#include <vector>

#include "Token.h"

enum class NodeType{
    START,
    WHILE_LOOP,
    FOR_LOOP,
    IF,
    IF_ELSE,
    ELSE,
    ERROR,
    TOKEN,
    DECLARATION,
    STATEMENT,
    COMPOUND_LOGIC_EXPR,
    COMPOUND_EXPR,
    LOGICOP
};


class Node {
public:

    Node(const Token& token_) : type{NodeType::TOKEN}, newline{false}, token{token_}
    {
    }


    NodeType type;
    bool newline;
    Token token;
    std::vector<std::unique_ptr<Node>> children;

    void add(std::unique_ptr<Node>& node) {
        children.emplace_back(std::move(node));
    }

};


#endif //TKOM_TREE_H
