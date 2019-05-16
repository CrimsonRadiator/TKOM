#ifndef TKOM_TEMPLATEGENERATOR_H
#define TKOM_TEMPLATEGENERATOR_H

#include <string>
#include <fstream>

class TemplateGenerator {
public:
    TemplateGenerator(const std::string& filename) :
        outStream(filename)
    {};

private:
    std::ofstream outStream;
};


#endif //TKOM_TEMPLATEGENERATOR_H
