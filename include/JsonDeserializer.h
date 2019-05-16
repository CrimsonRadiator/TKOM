#ifndef TKOM_JSONDESERIALIZER_H
#define TKOM_JSONDESERIALIZER_H

#include "json.h"

using json = nlohmann::json;

class JsonDeserializer {

public:
    explicit JsonDeserializer(const json& json_data_) : json_data{json_data_} {};

    std::string getValueFromString(const std::string& str) const;
private:
    json json_data;
};


#endif //TKOM_JSONDESERIALIZER_H
