#ifndef TKOM_JSONDESERIALIZER_H
#define TKOM_JSONDESERIALIZER_H

#include "json.h"
#include <Token.h>

using json = nlohmann::json;

class JsonDeserializer
{

public:
    explicit JsonDeserializer(const json &json_data_) : json_data{json_data_}
    {};

    TokenValue getValueFromString(const std::string &str) const;

private:
    json json_data;
};


#endif //TKOM_JSONDESERIALIZER_H
