#define BOOST_TEST_MODULE TKOMtests

#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <string>

#include "Source.h"
#include "Scanner.h"
#include "Parser.h"


typedef std::pair<std::string, std::string> PSS;

//quick fix
namespace std {
    std::ostream &operator<<(std::ostream &os,
                            const std::pair<std::string, std::string> &pss) {
        os << pss.first << " " << pss.second;
        return os;
    }

}



class FakeSource : public Source{
public:
    explicit FakeSource(std::string& testText_) : Source(),
                                        testText{testText_},
                                        fakeSourceStream{testText_}


    {}

    explicit FakeSource() = default;

    void loadData(std::string& testText_){
        fakeSourceStream<< testText_;
    }

    char getNextChar() override {

        if(!fakeSourceStream.get(currentChar))
            currentChar = EOF;

        if (currentChar == '\n') {
            ++lineNumber;
            charNumber = 0;
        } else {
            ++charNumber;
        }
        return currentChar;
    }

    char peekNextChar() override {
        return fakeSourceStream.peek();
    }

private:
    std::string testText;
    std::stringstream fakeSourceStream;
};

class F{
public:
    F() :   scanner{source},
            counter{0}
    {}

    unsigned read(bool print){

        for (Token t = scanner.getNextToken();
            t.getType() != TokenType::EOFT;
            t = scanner.getNextToken()){
            ++counter;
            if(print)
                std::cout<<scanner.getTypeName(t)<<" : "<<t.getText()<<std::endl;
            resultTokens.emplace_back(std::make_pair(scanner.getTypeName(t), t.getText()));
        }
        return counter;
    }

    ~F() = default;

    std::vector<std::pair<std::string, std::string>> resultTokens;
    std::vector<std::pair<std::string, std::string>> testTokens;
    FakeSource source;
    Scanner scanner;
    unsigned counter;
};

class P : public F{
public:
    P() : F(), parser{scanner} {};
    Parser parser;

    bool parse(){
        while(parser.getCurrentToken().getType() != TokenType::EOFT){
            return parser.root();
        }
        return true;
    }
};


BOOST_FIXTURE_TEST_SUITE(tkom_tests_suite, F)


    BOOST_AUTO_TEST_CASE(emptyString) {
        std::string str = "";
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(true), 0);
    }

    BOOST_AUTO_TEST_CASE(oneLetter) {
        std::string str = "a";
        testTokens.emplace_back(std::make_pair("TEXT", "a"));
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(true), 1);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );

    }

    BOOST_AUTO_TEST_CASE(randomText) {
        std::string str = "this is random text\neven more @#$%^&\n*#$)(&^%$#$%^&*";
        testTokens.emplace_back(std::make_pair("TEXT", str));
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(true), 1);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );

    }

    BOOST_AUTO_TEST_CASE(randomTextWithKeywords) {
        std::string str = "Try this{ { } { \n} } \n{ }} or not \n for endwhile in";
        str+="for i in a.code = % != >= <= < > ! not and or ";
        testTokens.emplace_back(std::make_pair("TEXT", str));
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(true), 1);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );

    }

    BOOST_AUTO_TEST_CASE(textAndTemplate) {
        std::string str = "Outside template {{ inside}} outside{{ insideagain}} outsideagain";
        testTokens.emplace_back(std::make_pair("TEXT", "Outside template "));
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ID", "inside"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        testTokens.emplace_back(std::make_pair("TEXT", " outside"));
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ID", "insideagain"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        testTokens.emplace_back(std::make_pair("TEXT", " outsideagain"));

        source.loadData(str);
        read(true);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );

    }

    BOOST_AUTO_TEST_CASE(temlatesWithNewlines) {
        std::string str = "{{ inside}}{{insideagain}}\n asdf";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ID", "inside"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ID", "insideagain"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENEW", "}}"));
        testTokens.emplace_back(std::make_pair("TEXT", " asdf"));

        source.loadData(str);
        read(true);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );
    }


    BOOST_AUTO_TEST_CASE(declarationsAndTypes) {
        std::string str = "{{int x = 10}}{{string a = fff}}{{bool z = false}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("TYPE", "int"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("ASSIGNOP", "="));
        testTokens.emplace_back(std::make_pair("NUMBER", "10"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("TYPE", "string"));
        testTokens.emplace_back(std::make_pair("ID", "a"));
        testTokens.emplace_back(std::make_pair("ASSIGNOP", "="));
        testTokens.emplace_back(std::make_pair("ID", "fff"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("TYPE", "bool"));
        testTokens.emplace_back(std::make_pair("ID", "z"));
        testTokens.emplace_back(std::make_pair("ASSIGNOP", "="));
        testTokens.emplace_back(std::make_pair("BOOLVAL", "false"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));


        source.loadData(str);
        read(true);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );
    }

    BOOST_AUTO_TEST_CASE(whileLoop) {
        std::string str = "{{while x > 0}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("WHILE", "while"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        testTokens.emplace_back(std::make_pair("NUMBER", "0"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(forLoop) {
        std::string str = "{{for x in y}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("FOR", "for"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("IN", "in"));
        testTokens.emplace_back(std::make_pair("ID", "y"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(ifElse) {
        std::string str = "{{if x > 0}}text1{{else}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("IF", "if"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        testTokens.emplace_back(std::make_pair("NUMBER", "0"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        testTokens.emplace_back(std::make_pair("TEXT", "text1"));
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ELSE", "else"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }


     BOOST_AUTO_TEST_CASE(endKeywords) {
        std::string str = "{{endfor}}{{endwhile}}{{endif}}{{endelse}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ENDFOR", "endfor"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ENDWHILE", "endwhile"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ENDIF", "endif"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ENDELSE", "endelse"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }


    BOOST_AUTO_TEST_CASE(operators) {
        std::string str = "{{  = == < > >= <= and or not % + - / }}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ASSIGNOP", "="));
        testTokens.emplace_back(std::make_pair("COMPOP", "=="));
        testTokens.emplace_back(std::make_pair("COMPOP", "<"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">="));
        testTokens.emplace_back(std::make_pair("COMPOP", "<="));
        testTokens.emplace_back(std::make_pair("LOGICOP", "and"));
        testTokens.emplace_back(std::make_pair("LOGICOP", "or"));
        testTokens.emplace_back(std::make_pair("LOGICOP", "not"));
        testTokens.emplace_back(std::make_pair("MATHOP", "%"));
        testTokens.emplace_back(std::make_pair("MATHOP", "+"));
        testTokens.emplace_back(std::make_pair("MATHOP", "-"));
        testTokens.emplace_back(std::make_pair("MATHOP", "/"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(operatorsAndBraces) {
        std::string str = "{{ ((a==b)and(x or b))}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("ID", "a"));
        testTokens.emplace_back(std::make_pair("COMPOP", "=="));
        testTokens.emplace_back(std::make_pair("ID", "b"));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));
        testTokens.emplace_back(std::make_pair("LOGICOP", "and"));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("LOGICOP", "or"));
        testTokens.emplace_back(std::make_pair("ID", "b"));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(complexLogic1) {
        std::string str = "{{(x > 0 and (z % 2 == 4))}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        testTokens.emplace_back(std::make_pair("NUMBER", "0"));
        testTokens.emplace_back(std::make_pair("LOGICOP", "and"));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("ID", "z"));
        testTokens.emplace_back(std::make_pair("MATHOP", "%"));
        testTokens.emplace_back(std::make_pair("NUMBER", "2"));
        testTokens.emplace_back(std::make_pair("COMPOP", "=="));
        testTokens.emplace_back(std::make_pair("NUMBER", "4"));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));

        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(escapeChars) {
        std::string str = "alamakota \\{\\{ for x in y\\}\\}";
        testTokens.emplace_back(std::make_pair("TEXT", "alamakota {{ for x in y}}"));
        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(jsonIDexamples) {
        std::string str = "{{ abc abc.abc.abc[4567] abc[123] }}";

        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ID", "abc"));
        testTokens.emplace_back(std::make_pair("ID", "abc.abc.abc[4567]"));
        testTokens.emplace_back(std::make_pair("ID", "abc[123]"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        source.loadData(str);
        read(true);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()



BOOST_FIXTURE_TEST_SUITE(parse_tests_suite, P)


    BOOST_AUTO_TEST_CASE(parseTemplate) {
        std::string text = "{{ token }}";
        source.loadData(text);
        BOOST_CHECK_EQUAL(parse(), true);
    }

    BOOST_AUTO_TEST_CASE(parseBadTemplate) {
        std::string text = "{{ 1 = 1 }}";
        source.loadData(text);
        BOOST_CHECK_EQUAL(parse(), false);
    }

    BOOST_AUTO_TEST_CASE(parseText) {
        std::string text = "{ 1 = 1 }}";
        source.loadData(text);
        BOOST_CHECK_EQUAL(parse(), true);
    }

    BOOST_AUTO_TEST_CASE(parseDefinition) {
        std::string text = "{{ x = 1 }}";
        source.loadData(text);
        BOOST_CHECK_EQUAL(parse(), true);
    }

    BOOST_AUTO_TEST_CASE(parseStatement) {
    
    }
}




BOOST_AUTO_TEST_SUITE_END()