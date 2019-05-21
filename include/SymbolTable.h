#ifndef TKOM_SYMBOL_TABLE_H
#define TKOM_SYMBOL_TABLE_H

#include "Node.h"
#include "json.h"
#include "JsonDeserializer.h"

#include <memory>
#include <unordered_map>

using json = nlohmann::json;


class SymbolTable
{
public:
    SymbolTable(json data_) : jd{data_}
    {};

    void add(const Node &root);

    void add(json &j);

    int evaluateExpression(const Node &expr);

private:
    std::unordered_map<std::string, Value> symbols;
    JsonDeserializer jd;
};


#endif //TKOM_SYMBOL_TABLE_H

