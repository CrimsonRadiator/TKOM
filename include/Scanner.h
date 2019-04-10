#ifndef TKOM_SCANNER_H
#define TKOM_SCANNER_H

#include "Source.h"
#include "Token.h"
#include <sstream>
#include <memory>
#include <unordered_map>


class Scanner {
public:
    Scanner(Source& source);
    ~Scanner();

    Token getNextToken();
    std::string getTypeName(Token& t) const;

private:
    bool firstChar;
    Source& source;
    char skipWhite();
    std::string idRegex(std::string& text);
    Token extractToken();
    bool outsideTemplate;

    std::unordered_map<std::string, TokenType> keywords;
    std::unordered_map<TokenType, std::string> typeNames;


};


#endif //TKOM_SCANNER_H
