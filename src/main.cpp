#include <iostream>
#include <fstream>

#include "json.h"
#include "Source.h"
#include "Scanner.h"
#include "Parser.h"
#include "CodeGenerator.h"
#include "JsonDeserializer.h"

using json = nlohmann::json;

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout<<"Usage: tgen JSONFILE INFILE";
    }

    Source src(argv[2]);
    std::ifstream fs(argv[1]);
    if(!fs)
        std::cout<<"Problem reading json file" <<argv[1];
    json j_data;
    fs >> j_data;


    Scanner scanner(src);
    Parser parser(scanner);
    CodeGenerator generator(j_data);
    std::unique_ptr<Node> root = parser.root();

    generator.generate(*root);

    return 0;
}
