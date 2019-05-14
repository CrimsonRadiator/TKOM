#include <memory>
#include <cctype>
#include <Scanner.h>
#include <iostream>
#include <Token.h>


#include "Scanner.h"
#include "SymbolTable.h"




Token Scanner::getNextToken() {
    return extractToken();
}


char Scanner::skipWhite(){
    while (!(isblank(source.getCurr()) && !isblank(source.peekNextChar())))
        source.getNextChar();

    return source.getNextChar();
}

Token Scanner::extractToken() {
    if(firstChar){
        source.getNextChar();
        firstChar = false;
    }

    //escape char

    //check if EOF
    if (source.getCurr() == EOF)
        return Token(TokenType::EOFT, "");

    else if (outsideTemplate) {

        //open template
        if(source.getCurr() == '{' && source.peekNextChar() == '{') {
            source.getNextChar();
            source.getNextChar();
            outsideTemplate = false;
            return Token(TokenType::OPTEMPLATE, "{{");
        }



        std::string text(1, source.getCurr());

        //get all text
        while (source.peekNextChar() != EOF && !(source.getCurr() =='{' && source.peekNextChar()=='{')) {
            //escape char
            if(source.peekNextChar() == '\\') {
                source.getNextChar();
                if (source.peekNextChar() == EOF)
                    break;
            }
            else
                text += source.getNextChar();
        }

        if (source.getCurr() == EOF){
            return Token(TokenType::EOFT, "");
        }
        else if (source.peekNextChar() == EOF) {
            source.getNextChar();
            return Token(TokenType::TEXT, text);
        }
        else {
            text = text.substr(0, text.size()-1); //TODO: no-copy way
            return Token(TokenType::TEXT, text);
        }
    }
    else{

        //skip whitespaces
        if(isblank(source.getCurr()))
            skipWhite();

        //brace
        if(source.getCurr() == '('){
            source.getNextChar();
            return Token(TokenType::OPBRACKET, "(");
        }

        //brace
        if(source.getCurr() == ')') {
            source.getNextChar();
            return Token(TokenType::CLBRACKET, ")");
        }

        //end template
        if (source.getCurr() == '}' && source.peekNextChar() =='}'){
            outsideTemplate = true;
            source.getNextChar();
            if(source.getNextChar() == '\n') {
                source.getNextChar();
                return Token(TokenType::CLTEMPLATENEW, "}}");
            }
            return Token(TokenType::CLTEMPLATENONEW, "}}");
        }

        //id or keyword
        if(isalpha(source.getCurr())){
            std::string text(1,source.getCurr());
            while(source.getCurr()!= EOF && isalnum(source.getCurr())) {
                text += source.getNextChar();
            }

            //skip last char
            text = text.substr(0, text.size()-1); //TODO: no-copy way


            //check if keyword
            if(keywords.find(text)!=keywords.end())
                return Token(keywords[text], text);

            //check if EOF
            if(source.getCurr() == EOF){
                return Token(TokenType::EOFT, "");
            }

            if (source.getCurr() =='.' || source.getCurr() == '['){
                text+=source.getCurr();
                return Token(TokenType::ID, idRegex(text));
            }

            return Token(TokenType::ID, text);
        }

        //check if digit
        if(isdigit(source.getCurr())){
            std::string text(1, source.getCurr());
            while(source.getCurr()!= EOF && isdigit(source.getCurr())){
                text+=source.getNextChar();
            }
            if(source.getCurr()==EOF)
                return Token(TokenType::EOFT, "");

            text = text.substr(0, text.size()-1); //TODO: no-copy way
            return Token(TokenType::NUMBER, text);
        }

        std::string tmpString = {source.getCurr(), source.peekNextChar()};

        //get two letter operators
        if(keywords.find(tmpString)!=keywords.end()) {
            source.getNextChar();
            source.getNextChar();
            return Token(keywords[tmpString], tmpString);
        }

        tmpString= tmpString.substr(0, tmpString.size()-1);

        //get one letter operators
        if(keywords.find(tmpString)!=keywords.end()) {
            source.getNextChar();
            return Token(keywords[tmpString], tmpString);
        }


        Logger::getInstance().logBadToken(source);

        //failure
        while(source.getNextChar()!= EOF)
        {}

        return Token(TokenType::EOFT, "");
    }

}

std::string Scanner::idRegex(std::string& text){
    if(isalpha(source.getCurr())) {
        text+=source.getCurr();
        while (isalnum(source.getCurr())) {
            text += source.getNextChar();
        }
    }

    if(source.getCurr() == '.'){
        source.getNextChar();
        return idRegex(text);
    }
    else if(source.getCurr() == '['){
        do{
            text+=source.getNextChar();
        }while(isdigit(source.getCurr()));

        if(source.getCurr()=='.' || source.getCurr()==']'){
            source.getNextChar();
            text+=source.getCurr();
            return idRegex(text);
        }
        else return idRegex(text);
    }
    else{
        text = text.substr(0, text.length()-1);
        return text;
    }
}

std::string Scanner::getTypeName(Token& t) const{
    return typeNames.at(t.getType());
}

Scanner::Scanner(Source& source_) : firstChar{true},
                                    source{source_},
                                    outsideTemplate{true} {

    keywords.emplace(std::make_pair("=", TokenType::ASSIGNOP));
    keywords.emplace(std::make_pair("not", TokenType::LOGICOP));
    keywords.emplace(std::make_pair("and", TokenType::LOGICOP));
    keywords.emplace(std::make_pair("or", TokenType::LOGICOP));
    keywords.emplace(std::make_pair("==", TokenType::COMPOP));
    keywords.emplace(std::make_pair("<", TokenType::COMPOP));
    keywords.emplace(std::make_pair("<=", TokenType::COMPOP));
    keywords.emplace(std::make_pair(">=", TokenType::COMPOP));
    keywords.emplace(std::make_pair(">", TokenType::COMPOP));
    keywords.emplace(std::make_pair("+", TokenType::MATHOP));
    keywords.emplace(std::make_pair("-", TokenType::MATHOP));
    keywords.emplace(std::make_pair("/", TokenType::MATHOP));
    keywords.emplace(std::make_pair("*", TokenType::MATHOP));
    keywords.emplace(std::make_pair("%", TokenType::MATHOP));
    keywords.emplace(std::make_pair("for", TokenType::FOR));
    keywords.emplace(std::make_pair("endfor", TokenType::ENDFOR));
    keywords.emplace(std::make_pair("while", TokenType::WHILE));
    keywords.emplace(std::make_pair("endwhile", TokenType::ENDWHILE));
    keywords.emplace(std::make_pair("else", TokenType::ELSE));
    keywords.emplace(std::make_pair("endelse", TokenType::ENDELSE));
    keywords.emplace(std::make_pair("if", TokenType::IF));
    keywords.emplace(std::make_pair("endif", TokenType::ENDIF));
    keywords.emplace(std::make_pair("in", TokenType::IN));
    keywords.emplace(std::make_pair("int", TokenType::TYPE));
    keywords.emplace(std::make_pair("bool", TokenType::TYPE));
    keywords.emplace(std::make_pair("string", TokenType::TYPE));
    keywords.emplace(std::make_pair("true", TokenType::BOOLVAL));
    keywords.emplace(std::make_pair("false", TokenType::BOOLVAL));

    typeNames.emplace(std::make_pair(TokenType::EOFT, "EOFT"));
    typeNames.emplace(std::make_pair(TokenType::NUMBER, "NUMBER"));
    typeNames.emplace(std::make_pair(TokenType::MATHOP, "MATHOP"));
    typeNames.emplace(std::make_pair(TokenType::LOGICOP, "LOGICOP"));
    typeNames.emplace(std::make_pair(TokenType::ENDFOR, "ENDFOR"));
    typeNames.emplace(std::make_pair(TokenType::ENDWHILE, "ENDWHILE"));
    typeNames.emplace(std::make_pair(TokenType::ENDELSE, "ENDELSE"));
    typeNames.emplace(std::make_pair(TokenType::ASSIGNOP, "ASSIGNOP"));
    typeNames.emplace(std::make_pair(TokenType::BOOLVAL, "BOOLVAL"));
    typeNames.emplace(std::make_pair(TokenType::OPTEMPLATE, "OPTEMPLATE"));
    typeNames.emplace(std::make_pair(TokenType::CLBRACKET, "CLBRACKET"));
    typeNames.emplace(std::make_pair(TokenType::CLTEMPLATENEW, "CLTEMPLATENEW"));
    typeNames.emplace(std::make_pair(TokenType::CLTEMPLATENONEW, "CLTEMPLATENONEW"));
    typeNames.emplace(std::make_pair(TokenType::COMPOP, "COMPOP"));
    typeNames.emplace(std::make_pair(TokenType::ELSE, "ELSE"));
    typeNames.emplace(std::make_pair(TokenType::ENDIF, "ENDIF"));
    typeNames.emplace(std::make_pair(TokenType::FOR, "FOR"));
    typeNames.emplace(std::make_pair(TokenType::IF, "IF"));
    typeNames.emplace(std::make_pair(TokenType::WHILE, "WHILE"));
    typeNames.emplace(std::make_pair(TokenType::ID, "ID"));
    typeNames.emplace(std::make_pair(TokenType::IN, "IN"));
    typeNames.emplace(std::make_pair(TokenType::OPBRACKET, "OPBRACKET"));
    typeNames.emplace(std::make_pair(TokenType::TEXT, "TEXT"));
    typeNames.emplace(std::make_pair(TokenType::TYPE, "TYPE"));
}

Scanner::~Scanner()  = default;

