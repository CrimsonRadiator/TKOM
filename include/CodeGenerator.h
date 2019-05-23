#ifndef TKOM_SYMBOL_TABLE_H
#define TKOM_SYMBOL_TABLE_H

#include "Node.h"
#include "json.h"
#include "JsonDeserializer.h"

#include <memory>
#include <unordered_map>

using json = nlohmann::json;


class CodeGenerator
{
public:
    CodeGenerator(json data_) : jd{data_}
    {};

    void generate(const Node &root);

    int evaluateExpression(const Node &expr);

private:
    std::unordered_map<std::string, TokenValue> symbols;
    JsonDeserializer jd;
    void printTokenValue(const TokenValue& t) const;
};


#endif //TKOM_SYMBOL_TABLE_H

