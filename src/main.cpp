#include <iostream>

#include "json.h"
#include "Source.h"
#include "Scanner.h"
#include "Parser.h"
//#include "SymbolTable.h"
#include "JsonDeserializer.h"

using json = nlohmann::json;

int main()
{
    Source src("a.txt");
    std::ifstream fs("../data.json");
    json j_data;
    fs >> j_data;

    //std::cout<<j_data["pi"];
    JsonDeserializer deserializer(j_data);
    std::cout << deserializer.getValueFromString("happy").boolean;
    std::cout << deserializer.getValueFromString("answer.everything").integer;
    std::cout << deserializer.getValueFromString("name").str;

    //Scanner scanner(src);
    //Parser parser(scanner);
    //SymbolTable symbolTable;

    //std::unique_ptr<Node> root = parser.root();
    //symbolTable.add(*root);



    return 0;
}
