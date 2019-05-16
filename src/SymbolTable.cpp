
#include <SymbolTable.h>

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
        int val;
        std::istringstream(t.getText()) >> val;
        return val;
    }
    else if (t.getType() == TokenType::BOOLVAL)
    {
        return (t.getText() == "true");
    }
    else if (t.getType() == TokenType::ID)
    {
        auto it = symbols.find(t.getText());
        if (it == symbols.end())
        {
            if (!jd.getValueFromString(t.getText()).empty())
            {
                //TODO: Return value;
            }
            //ERROR
            //TODO: Add logger info about error.
            return 0;
        }

        std::string value = it->second.first;
        ValueType type = it->second.second;

        if (type == ValueType::STRING)
        {
            //ERROR
            //TODO: Add logger info about error.
            return 0;
        }
        else if (type == ValueType::BOOLEAN)
        {
            return (value == "true");
        }
        else
        {
            //string to integer conversion
            int tmp;
            std::istringstream(value) >> tmp;
            return tmp;
        }

    }

    //ERROR
    //TODO: Add logger info about error.
    return 0;
}

void SymbolTable::add(const Node &root)
{

    if (root.type == NodeType::DECLARATION)
    {
        auto &variable = root.children[0]->children[0]->token;
        auto &value = root.children[0]->children[1]->token;

        std::string name = variable.getText();

        if (value.getType() == TokenType::ID)
        {
            auto it = symbols.find(value.getText());

            if (it == symbols.end())
            {
                //TODO::Logging
                //Logger::getInstance().logUndefinedSymbol(it->second.first);
            }
            else
            {

                //symbols[name] = std::make_pair(it->second.first, it->second.second);
            }
        }
        else
        {
            //TODO: return pair (value, type).
            //symbols[name] = std::make_pair(evaluateExpression(*(root.children[0]->children[1])), ValueType::INTEGER);
        }
        return;
    }
    for (auto &child : root.children)
        add(*child);

}
