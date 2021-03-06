#define BOOST_TEST_MODULE TKOMtests

#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <string>
#include <memory>

#include "Node.h"
#include "Source.h"
#include "Scanner.h"
#include "Parser.h"
#include "JsonDeserializer.h"


typedef TokenType TT;
typedef std::unique_ptr<Node> NP;


//quick fix
namespace std {
    std::ostream &operator<<(std::ostream &os,
                            const std::pair<std::string, std::string> &pss) {
        os << pss.first << " " << pss.second;
        return os;
    }

    std::ostream &operator<<(std::ostream &os,
                            const std::pair<std::string, TokenValue> & stv) {
        switch(stv.second.valueType){
            case TokenValueType::STRING:
                os << stv.first << " " << stv.second.str; break;
            case TokenValueType::INTEGER:
                os << stv.first << " " << stv.second.integer; break;
            case TokenValueType::BOOLEAN:
                os << stv.first << " " << std::boolalpha<<stv.second.boolean; break;
        }
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
            resultTokens.emplace_back(std::make_pair(scanner.getTypeName(t), t.getValue()));
        }
        return counter;
    }

    ~F() = default;

    std::vector<std::pair<std::string, TokenValue>> resultTokens;
    std::vector<std::pair<std::string, TokenValue>> testTokens;
    FakeSource source;
    Scanner scanner;
    unsigned counter;
};

class P : public F{
public:
    P() : F(), parser{scanner} {};
    Parser parser;

    std::unique_ptr<Node> parse(){
        while(parser.getCurrentToken().getType() != TokenType::EOFT){
            return parser.root();
        }
        return nullptr;
    }


};

NP make_node(TT ttype, const std::string& str){
    NP result = std::make_unique<Node>(Token(ttype, str));
    return result;
}

NP make_node(TT ttype, const char* chars_){
    NP result = std::make_unique<Node>(Token(ttype, std::string(chars_)));
    return result;
}


NP make_node(TT ttype, int integer){
    NP result = std::make_unique<Node>(Token(ttype, integer));
    return result;
}

NP make_node(TT ttype, bool boolean){
    NP result = std::make_unique<Node>(Token(ttype, boolean));
    return result;
}
bool operator != (const Node &L, const Node &R);

bool operator == (const Node &L, const Node &R)
{
    if(L.children.size() != R.children.size())
        return false;
    for(unsigned long i = 0; i < L.children.size(); ++i){
        if (*L.children[i].get() != *R.children[i].get())
            return false;
    }
    return(
           L.token.getType() == R.token.getType() &&
           L.token.getText() == R.token.getText()
           );
}

bool operator != (const Node &L, const Node &R){
    return !(L == R);
}



BOOST_FIXTURE_TEST_SUITE(scanner_tests_suite, F)


    BOOST_AUTO_TEST_CASE(emptyString) {
        std::string str = "";
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(false), 0);
    }

    BOOST_AUTO_TEST_CASE(oneLetter) {
        std::string str = "a";
        testTokens.emplace_back(std::make_pair("TEXT", std::string("a")));
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(false), 1);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );

    }

    BOOST_AUTO_TEST_CASE(randomText) {
        std::string str = "this is random text\neven more @#$%^&\n*#$)(&^%$#$%^&*";
        testTokens.emplace_back(std::make_pair("TEXT", str));
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(false), 1);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );

    }

    BOOST_AUTO_TEST_CASE(randomTextWithKeywords) {
        std::string str = "Try this{ { } { \n} } \n{ }} or not \n for endwhile in";
        str+="for i in a.code = % != >= <= < > ! not and or ";
        testTokens.emplace_back(std::make_pair("TEXT", str));
        source.loadData(str);
        BOOST_CHECK_EQUAL( read(false), 1);
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
        read(false);
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
        read(false);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );
    }


    BOOST_AUTO_TEST_CASE(declarationsAndTypes) {
        std::string str = "{{int x = 10}}{{string a = fff}}{{bool z = false}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("TYPE", "int"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("ASSIGNOP", "="));
        testTokens.emplace_back(std::make_pair("NUMBER", 10));
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
        testTokens.emplace_back(std::make_pair<std::string, TokenValue>("BOOLVAL", TokenValue(false)));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));


        source.loadData(str);
        read(false);
        BOOST_TEST( resultTokens == testTokens, boost::test_tools::per_element() );
    }

    BOOST_AUTO_TEST_CASE(whileLoop) {
        std::string str = "{{while x > 0}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("WHILE", "while"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        testTokens.emplace_back(std::make_pair<std::string, TokenValue>("NUMBER", TokenValue(0)));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(false);
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
        read(false);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(ifElse) {
        std::string str = "{{if x > 0}}text1{{else}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("IF", "if"));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        testTokens.emplace_back(std::make_pair<std::string, TokenValue>("NUMBER", TokenValue(0)));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        testTokens.emplace_back(std::make_pair("TEXT", "text1"));
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("ELSE", "else"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(false);
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
        read(false);
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
        read(false);
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
        read(false);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(complexLogic1) {
        std::string str = "{{(x > 0 and (z % 2 == 4))}}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("ID", "x"));
        testTokens.emplace_back(std::make_pair("COMPOP", ">"));
        TokenValue tv(0);
        testTokens.emplace_back(std::pair<std::string, TokenValue>("NUMBER", tv));
        testTokens.emplace_back(std::make_pair("LOGICOP", "and"));
        testTokens.emplace_back(std::make_pair("OPBRACKET", "("));
        testTokens.emplace_back(std::make_pair("ID", "z"));
        testTokens.emplace_back(std::make_pair("MATHOP", "%"));
        testTokens.emplace_back(std::make_pair<std::string, int>("NUMBER", 2));
        testTokens.emplace_back(std::make_pair("COMPOP", "=="));
        testTokens.emplace_back(std::make_pair<std::string, int>("NUMBER", 4));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));
        testTokens.emplace_back(std::make_pair("CLBRACKET", ")"));

        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));
        source.loadData(str);
        read(false);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(escapeChars) {
        std::string str = "alamakota \\{\\{ for x in y\\}\\}";
        testTokens.emplace_back(std::make_pair("TEXT", "alamakota {{ for x in y}}"));
        source.loadData(str);
        read(false);
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
        read(false);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(templateText){
        std::string str = "{{ \"template_tex t\" }}";
        testTokens.emplace_back(std::make_pair("OPTEMPLATE", "{{"));
        testTokens.emplace_back(std::make_pair("TEMPLATE_TEXT", "template_tex t"));
        testTokens.emplace_back(std::make_pair("CLTEMPLATENONEW", "}}"));

        source.loadData(str);
        read(false);
        BOOST_TEST(resultTokens == testTokens, boost::test_tools::per_element());
    }


    BOOST_AUTO_TEST_CASE(randomToken){
        std::string str = "{#$%^&*(YUH{{]]}}GDUOIA intadd {}{}{}[][}{][]{()";
        source.loadData(str);
        read(false);
    }

BOOST_AUTO_TEST_SUITE_END()



BOOST_FIXTURE_TEST_SUITE(parse_tests_suite, P)


    BOOST_AUTO_TEST_CASE(parseTemplate) {
        std::string text = "{{ token }}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto id = make_node(TT::ID, "token");
        test->add(id);
        BOOST_CHECK(*test.get() == *result.get());
    }

    BOOST_AUTO_TEST_CASE(parseBadTemplate) {
        std::string text = "{{ a == ) ) )  }}";
        source.loadData(text);
        auto result = parser.root();

        BOOST_CHECK(result == nullptr);
    }


    BOOST_AUTO_TEST_CASE(parseNumberDefinition) {
        std::string text = "{{ int x = 1 }}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto type = make_node(TT::TYPE, "int");
        auto eq = make_node(TT::ASSIGNOP, "=");
        auto x = make_node(TT::ID, "x");
        auto b = make_node(TT::NUMBER, 1);
        eq->add(x);
        eq->add(b);
        type->add(eq);
        test->add(type);
        BOOST_CHECK(*test.get() == *result.get());
    }

    BOOST_AUTO_TEST_CASE(parseVariableDefinition){
        std::string text = "{{ bool x = y }}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto type = make_node(TT::TYPE, "bool");
        auto eq = make_node(TT::ASSIGNOP, "=");
        auto x = make_node(TT::ID, "x");
        auto y = make_node(TT::ID, "y");
        eq->add(x);
        eq->add(y);
        type->add(eq);
        test->add(type);
        BOOST_CHECK(*test.get() == *result.get());
    }

    BOOST_AUTO_TEST_CASE(parseForLoop){
        std::string text = "{{ for int x in range }} TTTEEXXTT {{endfor}}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto for_expr= make_node(TT::FOR, "for");
        auto type = make_node(TT::TYPE, "int");
        auto x = make_node(TT::ID, "x");
        auto range = make_node(TT::ID, "range");
        for_expr->add(type);
        for_expr->add(x);
        for_expr->add(range);

        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t = make_node(TT::TEXT, " TTTEEXXTT ");
        body->add(t);

        auto endfor = make_node(TT::ENDFOR, "endfor");

        for_expr->add(body);
        for_expr->add(endfor);

        test->add(for_expr);
        BOOST_CHECK(*test.get() == *result.get());

    }

     BOOST_AUTO_TEST_CASE(parseWhileLoop){
        std::string text = "{{ while x > 0 }} TTTEEXXTT {{endwhile}}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto while_expr= make_node(TT::WHILE, "while");
        auto gt = make_node(TT::COMPOP, ">");
        auto x = make_node(TT::ID, "x");
        auto zero = make_node(TT::NUMBER, 0);

        gt->add(x);
        gt->add(zero);
        while_expr->add(gt);

        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t = make_node(TT::TEXT, " TTTEEXXTT ");
        body->add(t);

        auto endwhile = make_node(TT::ENDWHILE, "endwhile");

        while_expr->add(body);
        while_expr->add(endwhile);

        test->add(while_expr);

        BOOST_CHECK(*test.get() == *result.get());
    }

    BOOST_AUTO_TEST_CASE(parseIf){
        std::string text = "{{ if x == true }} TTTEEXXTT {{endif}}";
        source.loadData(text);
        auto result = parser.root();


        auto test = make_node(TT::START, "");
        auto if_expr= make_node(TT::IF, "if");
        auto cond = make_node(TT::COMPOP, "==");
        auto x = make_node(TT::ID, "x");
        auto tr= make_node(TT::BOOLVAL, true);
        cond->add(x);
        cond->add(tr);
        if_expr->add(cond);
        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t = make_node(TT::TEXT, " TTTEEXXTT ");
        body->add(t);
        auto endif = make_node(TT::ENDIF, "endif");
        if_expr->add(body);
        if_expr->add(endif);

        test->add(if_expr);

        BOOST_CHECK(*test.get() == *result.get());

    }

    BOOST_AUTO_TEST_CASE(parseIfElse){
        std::string text = "{{ if x == true }} TTTEEXXTT {{else}} 222222 {{endelse}}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto if_expr= make_node(TT::IF, "if");
        auto cond = make_node(TT::COMPOP, "==");
        auto x = make_node(TT::ID, "x");
        auto tr= make_node(TT::BOOLVAL, true);
        cond->add(x);
        cond->add(tr);
        if_expr->add(cond);
        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t = make_node(TT::TEXT, " TTTEEXXTT ");
        body->add(t);
        auto el = make_node(TT::ELSE, "else");
        auto body2 = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t2 = make_node(TT::TEXT, " 222222 ");
        body2->add(t2);
        auto endif = make_node(TT::ENDELSE, "endelse");
        if_expr->add(body);
        el->add(body2);
        el->add(endif);
        if_expr->add(el);
        test->add(if_expr);

        BOOST_CHECK(*test.get() == *result.get());
    }


    BOOST_AUTO_TEST_CASE(parseTemplatesWithoutMultipleSegments){

        std::string text = "{{ if x == true }}{{if y > 0}} 1111 {{endif}}{{else}} 222222 {{endelse}}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto if_expr= make_node(TT::IF, "if");
        auto cond = make_node(TT::COMPOP, "==");
        auto x = make_node(TT::ID, "x");
        auto tr= make_node(TT::BOOLVAL, true);
        cond->add(x);
        cond->add(tr);
        if_expr->add(cond);
        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");

        auto Wif_expr= make_node(TT::IF, "if");
        auto Wcond = make_node(TT::COMPOP, ">");
        auto Wx = make_node(TT::ID, "y");
        auto Wtr= make_node(TT::NUMBER, 0);
        Wcond->add(Wx);
        Wcond->add(Wtr);
        Wif_expr->add(Wcond);
        auto Wbody = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto Wt = make_node(TT::TEXT, " 1111 ");
        Wbody->add(Wt);
        auto Wendif = make_node(TT::ENDIF, "endif");
        Wif_expr->add(Wbody);
        Wif_expr->add(Wendif);

        body->add(Wif_expr);

        auto el = make_node(TT::ELSE, "else");
        auto body2 = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t2 = make_node(TT::TEXT, " 222222 ");
        body2->add(t2);
        auto endif = make_node(TT::ENDELSE, "endelse");
        if_expr->add(body);
        el->add(body2);
        el->add(endif);
        if_expr->add(el);
        test->add(if_expr);

        BOOST_CHECK(*test.get() == *result.get());
    }


    BOOST_AUTO_TEST_CASE(parseTemplatesWithMulipleSegments){

        std::string text = "{{ if x == true }} text1 {{if y > 0}} text2 {{endif}} text3 {{else}} text4 {{endelse}}";
        source.loadData(text);
        auto result = parser.root();

        auto test = make_node(TT::START, "");
        auto if_expr= make_node(TT::IF, "if");
        auto cond = make_node(TT::COMPOP, "==");
        auto x = make_node(TT::ID, "x");
        auto tr= make_node(TT::BOOLVAL, true);
        cond->add(x);
        cond->add(tr);
        if_expr->add(cond);
        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");

        auto t1 = make_node(TT::TEXT, " text1 ");

        auto Wif_expr= make_node(TT::IF, "if");
        auto Wcond = make_node(TT::COMPOP, ">");
        auto Wx = make_node(TT::ID, "y");
        auto Wtr= make_node(TT::NUMBER, 0);
        Wcond->add(Wx);
        Wcond->add(Wtr);
        Wif_expr->add(Wcond);
        auto Wbody = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto Wt = make_node(TT::TEXT, " text2 ");
        Wbody->add(Wt);
        auto Wendif = make_node(TT::ENDIF, "endif");
        Wif_expr->add(Wbody);
        Wif_expr->add(Wendif);

        body->add(t1);
        body->add(Wif_expr);
        auto t3 = make_node(TT::TEXT, " text3 ");
        body->add(t3);

        auto el = make_node(TT::ELSE, "else");
        auto body2 = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t2 = make_node(TT::TEXT, " text4 ");
        body2->add(t2);
        auto endif = make_node(TT::ENDELSE, "endelse");
        if_expr->add(body);
        el->add(body2);
        el->add(endif);
        if_expr->add(el);
        test->add(if_expr);

        BOOST_CHECK(*test.get() == *result.get());

    }


     BOOST_AUTO_TEST_CASE(parseCompoundLogicExpression){

        std::string text = "{{ if (x == true) and ((y > 0) or ( not true)) }} text1 {{endif}}";
        source.loadData(text);
        auto result = parser.root();


        auto test = make_node(TT::START, "");
        auto if_expr= make_node(TT::IF, "if");
        auto and_expr = make_node(TT::LOGICOP, "and");
        auto cond = make_node(TT::COMPOP, "==");
        auto x = make_node(TT::ID, "x");
        auto tr= make_node(TT::BOOLVAL, true);
        cond->add(x);
        cond->add(tr);

        and_expr->add(cond);


        auto or_expr = make_node(TT::LOGICOP, "or");
        auto cond2 = make_node(TT::COMPOP, ">");
        auto y = make_node(TT::ID, "y");
        auto zero= make_node(TT::NUMBER, 0);
        cond2->add(y);
        cond2->add(zero);
        or_expr->add(cond2);
        auto not_expr = make_node(TT::LOGICOP, "not");
        auto v = make_node(TT::BOOLVAL, true);
        not_expr->add(v);
        or_expr->add(not_expr);
        and_expr->add(or_expr);

        if_expr->add(and_expr);

        auto body = make_node(TT::MULTIPLE_SEGMENTS, "");
        auto t = make_node(TT::TEXT, " text1 ");
        body->add(t);
        auto endif = make_node(TT::ENDIF, "endif");
        if_expr->add(body);
        if_expr->add(endif);

        test->add(if_expr);

        BOOST_CHECK(*test.get() == *result.get());
    }

BOOST_AUTO_TEST_SUITE_END()
