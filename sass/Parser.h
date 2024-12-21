#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t pos = 0;

    Token peek(int offset = 0) const;
    Token consume(TokenType type, const std::string& errMsg);
    bool match(TokenType type);
    Token advance();

    // Grammar rules (very simplified)
    std::unique_ptr<ASTNode> parseDataStep();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfThen();
    std::unique_ptr<ASTNode> parseOutput();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parsePrimary();
};

#endif // PARSER_H
