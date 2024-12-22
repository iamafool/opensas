#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token> &tokens);
    std::unique_ptr<ASTNode> parse();
    std::unique_ptr<ProgramNode> parseProgram(); // To handle multiple global and data statements

private:
    const std::vector<Token> &tokens;
    size_t pos = 0;

    Token peek(int offset=0) const;
    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string &errMsg);

    // Grammar rules
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseDataStep();
    std::unique_ptr<ASTNode> parseOptions();
    std::unique_ptr<ASTNode> parseLibname();
    std::unique_ptr<ASTNode> parseTitle();
    std::unique_ptr<ASTNode> parseProc();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfElseIf(); // Updated to handle IF-ELSE IF
    std::unique_ptr<ASTNode> parseIfThen();
    std::unique_ptr<ASTNode> parseIfElse();
    std::unique_ptr<ASTNode> parseOutput();
    std::unique_ptr<ASTNode> parseDrop();
    std::unique_ptr<ASTNode> parseKeep();
    std::unique_ptr<ASTNode> parseRetain();
    std::unique_ptr<ASTNode> parseArray();
    std::unique_ptr<ASTNode> parseDo();
    std::unique_ptr<ASTNode> parseEndDo();
    std::unique_ptr<ASTNode> parseBlock(); // New method for parsing blocks

    // Expression parsing with precedence
    std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    int getPrecedence(const std::string& op) const;
};

#endif // PARSER_H
