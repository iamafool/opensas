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

// Test case for executing a simple DATA step and PROC PRINT
TEST_F(SassTest, DataStepOutput1) {
    std::string code = R"(
        data a;
           a = 10;
           output;
        run;
    )";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // We can print or check token types. 
    // For a minimal example, let's check for a handful of expected tokens:
    //   KEYWORD_DATA, IDENTIFIER("a"), SEMICOLON, IDENTIFIER("a"), EQUAL, NUMBER("10"), ...
    ASSERT_GE(tokens.size(), 8u) << "Expected at least 8 tokens.";

    // Example checks:
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_DATA);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].text, "a");
    EXPECT_EQ(tokens[2].type, TokenType::SEMICOLON);

    // ... and so forth. Adjust indices based on your lexer's output.
    // Make sure we see 'output' and 'run'
    bool foundOutput = false;
    bool foundRun = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::KEYWORD_OUTPUT) foundOutput = true;
        if (t.type == TokenType::KEYWORD_RUN) foundRun = true;
    }
    EXPECT_TRUE(foundOutput);
    EXPECT_TRUE(foundRun);

    std::unique_ptr<ASTNode> ast;

    Parser parser(tokens);
    ParseResult parseResult;
    parseResult = parser.parseStatement();
    // Verify that the AST is not null and contains expected nodes
    ASSERT_EQ(parseResult.status, ParseStatus::PARSE_SUCCESS);

    interpreter->execute(parseResult.node.get());

    // Verify that the dataset 'a' exists with correct values
    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "a.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 1);
    EXPECT_EQ(sasdoc1.obs_count, 1);

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 10.0);
}

TEST_F(SassTest, DataStepInput1) {
    std::string code = R"(
        data employees;
          input name $ age;
          datalines;
john 23
mary 30
;
        run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseStatement();
    ASSERT_TRUE(parseResult.status == ParseStatus::PARSE_SUCCESS);

    // 3) Interpret
    interpreter->execute(parseResult.node.get());

    // 4) Check dataset WORK.employees => 2 obs, 2 vars
    // Verify that the dataset 'a' exists with correct values
    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "employees.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 2);
    EXPECT_EQ(sasdoc1.obs_count, 2);

    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[0]).get(), "john");
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 23.0);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[2]).get(), "mary");
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 30.0);
}