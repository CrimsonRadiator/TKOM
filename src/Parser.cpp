#include "Parser.h"

//FIX endTemplate(
// )

typedef TokenType TT;
typedef NodeType NT;
typedef std::unique_ptr<Node> NP;

void Parser::nextSymbol(){
    currentToken = scanner.getNextToken();
}


NP Parser::accept(TT type) {
     NP result;
    if(currentToken.getType() == type) {
        result = std::make_unique<Node>(currentToken);
        nextSymbol();
    }
    return result;
}

NP Parser::accept(TT type, const std::string& val){
    if (currentToken.getText() == val)
        return accept(type);
    return nullptr;
}

NP Parser::root(){
    auto root = accept(TT::START);
    root->type = NT::START;
    while(auto seg = segment()){
        root->add(seg);
    }
    return root;
}

bool Parser::endTemplate(NP& t){
    auto result = accept(TT::CLTEMPLATENONEW);
    if(result) {
        t->newline = false;
        return true;
    }
    result = accept(TT::CLTEMPLATENEW);
    if(result) {
        t->newline = true;
        return true;
    }

    return false;

}

NP Parser::segment() {
    auto seg = accept(TT::TEXT);
    if (seg) {
        seg->type = NT::TOKEN;
        return seg;
    }
    if (!(seg = accept(TT::OPTEMPLATE)))
        return seg;
    else if (seg = forExpr()) {
        seg->type = NT::FOR_LOOP;
        auto body = segment();
        seg->add(body);
        auto end = keyword(TT::ENDFOR);
        seg->add(end);
    } else if ((seg= whileExpr())) {
        seg->type = NT::WHILE_LOOP;
        auto body = segment();
        seg->add(body);
        auto end = keyword(TT::ENDWHILE);
        seg->add(end);
    } else if ((seg= ifExpr())) {
        seg->type = NT::IF;
        auto body = segment();
        seg->add(body);
        auto middle = keyword(TT::ELSE);
        if (!middle) {
            middle = keywordSkip(TT::ENDIF);
            seg->add(middle);
            return seg;
        }
        seg->type = NT::IF_ELSE;
        body = segment();
        body->type = NT::ELSE;
        auto end = keyword(TT::ENDELSE);
        middle->add(body);
        middle->add(end);
        seg->add(middle);
        return seg;
    } else if((seg= expr())) {
        seg->type = NT::STATEMENT;
        if(!endTemplate(seg))
            return nullptr;
        seg->add(seg);
    } else if((seg= declaration())){
        return seg;
    }
    return seg;
}


NP Parser::keyword(TT type){
    auto tmp = accept(TT::OPTEMPLATE);
    if(!tmp)
        return nullptr;
    return keywordSkip(type);
}

NP Parser::keywordSkip(TT type){
    auto t = accept(type);
    if(!t)
        return nullptr;
    auto tmp = accept(TT::CLTEMPLATENEW);
    if(!tmp){
        tmp = accept(TT::CLTEMPLATENONEW);
        if(!tmp)
            return nullptr;
    }
    return t;
}


NP Parser::compoundExpr() {
    auto e = expr();
    if(e)
        return e;
    if(auto tmp = accept(TT::OPBRACKET)){
        auto lhs = compoundExpr();
        if(!lhs)
            return nullptr;
        tmp = accept(TT::CLBRACKET);
        if(!tmp)
            return nullptr;

        auto op = accept(TT::MATHOP);
        if(!op){
            op = accept(TT::LOGICOP);
            if(!op)
                return nullptr;
        }
        tmp = accept(TT::OPBRACKET);
        if(!tmp)
            return nullptr;
        auto rhs = compoundExpr();
        if(!rhs)
            return nullptr;
        tmp = accept(TT::CLBRACKET);
        if(!tmp)
            return nullptr;
        op->add(lhs);
        op->add(rhs);
        op->type = NT::COMPOUND_EXPR;
        return op;
    }
    return nullptr;

}

NP Parser::compoundLogicExpr() {
    auto expr = compoundExpr();
    if(expr)
        return expr;

    if(auto n = accept(TT::LOGICOP, "not")){
        auto child = compoundExpr();
        n->add(child);
        n->type = NT::LOGICOP;
        return n;
    }
    if(auto tmp = accept(TT::OPBRACKET)){
        auto lhs = compoundExpr();
        tmp = accept(TT::CLBRACKET);
        if(!tmp)
            return nullptr;
        if(!lhs)
            return nullptr;
        auto op = accept(TT::LOGICOP, "and");
        if(!op){
            op = accept(TT::LOGICOP, "or");
            if(!op)
                return nullptr;
        }
        tmp = accept(TT::OPBRACKET);
        if(!tmp)
            return nullptr;
        auto rhs = compoundExpr();
        if(!rhs)
            return nullptr;
        tmp = accept(TT::CLBRACKET);
        if(!tmp)
            return nullptr;
        op->add(lhs);
        op->add(rhs);
        op->type = NT::LOGICOP;
        return op;
    }
    return nullptr;
}

NP Parser::expr(){
    auto x = value();

    if(!x)
        return nullptr;
    auto mathop = accept(TT::MATHOP);
    if(mathop){
        auto y = value();
        if(!y)
            return nullptr;
        mathop->add(x);
        mathop->add(y);
        return mathop;
    }
    auto logicop = accept(TT::COMPOP);
    if(logicop){
        auto y = value();
        if(!y)
            return nullptr;
        logicop->add(x);
        logicop->add(y);
        return logicop;
    }

    return x;
}

NP Parser::declaration() {
    auto type = accept(TT::TYPE);
    if(!type)
        return nullptr;
    auto x = accept(TT::ID);
    if(!x)
        return nullptr;
    auto op = accept(TT::ASSIGNOP, "=");
    if(!op)
        return nullptr;
    auto y = compoundExpr();
    if(!y)
        return nullptr;
    op->add(x);
    op->add(y);
    type->add(op);
    type->type = NT::DECLARATION;
    if(!endTemplate(type))
        return nullptr;

    return type;
}

NP Parser::value(){
    auto x = accept(TT::ID);
    if(x) {
        return x;
    }
    else if((x = accept(TT::NUMBER))) {
        return x;
    }
    else if((x = accept(TT::BOOLVAL))) {
        return x;
    }

    return nullptr;

}

NP Parser::forExpr() {
    auto head = accept(TT::FOR);
    if(!head) {
        return nullptr;
    }
    auto x = accept(TT::ID);
    if(!x)
        return nullptr;
    x->type = NT::TOKEN;
    auto in = accept(TT::IN);
    if(!in)
        return nullptr;
    auto y = accept(TT::ID);
    if(!y)
        return nullptr;
    y->type = NT::TOKEN;
    head->add(x);
    head->add(y);
    if(!endTemplate(head))
        return nullptr;
    return head;
}

NP Parser::whileExpr() {
    auto head = accept(TT::WHILE);
    if(!head)
        return nullptr;
    auto cle = compoundLogicExpr();
    if(!cle)
        return nullptr;
    cle->type = NT::COMPOUND_LOGIC_EXPR;
    head->add(cle);
    if(!endTemplate(head))
        return nullptr;
    return head;
}



NP Parser::ifExpr() {
    auto head = accept(TT::IF);
    if (!head)
        return nullptr;
    auto cle = compoundLogicExpr();
    if (!cle)
        return nullptr;
    cle->type = NT::COMPOUND_LOGIC_EXPR;
    head->add(cle);
    if(!endTemplate(head))
        return nullptr;
    return head;
}



