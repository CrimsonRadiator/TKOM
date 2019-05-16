#ifndef TKOM_PARSER_H
#define TKOM_PARSER_H

#include "Scanner.h"
#include "Token.h"
#include "Node.h"

#include <memory>

class Parser
{
public:
    explicit Parser(const Scanner &scanner_) :
            scanner{scanner_},
            currentToken{TokenType::START, ""}
    {}

    void nextSymbol();

    std::unique_ptr<Node> accept(TokenType type);

    std::unique_ptr<Node> accept(TokenType type, const std::string &val);

    std::unique_ptr<Node> segment();

    std::unique_ptr<Node> root();

    std::unique_ptr<Node> multipleSegments();

    std::unique_ptr<Node> forExpr();

    std::unique_ptr<Node> whileExpr();

    std::unique_ptr<Node> ifExpr();

    std::unique_ptr<Node> compoundExpr();

    std::unique_ptr<Node> expr();

    std::unique_ptr<Node> declaration();

    bool endTemplate(std::unique_ptr<Node> &t);

    std::unique_ptr<Node> keyword(TokenType t);

    std::unique_ptr<Node> keywordSkip(TokenType t);

    std::unique_ptr<Node> value();

    std::unique_ptr<Node> failure();

    Token getCurrentToken() const
    {
        return currentToken;
    }


private:
    Scanner scanner;
    Token currentToken;
};


#endif //TKOM_PARSER_H
