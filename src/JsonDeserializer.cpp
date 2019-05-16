#include "JsonDeserializer.h"

#include <sstream>
#include <iostream>


std::string JsonDeserializer::getValueFromString(const std::string &str) const
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
                return "";
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
                //TODO: out of range check
                return j[index_number].dump();
            }

                //TODO: out of range check
            j = j[index_number];

        }
        else if (*it == '.')
        {
            if (it + 1 == str.end())
            {
                if(!j.contains(object_string))
                {
                    //TODO: Logger
                    return "";
                }
                return j[object_string].dump();
            }

            if(!j.contains(object_string))
            {
                //TODO: Logger
                return "";
            }
            j = j[object_string];
            object_string.clear();
        }
        else
        {
            object_string += *it;
        }

    }
    if(!j.contains(object_string))
    {
        //TODO: Logger
        return "";
    }
    return j[object_string].dump();
}
