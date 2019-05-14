#include <iostream>

#include "json.h"
#include "Source.h"
#include "Scanner.h"
#include "Parser.h"

using json = nlohmann::json;

int main() {
    Source src("a.txt");
    Scanner scanner(src);
    Parser parser(scanner);


    return 0;
}
