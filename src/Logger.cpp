//
// Created by ccc on 5/14/19.
//

#include <Logger.h>

#include "Logger.h"

void Logger::logBadToken(Source& source) const {
    std::cout << "Bad Token at line: " << source.getLineNumber()
                << ", char: " << source.getCharNumber()
                << std::endl;
}

void Logger::logParserFailure(unsigned int line) const {
    std::cout << "Parser failure at line: " << line << std::endl;
}

void Logger::logUndefinedSymbol(const Token &t) const {
    std::cout << "Undefined symbol " << t.getText() << std::endl;
}