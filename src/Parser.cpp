#include "Parser.h"


typedef TokenType TT;
typedef NodeType NT;
typedef std::unique_ptr<Node> NP;

void Parser::nextSymbol()
{
    currentToken = scanner.getNextToken();
}


NP Parser::accept(TT type)
{
    NP result;
    if (currentToken.getType() == type)
    {
        result = std::make_unique<Node>(currentToken);
        nextSymbol();
    }
    return result;
}

NP Parser::accept(TT type, const std::string &val)
{
    if (currentToken.getText() == val)
        return accept(type);
    return nullptr;
}

NP Parser::root()
{
    auto root = accept(TT::START);
    root->type = NT::START;
    while (auto seg = segment())
    {
        root->add(seg);
    }
    if (root->children.empty())
        return failure();
    return root;
}

NP Parser::multipleSegments()
{
    auto segments = std::make_unique<Node>(Token(TT::START, ""));
    segments->type = NT::MULTIPLE_SEGMENTS;

    while (auto seg = segment())
    {
        segments->add(seg);
    }
    if (segments->children.empty())
        return nullptr;

    return segments;
}

bool Parser::endTemplate(NP &t)
{
    auto result = accept(TT::CLTEMPLATENONEW);
    if (result)
    {
        t->newline = false;
        return true;
    }
    result = accept(TT::CLTEMPLATENEW);
    if (result)
    {
        t->newline = true;
        return true;
    }

    return false;

}

NP Parser::segment()
{
    auto seg = accept(TT::TEXT);
    if (seg)
    {
        seg->type = NT::TOKEN;
        return seg;
    }
    if (!(seg = accept(TT::OPTEMPLATE)))
        return nullptr;
    else if (seg = forExpr())
    {
        seg->type = NT::FOR_LOOP;
        auto body = multipleSegments();
        if (!body)
            return nullptr;
        seg->add(body);
        auto end = keyword(TT::ENDFOR);
        if (!end)
        {
            end = keywordSkip(TT::ENDFOR);
            if (!end)
                return nullptr;
        }
        seg->add(end);
        return seg;
    }
    else if ((seg = whileExpr()))
    {
        seg->type = NT::WHILE_LOOP;
        auto body = multipleSegments();
        if (!body)
            return nullptr;
        seg->add(body);
        auto end = keyword(TT::ENDWHILE);
        if (!end)
        {
            end = keywordSkip(TT::ENDWHILE);
            if (!end)
                return nullptr;
        }
        seg->add(end);
        return seg;
    }
    else if ((seg = ifExpr()))
    {
        seg->type = NT::IF;
        auto body = multipleSegments();
        if (!body)
            return nullptr;
        seg->add(body);
        auto middle = keyword(TT::ELSE);
        if (!middle)
        {
            middle = keywordSkip(TT::ELSE);
            if (middle)
                middle->type = NT::ELSE;
            if (!middle)
            {
                middle = keywordSkip(TT::ENDIF);
                if (!middle)
                    return nullptr;
                seg->add(middle);
                return seg;
            }
        }
        seg->type = NT::IF_ELSE;
        body = multipleSegments();
        if (!body)
            return nullptr;
        auto end = keyword(TT::ENDELSE);
        if (!end)
        {
            end = keywordSkip(TT::ENDELSE);
            if (!end)
                return nullptr;
        }
        middle->add(body);
        middle->add(end);
        seg->add(middle);
        return seg;
    }
    else if ((seg = expr()))
    {
        seg->type = NT::STATEMENT;
        if (!endTemplate(seg))
            return seg;
        return seg;
    }
    else if ((seg = declaration()))
    {
        return seg;
    }


    return seg;
}


NP Parser::keyword(TT type)
{
    auto tmp = accept(TT::OPTEMPLATE);
    if (!tmp)
        return nullptr;
    return keywordSkip(type);
}

NP Parser::keywordSkip(TT type)
{
    auto t = accept(type);
    if (!t)
        return nullptr;
    auto tmp = accept(TT::CLTEMPLATENEW);
    if (!tmp)
    {
        tmp = accept(TT::CLTEMPLATENONEW);
        if (!tmp)
            return nullptr;
    }
    return t;
}


NP Parser::compoundExpr()
{
    //simple expression
    auto e = expr();
    if (e)
        return e;

    //not expression
    if (auto not_expr = accept(TT::LOGICOP, "not"))
    {
        auto child = compoundExpr();
        if (!child)
            return nullptr;
        not_expr->add(child);
        not_expr->type = NT::NOT_EXPR;
        return not_expr;
    }
    //compound expression
    if (auto tmp = accept(TT::OPBRACKET))
    {
        auto lhs = compoundExpr();
        if (!lhs)
            return nullptr;
        tmp = accept(TT::CLBRACKET);
        if (!tmp)
            return nullptr;

        auto op = accept(TT::LOGICOP, "and");
        if (op)
        {
            op->type = NT::AND_EXPR;
        }
        else if (op = accept(TT::LOGICOP, "or"))
        {
            op->type = NT::OR_EXPR;
        }
        else if (op = accept(TT::MATHOP))
        {
            op->type = NT::MATH_EXPR;
        }
        else if (op = accept(TT::COMPOP))
        {
            op->type = NT::COMP_EXPR;

        }

        tmp = accept(TT::OPBRACKET);
        if (!tmp)
            return nullptr;
        auto rhs = compoundExpr();
        if (!rhs)
            return nullptr;
        tmp = accept(TT::CLBRACKET);
        if (!tmp)
            return nullptr;
        op->add(lhs);
        op->add(rhs);
        return op;
    }
    return nullptr;
}


NP Parser::expr()
{
    auto x = value();

    if (!x)
        return nullptr;
    auto mathop = accept(TT::MATHOP);
    if (mathop)
    {
        auto y = value();
        if (!y)
            return nullptr;
        mathop->add(x);
        mathop->add(y);
        mathop->type = NT::MATH_EXPR;
        return mathop;
    }
    auto logicop = accept(TT::COMPOP);
    if (logicop)
    {
        auto y = value();
        if (!y)
            return nullptr;
        logicop->add(x);
        logicop->add(y);
        logicop->type = NT::COMP_EXPR;
        return logicop;
    }

    return x;
}

NP Parser::declaration()
{
    auto type = accept(TT::TYPE);
    if (!type)
        return nullptr;
    auto x = accept(TT::ID);
    if (!x)
        return nullptr;
    auto op = accept(TT::ASSIGNOP, "=");
    if (!op)
        return nullptr;
    auto y = compoundExpr();
    if (!y)
        return nullptr;
    op->add(x);
    op->add(y);
    type->add(op);
    type->type = NT::DECLARATION;
    if (!endTemplate(type))
        return nullptr;

    return type;
}

NP Parser::value()
{
    auto x = accept(TT::ID);
    if (x)
    {
        return x;
    }
    else if ((x = accept(TT::NUMBER)))
    {
        return x;
    }
    else if ((x = accept(TT::BOOLVAL)))
    {
        return x;
    }

    return nullptr;

}

NP Parser::forExpr()
{
    auto head = accept(TT::FOR);
    if (!head)
    {
        return nullptr;
    }
    auto type = accept(TT::TYPE);
    if (!type)
        return nullptr;
    auto x = accept(TT::ID);
    if (!x)
        return nullptr;
    x->type = NT::TOKEN;
    auto in = accept(TT::IN);
    if (!in)
        return nullptr;
    auto y = accept(TT::ID);
    if (!y)
        return nullptr;
    head->add(type);
    head->add(x);
    head->add(y);
    if (!endTemplate(head))
        return nullptr;
    return head;
}

NP Parser::whileExpr()
{
    auto head = accept(TT::WHILE);
    if (!head)
        return nullptr;
    auto cle = compoundExpr();
    if (!cle)
        return nullptr;
    head->add(cle);
    if (!endTemplate(head))
        return nullptr;
    return head;
}


NP Parser::ifExpr()
{
    auto head = accept(TT::IF);
    if (!head)
        return nullptr;
    auto cle = compoundExpr();
    if (!cle)
        return nullptr;
    head->add(cle);
    if (!endTemplate(head))
        return nullptr;
    return head;
}

NP Parser::failure()
{
    Logger::getInstance().logParserFailure(scanner.getLine());
    return nullptr;
}

