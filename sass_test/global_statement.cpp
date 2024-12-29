#include <gtest/gtest.h>
#include <memory>
#include "fixture.h"
#include "Lexer.h"
#include "Parser.h"
#include <filesystem>

using namespace std;
using namespace sass;
namespace fs = std::filesystem;


// Test case for executing a simple DATA step and PROC PRINT
TEST_F(SassTest, GlobalLibname) {
    std::string code = R"(
        libname test "c:\workspace\c++\sass\test\data\";
    )";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_GE(tokens.size(), 4u) << "Expected at least 4 tokens.";

    // Example checks:
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_LIBNAME);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].text, "test");
    EXPECT_EQ(tokens[2].type, TokenType::STRING);
    EXPECT_EQ(tokens[3].type, TokenType::SEMICOLON);

    std::unique_ptr<ASTNode> ast;
    Parser parser(tokens);
    ParseResult parseResult;
    parseResult = parser.parseStatement();
    // Verify that the AST is not null and contains expected nodes
    ASSERT_EQ(parseResult.status, ParseStatus::PARSE_SUCCESS);

    interpreter->execute(parseResult.node.get());

    auto lib = env->getLibrary("TEST");
    bool hasTestLib = lib != nullptr;

    EXPECT_TRUE(hasTestLib);
    EXPECT_EQ(lib->getPath(), "c:\\workspace\\c++\\sass\\test\\data\\");
}