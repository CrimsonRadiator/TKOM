#ifndef TKOM_SOURCE_H
#define TKOM_SOURCE_H

#include <fstream>
#include <sstream>
#include <string>

class Source{
public:
    explicit Source(const std::string& filename) :
                            sourceStream(filename),
                            currentChar{0},
                            lineNumber{0},
                            charNumber{0},
                            position{0}

    {}


    explicit Source() : currentChar{0},
                        lineNumber{0},
                        charNumber{0},
                        position{0} {}

    /*! \brief extracts new char
     *
     *  sometimes increments lineNumber
     *  always changes charNumber
     *
     * @return next char from input file.
     */
    virtual char getNextChar();

    /*!
     * \breif peeks next char, without moving ahead
     *
     * @return next char from input file
     */
    virtual char peekNextChar(){
        return sourceStream.peek();
    }

    virtual char getCurr(){
        return currentChar;
    }

    unsigned getLineNumber() const{
        return lineNumber;
    }

    unsigned getCharNumber() const{
        return charNumber;
    }

protected:
    std::ifstream sourceStream;
    char currentChar;
    unsigned lineNumber;
    unsigned charNumber;
    unsigned position;

};


#endif //TKOM_SOURCE_H
