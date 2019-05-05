#ifndef TKOM_PARSER_H
#define TKOM_PARSER_H

#include "Scanner.h"
#include "Token.h"
#include "Node.h"

#include <memory>

class Parser {
public:
    explicit Parser(const Scanner& scanner_) :
                            scanner{scanner_},
                            currentToken{TokenType::START, ""}
    {}

    void nextSymbol();
    bool accept(TokenType type);
    bool accept(TokenType type, const std::string& val);

    bool segment();
    bool root();
    bool statement();
    bool forExpr();
    bool whileExpr();
    bool ifExpr();
    bool compoundLogicExpr();
    bool compoundExpr();
    bool expr();
    bool declaration();
    bool endTemplate();
    bool idOrNumOrBool();
    bool value();

    Token getCurrentToken() const{
        return currentToken;
    }



private:
    Scanner scanner;
    Token currentToken;
};


#endif //TKOM_PARSER_H
