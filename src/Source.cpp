//
// Created by ccc on 4/4/19.
//

#include "Source.h"

char Source::getNextChar() {

    if(!sourceStream.get(currentChar))
        currentChar=EOF;

    if (currentChar == '\n') {
        ++lineNumber;
        charNumber = 0;
    } else {
        ++charNumber;
    }
    return currentChar;
}
