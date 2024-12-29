#include <gtest/gtest.h>
#include <memory>
#include "fixture.h"
#include "Lexer.h"

using namespace std;
using namespace sass;


// Test case for executing a simple DATA step and PROC PRINT
TEST_F(SassTest, TokenStringLiteral) {
    std::string code = R"(
    "She said, ""Yes."""
    )";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_GE(tokens.size(), 1) << "Expected 1 tokens.";

    // Example checks:
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].text, "She said, \"Yes.\"");
}