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
    MULTIPLE_SEGMENTS
};

enum class ValueType{
    INTEGER,
    BOOLEAN,
    STRING
};

struct Value{
    Value(const Value& value_) : valueType{value_.valueType}
    {
        switch(value_.valueType)
        {
            case ValueType::STRING:
                str = value_.str; break;
            case ValueType::INTEGER:
                integer = value_.integer; break;
            case ValueType::BOOLEAN:
                boolean = value_.boolean; break;
        }
    }

    explicit Value(const std::string& str_) : valueType{ValueType::STRING}, str{str_}
        {}
    explicit Value(int integer_) : valueType{ValueType::INTEGER}, integer{integer_}
        {}
    explicit Value(bool boolean_) : valueType{ValueType::BOOLEAN}, boolean{boolean_}
        {}

    ValueType valueType;

    std::string str;
    int integer;
    bool boolean;

};

class Token
{
public:
    //TODO: another constructor for constant text
    Token(TokenType t, const Value& value_) : type{t}, value{value_} //TODO: don't copy string
    {};

    Token(TokenType t, const std::string& str_) :type{t}, value{str_}
    {};

    TokenType getType() const
    {
        return type;
    }

    std::string getText() const
    {
        if(value.valueType == ValueType::STRING)
            return value.str;

        if(value.valueType == ValueType::STRING)
            return std::to_string(value.integer);

        if(value.valueType == ValueType::BOOLEAN)
            return value.boolean ? "true" : "false";
    }

private:
    TokenType type;
    Value value;
};


#endif //TKOM_TOKEN_H
