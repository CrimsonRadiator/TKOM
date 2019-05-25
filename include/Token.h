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
    START,
    MULTIPLE_SEGMENTS,
    TEMPLATE_TEXT
};

enum class TokenValueType{
    INTEGER,
    BOOLEAN,
    STRING
};

struct TokenValue{
    //TODO: assign constructor

    TokenValue() = default;

    TokenValue(const TokenValue& value_) : valueType{value_.valueType}
    {
        switch(value_.valueType)
        {
            case TokenValueType::STRING:
                str = value_.str; break;
            case TokenValueType::INTEGER:
                integer = value_.integer; break;
            case TokenValueType::BOOLEAN:
                boolean = value_.boolean; break;
        }
    }

    TokenValue& operator = (const TokenValue &tv){
        if(this!=&tv){
            //TODO: maybe checks?
            valueType = tv.valueType;
            str = tv.str;
            integer = tv.integer;
            boolean = tv.boolean;
        }
        return *this;
    }

    TokenValue(const std::string& str_) : valueType{TokenValueType::STRING}, str{str_}
        {}
    TokenValue(int integer_) : valueType{TokenValueType::INTEGER}, integer{integer_}
        {}
    TokenValue(bool boolean_) : valueType{TokenValueType::BOOLEAN}, boolean{boolean_}
        {}
    TokenValue(const char *char_) : valueType{TokenValueType::STRING}, str{char_}
        {}

    bool operator==(const TokenValue& tv) const{
        if(tv.valueType != valueType)
            return false;
        switch(valueType){
            case TokenValueType::BOOLEAN:
                return boolean == tv.boolean;
            case TokenValueType::INTEGER:
                return integer == tv.integer;
            case TokenValueType::STRING:
                return str == tv.str;
        }
    }

    TokenValueType valueType;

    std::string str;
    int integer;
    bool boolean;

};

class Token
{
public:
    Token(TokenType t, const TokenValue& value_) : type{t}, value{value_}
    {};

    Token(TokenType t, const std::string& str_) :type{t}, value{str_}
    {};

    Token(TokenType t,  int integer_) : type{t}, value{integer_}
    {};

    Token(TokenType t, bool boolean_) : type{t}, value{boolean_}
    {};

    Token(TokenType t, const char* chars_) :type{t}, value{std::string(chars_)}
    {};

    TokenType getType() const
    {
        return type;
    }

    std::string getText() const
    {
        if(value.valueType == TokenValueType::STRING)
            return value.str;

        if(value.valueType == TokenValueType::STRING)
            return std::to_string(value.integer);

        if(value.valueType == TokenValueType::BOOLEAN)
            return value.boolean ? "true" : "false";
    }

    TokenValue getValue() const{
        return value;
    }

private:
    //TODO: Mutable?
    TokenType type;
    TokenValue value;
};


#endif //TKOM_TOKEN_H
