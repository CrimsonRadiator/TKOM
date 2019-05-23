#include <CodeGenerator.h>
#include <Token.h>

#include "CodeGenerator.h"
#include "Logger.h"

void CodeGenerator::printTokenValue(const TokenValue& tv) const
{
    switch (tv.valueType)
    {
        case TokenValueType::STRING:
            std::cout << tv.str;
            break;
        case TokenValueType::INTEGER:
            std::cout << tv.integer;
            break;
        case TokenValueType::BOOLEAN:
            std::cout << tv.boolean;
            break;
    }
}

int CodeGenerator::evaluateExpression(const Node &expr)
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
            {
                //std::cout<<"STRING "<<ret.str;
                return false;
            }
            else if(ret.valueType == TokenValueType::INTEGER)
            {
                //std::cout<<"INTEGER "<<ret.integer;
                return ret.integer;
            }
            else
            {
                //std::cout<<"BOOLEAN "<<ret.boolean;
                return ret.boolean;
            }
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

void CodeGenerator::generate(const Node &root)
{
    TokenType type = root.token.getType();
    TokenValue value = root.token.getValue();

    switch(type)
    {
        case TokenType::START:
            for(auto& c : root.children)
                generate(*c);
            break;
        case TokenType::MULTIPLE_SEGMENTS:
            for (auto& c : root.children)
                generate(*c);
            break;
        case TokenType::IF:
            if(evaluateExpression(*root.children[0]))
            {
                generate(*root.children[1]);
            }
            else if(root.children[2]->token.getType() == TokenType::ELSE)
            {
                generate(*(root.children[2]->children[0]));
            }
            break;
        case TokenType::WHILE:
            while(evaluateExpression(*root.children[0]))
            {
               generate(*root.children[1]);
            }
            break;
        case TokenType::FOR:
            {
            auto var_name = root.children[2]->token.getText();
            for(auto e : jd.getVectorFromString(var_name))
            {
                symbols[var_name] = e;
                generate(*root.children[1]);
            }
            break;
        }
        case TokenType::TYPE:
        {
            auto var_name = root.children[0]->children[0]->token.getText();
            auto var_value = evaluateExpression(*(root.children[0]->children[1]));

            symbols[var_name] = TokenValue(var_value);
            break;
        }
        case TokenType::ID:
        {
            std::string var_name = root.token.getValue().str;
            auto it = symbols.find(var_name);
            if(it == symbols.end())
            {
                TokenValue ret = jd.getValueFromString(var_name);
                printTokenValue(ret);
            }
            break;
        }
        default:
            printTokenValue(value);
            break;
    }
}
