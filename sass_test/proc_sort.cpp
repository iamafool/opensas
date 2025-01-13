#include <gtest/gtest.h>
#include <memory>
#include "fixture.h"
#include "Lexer.h"
#include "Parser.h"
#include <filesystem>
#include <boost/flyweight.hpp>

using namespace std;
using namespace sass;
namespace fs = std::filesystem;

using flyweight_string = boost::flyweight<std::string>;

TEST_F(SassTest, ProcSortBasic01) {
    std::string code = R"(
data in;
    input x num1 num2 num3;
    datalines;
3 15 20 25
1 5 10 15
2 10 15 20
;
run;

proc sort data=in;
    by x;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 2);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "in.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 4);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "num1");
    EXPECT_EQ(sasdoc1.var_names[2], "num2");
    EXPECT_EQ(sasdoc1.var_names[3], "num3");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 1);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 5);
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 2);
    EXPECT_EQ(std::get<double>(sasdoc1.values[5]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[6]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[7]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[8]), 3);
    EXPECT_EQ(std::get<double>(sasdoc1.values[9]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[10]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[11]), 25);
}

TEST_F(SassTest, ProcSortBasic02) {
    std::string code = R"(
data in;
    input x num1 num2 num3;
    datalines;
3 15 20 25
1 5 10 15
2 10 15 20
;
run;

proc sort data=in out=out;
    by x;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 2);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 4);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "num1");
    EXPECT_EQ(sasdoc1.var_names[2], "num2");
    EXPECT_EQ(sasdoc1.var_names[3], "num3");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 1);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 5);
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 2);
    EXPECT_EQ(std::get<double>(sasdoc1.values[5]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[6]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[7]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[8]), 3);
    EXPECT_EQ(std::get<double>(sasdoc1.values[9]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[10]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[11]), 25);
}

TEST_F(SassTest, ProcSortNodupkey01) {
    std::string code = R"(
data in;
    input x num1 num2 num3;
    datalines;
3 15 20 25
1 5 10 15
1 6 11 16
2 10 15 20
;
run;

proc sort data=in out=out nodupkey;
    by x;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 2);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 4);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "num1");
    EXPECT_EQ(sasdoc1.var_names[2], "num2");
    EXPECT_EQ(sasdoc1.var_names[3], "num3");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 1);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 5);
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 2);
    EXPECT_EQ(std::get<double>(sasdoc1.values[5]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[6]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[7]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[8]), 3);
    EXPECT_EQ(std::get<double>(sasdoc1.values[9]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[10]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[11]), 25);
}