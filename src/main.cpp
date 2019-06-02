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

    json j_data;
    if(argc == 3)
    {
        std::ifstream fs(argv[2]);
        if(!fs)
        {
            std::cout << "Problem reading json file" << argv[2];
            return 2;
        }
        fs >> j_data;
    }
    else if(argc != 2)
    {
        std::cout<<"Usage: tgen INFILE JSONFILE";
        return 1;
    }

    Source src(argv[1]);


    Scanner scanner(src);
    Parser parser(scanner);
    CodeGenerator generator(j_data);
    std::unique_ptr<Node> root = parser.root();

    generator.generate(*root);

    return 0;
}
