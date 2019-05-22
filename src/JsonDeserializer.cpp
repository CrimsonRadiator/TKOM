#include "JsonDeserializer.h"

#include <sstream>
#include <iostream>

TokenValue JsonDeserializer::jsonToTokenValue(const json& j) const {
    if(j.is_boolean())
        return TokenValue((bool)j);
    if(j.is_number_integer())
        return TokenValue((int)j);
    if(j.is_string())
        return TokenValue(j.get<std::string>());
    //error
    //TODO: Logger
    return TokenValue(false);
}

TokenValue JsonDeserializer::getValueFromString(const std::string &str) const
{
    std::string object_string;
    json j(json_data);
    j = j[0];

    for (auto it = str.begin(); it != str.end(); ++it)
    {

        if (*it == '[')
        {
            if (!j.contains(object_string))
            {
                //TODO: Logger
                return TokenValue("");
            }
            j = j[object_string];

            object_string.clear();

            std::string index_string;

            for (++it; *it != ']'; ++it)
            {
                index_string += *it;
            }

            ++it;
            unsigned index_number;
            std::istringstream(index_string) >> index_number;

            if (it == str.end())
            {
                //TODO: out of range check, type check
                return jsonToTokenValue(j[object_string]);
            }

            //TODO: out of range check
            j = j[index_number];

        }
        else if (*it == '.')
        {
            if (it + 1 == str.end())
            {
                if (!j.contains(object_string))
                {
                    //TODO: Logger
                    return TokenValue("");
                }
                //TODO:: type check
                return jsonToTokenValue(j[object_string]);
            }

            if (!j.contains(object_string))
            {
                //TODO: Logger
                return TokenValue("");
            }
            j = j[object_string];
            object_string.clear();
        }
        else
        {
            object_string += *it;
        }

    }
    if (!j.contains(object_string))
    {
        //TODO: Logger
        return TokenValue("");
    }
    //TODO:: type check;
    return jsonToTokenValue(j[object_string]);
}
