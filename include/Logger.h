#ifndef TKOM_LOGGER_H
#define TKOM_LOGGER_H

#include <iostream>
#include "Token.h"
#include "Source.h"


class Logger
{
public:
    static Logger &getInstance()
    {
        static Logger instance;
        return instance;
    }

private:
    Logger()
    {}

public:
    Logger(Logger const &) = delete;

    void operator=(Logger const &) = delete;

    void logBadToken(Source &source) const;

    void logParserFailure(unsigned int) const;

    void logUndefinedSymbol(const Token &t) const;
};


#endif //TKOM_LOGGER_H
