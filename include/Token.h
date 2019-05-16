
#ifndef TKOM_TOKEN_H
#define TKOM_TOKEN_H

#include <string>
#include <memory>
#include "Source.h"


enum class TokenType
{
    OPTEMPLATE,
    CLTEMPLATENEW,
    CLTEMPLATENONEW,
    OPBRACKET,
    CLBRACKET,
    ID,
    MATHOP,
    LOGICOP,
    COMPOP,
    ASSIGNOP,
    FOR,
    ENDFOR,
    WHILE,
    ENDWHILE,
    IF,
    ENDIF,
    ELSE,
    ENDELSE,
    IN,
    BOOLVAL,
    NUMBER,
    TEXT,
    TYPE,
    EOFT,
    START
};

class Token
{
public:
    //TODO: another constructor for constant text
    Token(TokenType t, std::string text_) : type{t}, text{text_} //TODO: don't copy string
    {};

    TokenType getType() const
    {
        return type;
    }

    std::string getText() const
    {
        return text;
    }

private:
    TokenType type;
    std::string text;
};


#endif //TKOM_TOKEN_H
