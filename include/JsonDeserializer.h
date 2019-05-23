#ifndef TKOM_JSONDESERIALIZER_H
#define TKOM_JSONDESERIALIZER_H

#include "json.h"
#include "Token.h"
#include <vector>

using json = nlohmann::json;

class JsonDeserializer
{

public:
    explicit JsonDeserializer(const json &json_data_) : json_data{json_data_}
    {};

    TokenValue getValueFromString(const std::string &str) const;

    std::vector<TokenValue> getVectorFromString(const std::string& j) const;

private:
    json json_data;
    TokenValue jsonToTokenValue(const json& j) const;
    json getJsonFromString(const std::string& str) const;

};


#endif //TKOM_JSONDESERIALIZER_H
