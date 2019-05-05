#include "Parser.h"

typedef TokenType TT;

void Parser::nextSymbol(){
    currentToken = scanner.getNextToken();
}

bool Parser::accept(TT type) {
    if(currentToken.getType() == type){
        nextSymbol();
        return true;
    }else
        return false;
}

bool Parser::accept(TT type, const std::string& val){
    if (currentToken.getText() == val)
        return accept(type);
    return false;
}

bool Parser::root(){
    accept(TT::START);
    while(!accept(TT::EOFT))
        if(!segment())
            return false;
    return true;
}

bool Parser::endTemplate(){
    if(accept(TT::CLTEMPLATENONEW))
        return true;
    else if(accept(TT::CLTEMPLATENEW))
        return true;
    else
        return false;
}

bool Parser::segment() {

    //TEXT
    if(accept(TT::TEXT))
        return true;

    if( accept(TT::OPTEMPLATE) ){
        //STATEMENT
        if(statement()) {
            return endTemplate();
        }
        //FOR EXPR
        else if (forExpr() && endTemplate()){
            if(segment()){
                if(accept(TT::OPTEMPLATE) &&
                   accept(TT::ENDFOR) &&
                   endTemplate())
                    return true;
            }
        }
        //WHILE EXPR
        else if (whileExpr() && endTemplate()){
            if(segment()){
                if(accept(TT::OPTEMPLATE) &&
                accept(TT::ENDWHILE) &&
                endTemplate())
                    return true;
            }
        }
        else if (ifExpr() && endTemplate()){
            if(segment()){
                //IF ELSE
                if(accept(TT::OPTEMPLATE)&&
                   accept(TT::ELSE) &&
                   endTemplate()){
                    if(segment()){
                        if(accept(TT::OPTEMPLATE) &&
                        accept(TT::ENDELSE) &&
                        endTemplate())
                            return true;
                    }
                }

                //IF
                if(accept(TT::ENDIF) && endTemplate())
                    return true;
            }
        }
    }
    return false;
}

bool Parser::statement() {
    if(compoundExpr() && endTemplate())
        return true;
    else if (declaration() && endTemplate())
        return true;
    else
        return false;
}


bool Parser::compoundExpr() {
    if(expr()){
        if(accept(TT::MATHOP)){
            if(expr())
                return true;
            else if(accept(TT::OPBRACKET)){
                if(compoundExpr()){
                    if(accept(TT::CLBRACKET))
                        return true;
                }
            }
        }
        if(accept(TT::LOGICOP)){
            if(expr())
                return true;
            else if(accept(TT::OPBRACKET)){
                if(compoundExpr()){
                    if(accept(TT::CLBRACKET))
                        return true;
                }
            }
        }
    }
    else if(accept(TT::OPBRACKET)){
        if(compoundExpr()){
            if(accept(TT::CLBRACKET)){
                if (accept(TT::MATHOP)){
                    if(expr())
                        return true;
                    else if(accept(TT::OPBRACKET)){
                        if(compoundExpr()){
                            if (accept(TT::CLBRACKET))
                                return true;
                        }
                    }
                }
                if(accept(TT::COMPOP)){
                    if(expr())
                        return true;
                    else if(accept(TT::OPBRACKET)){
                        if(compoundExpr()){
                            if(accept(TT::CLBRACKET))
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;

}
bool Parser::compoundLogicExpr() {
    if(compoundExpr())
        return true;

    if(accept(TT::OPBRACKET)){
        if(compoundLogicExpr()){
            if(accept(TT::CLBRACKET)){
               if(accept(TT::LOGICOP, "and")){
                   if(accept(TT::OPBRACKET)){
                       if(compoundLogicExpr()){
                           if(accept(TT::CLBRACKET))
                               return true;
                       }
                   }
               } else if(accept(TT::LOGICOP, "or")){
                   if(accept(TT::OPBRACKET)){
                       if(compoundLogicExpr()){
                           if(accept(TT::CLBRACKET))
                               return true;
                       }
                   }
               }
            }
        }


    }
    else if (accept(TT::LOGICOP, "not")) {
        if (compoundLogicExpr())
            return true;
    }
    return false;
}

bool Parser::expr(){
    if(value())
        return true;
    else if (idOrNumOrBool()){
        if(accept(TT::MATHOP)){
            if(idOrNumOrBool())
                return true;
        }
        else if (accept(TT::LOGICOP)){
                if(idOrNumOrBool())
                    return true;
        }
    }
    return false;
}

bool Parser::declaration() {
    if(accept(TT::TYPE)){
        if(accept(TT::ID)){
           if(accept(TT::LOGICOP, "=")){
               return true;
           }
        }
    }
    return false;
}

bool Parser::value(){
    if(idOrNumOrBool())
        return true;
    //TODO: ESCAPE QUOTES
    return false;
}

bool Parser::idOrNumOrBool(){
    if (accept(TT::ID))
        return true;
    if (accept(TT::NUMBER))
        return true;
    if (accept(TT::BOOLVAL))
        return true;

    return false;
}

bool Parser::forExpr() {
    if(accept(TT::FOR)){
        if(accept(TT::ID)){
            if(accept(TT::IN)){
                if(accept(TT::ID))
                    return true;
            }
        }
    }
    return false;
}

bool Parser::whileExpr() {
    if(accept(TT::WHILE)){
        if(compoundLogicExpr())
            return true;
    }
    return false;
}

bool Parser::ifExpr() {
   if(accept(TT::IF)){
       if(compoundLogicExpr())
           return true;
   }
   return false;
}