
#ifndef TKOM_TOKEN_H
#define TKOM_TOKEN_H

#include <string>
#include <memory>
#include "Source.h"

enum class TokenType{
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
        EOFT
    };

class Token {
public:
    Token(TokenType t, std::string text_) : type{t}, text{text_}
    {};

    TokenType getType() const{
        return type;
    }
    std::string getText() const{
        return text;
    }
private:
    TokenType type;
    std::string text;

};




#endif //TKOM_TOKEN_H
