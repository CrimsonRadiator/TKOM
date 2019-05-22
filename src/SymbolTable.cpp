#include <SymbolTable.h>
#include <Token.h>

#include "SymbolTable.h"
#include "Logger.h"

int SymbolTable::evaluateExpression(const Node &expr)
{

    auto &t = expr.token;
    if (t.getType() == TokenType::COMPOP)
    {
        if (t.getText() == ">")
            return evaluateExpression(*(expr.children[0])) >
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == ">=")
            return evaluateExpression(*(expr.children[0])) >=
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "<")
            return evaluateExpression(*(expr.children[0])) <
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "<=")
            return evaluateExpression(*(expr.children[0])) <=
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "==")
            return evaluateExpression(*(expr.children[0])) ==
                   evaluateExpression(*(expr.children[1]));
    }
    else if (t.getType() == TokenType::LOGICOP)
    {
        if (t.getText() == "and")
            return evaluateExpression(*(expr.children[0])) &&
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "or")
            return evaluateExpression(*(expr.children[0])) ||
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "not")
            return !evaluateExpression(*(expr.children[0]));
    }
    else if (t.getType() == TokenType::MATHOP)
    {
        if (t.getText() == "+")
            return evaluateExpression(*(expr.children[0])) +
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "-")
            return evaluateExpression(*(expr.children[0])) -
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "/")
            return evaluateExpression(*(expr.children[0])) /
                   evaluateExpression(*(expr.children[1]));
        else if (t.getText() == "%")
            return evaluateExpression(*(expr.children[0])) %
                   evaluateExpression(*(expr.children[1]));

    }
    else if (t.getType() == TokenType::NUMBER)
    {
        return t.getValue().integer;
    }
    else if (t.getType() == TokenType::BOOLVAL)
    {
        return t.getValue().boolean;
    }
    else if (t.getType() == TokenType::ID)
    {
        auto it = symbols.find(t.getValue().str);
        if (it == symbols.end())
        {
            TokenValue ret = jd.getValueFromString(t.getValue().str);
            if(ret.valueType == TokenValueType::STRING)
                return false;
            else if(ret.valueType == TokenValueType::INTEGER)
                return ret.integer;
            else
                return ret.boolean;
            //ERROR
            //TODO: Add logger info about error.
        }


        if (it->second.valueType == TokenValueType::STRING)
        {
            //ERROR
            //TODO: Add logger info about error.
            return false;
        }
        else if (it->second.valueType == TokenValueType::BOOLEAN)
        {
            return it->second.boolean;
        }
        else
        {
            return it->second.integer;
        }

    }

    //ERROR
    //TODO: Add logger info about error.

    return 0;
}

void SymbolTable::add(const Node &root)
{

    if (root.children[0]->token.getType() == TokenType::ASSIGNOP)
    {
        auto &variable = root.children[0]->children[0]->token;
        auto &value = root.children[0]->children[1]->token;

        std::string name = variable.getText();

        if (value.getType() == TokenType::ID)
        {
            auto it = symbols.find(value.getValue().str);

            if (it == symbols.end())
            {
                //TODO::Logging
                //Logger::getInstance().logUndefinedSymbol(it->second.first);
            }
            else
            {

                //SOMETHING
                //CONSTRUCTORS?
                symbols.emplace(name, it->second);
            }
        }
        else
        {
            //SOMETHING
            symbols.emplace(name, TokenValue(evaluateExpression(*(root.children[0]->children[1]))));
        }
        return;
    }
    for (auto &child : root.children)
        add(*child);

}
