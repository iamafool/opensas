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
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 5.0}, {"num2", 10.0}, {"num3", 15.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 10.0}, {"num2", 15.0}, {"num3", 20.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 15.0}, {"num2", 20.0}, {"num3", 25.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
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
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 5.0}, {"num2", 10.0}, {"num3", 15.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 10.0}, {"num2", 15.0}, {"num3", 20.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 15.0}, {"num2", 20.0}, {"num3", 25.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
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
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 5.0}, {"num2", 10.0}, {"num3", 15.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 10.0}, {"num2", 15.0}, {"num3", 20.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 15.0}, {"num2", 20.0}, {"num3", 25.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
}
