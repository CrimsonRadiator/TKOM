#include "JsonDeserializer.h"

#include <sstream>
#include <iostream>
#include "Logger.h"

TokenValue JsonDeserializer::jsonToTokenValue(const json& j) const {
    if(j.is_number_integer())
        return TokenValue((int)j);
    if(j.is_boolean())
        return TokenValue((bool)j);
    if(j.is_string())
        return TokenValue(j.get<std::string>());
    //error
    Logger::getInstance().logBadJson(j.dump());

    return TokenValue(false);
}

json JsonDeserializer::getJsonFromString(const std::string &str) const
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
                Logger::getInstance().logBadJson(j.dump());
                return json();
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
                Logger::getInstance().logBadJson(j.dump());
                return j[object_string];
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
                    Logger::getInstance().logBadJson(j.dump());
                    return json();
                }
                //TODO:: type check
                return j[object_string];
            }

            if (!j.contains(object_string))
            {
                Logger::getInstance().logBadJson(j.dump());
                return json();
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
        Logger::getInstance().logBadJson(j.dump());
        return json();
    }
    //TODO:: type check;
    return j[object_string];

}

TokenValue JsonDeserializer::getValueFromString(const std::string &str) const
{
   return jsonToTokenValue(getJsonFromString(str));
}

std::vector<TokenValue> JsonDeserializer::getVectorFromString(const std::string& str) const
{
    json j = getJsonFromString(str);
    std::vector<TokenValue> result;
    if(j.is_array())
    {
        for(auto e : j)
        {
            result.push_back(jsonToTokenValue(e));
        }
    }
    return result;

}


