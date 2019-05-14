#include <iostream>

#include "json.h"
#include "Source.h"
#include "Scanner.h"

using json = nlohmann::json;

int main() {
    Source src("a.txt");
    Scanner scanner(src);
    while(src.getCurr()!=EOF) {
        Token t = scanner.getNextToken();
        std::cout << "TYPE: "+scanner.getTypeName(t)<< " DATA: "<<t.getText()<<"\n";
    }


    return 0;
}
