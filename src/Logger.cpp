//
// Created by ccc on 5/14/19.
//

#include <Logger.h>

#include "Logger.h"

void Logger::logBadToken(Source &source) const
{
    std::cout << "Bad Token at line: " << source.getLineNumber()
              << ", char: " << source.getCharNumber()
              << std::endl;
}

void Logger::logParserFailure(unsigned int line) const
{
    std::cout << "Parser failure at line: " << line << std::endl;
}

void Logger::logBadJson(const std::string& str) const
{
    std::cout << "Json parser problem with: " << str<< std::endl;
}

