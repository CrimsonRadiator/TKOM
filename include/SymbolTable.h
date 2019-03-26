//
// Created by ccc on 3/26/19.
//

#ifndef TKOM_SYMBOL_TABLE_H
#define TKOM_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

enum class TokenType{
    Text,
    AlphaNumString,
    Number,
    OpenCurlyBrace,
    EscapedOpenCurlyBrace,
    CloseCurlyBrace,
    EscapedCloseCurlyBrace,
    OpenBrace,
    CloseBrace,
    Type,
    MathOperator,
    LogicOperator
};

class Token{
    TokenType type;
    union value{
        std::string stringValue;
        bool boolValue;
        char charValue;
        int integerValue;
    };
};



class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();


    bool tokenExists(std::string const &tokenName){
        return table.find(tokenName) == table.end();
    }

    Token getToken(std::string const &tokenName) {
        return table.at(tokenName);
    }

    void addToken(Token const & token, std::string const &tokenName){
        table[tokenName] = token;
    }

private:

    std::unordered_map<std::string, Token> table;

};


#endif //TKOM_SYMBOL_TABLE_H

