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
TEST_F(SassTest, ProcPrintNoDataset) {
    std::string code = R"(
        data a;
           a = 10;
           output;
        run;
        proc print;
        run;
    )";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_GE(tokens.size(), 16) << "Expected 16 tokens.";

    EXPECT_EQ(tokens[11].type, TokenType::KEYWORD_PROC);
    EXPECT_EQ(tokens[12].type, TokenType::KEYWORD_PRINT);

    std::unique_ptr<ASTNode> ast;
    Parser parser(tokens);
    ParseResult parseResult;
    parseResult = parser.parseStatement();
    ASSERT_EQ(parseResult.status, ParseStatus::PARSE_SUCCESS);


    interpreter->execute(parseResult.node.get());
}
