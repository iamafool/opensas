#include "Parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& t) : tokens(t) {}

Token Parser::peek(int offset) const {
    if (pos + offset < tokens.size()) {
        return tokens[pos + offset];
    }
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

Token Parser::advance() {
    if (pos < tokens.size()) return tokens[pos++];
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& errMsg) {
    if (peek().type == type) return advance();
    throw std::runtime_error(errMsg);
}

std::unique_ptr<ASTNode> Parser::parse() {
    // For simplicity, assume we only have one data step in the input.
    return parseDataStep();
}

std::unique_ptr<ASTNode> Parser::parseDataStep() {
    // data <dataset>; set <dataset>; ... run;
    auto node = std::make_unique<DataStepNode>();
    consume(TokenType::KEYWORD_DATA, "Expected 'data'");
    node->outputDataSet = consume(TokenType::IDENTIFIER, "Expected dataset name").text;
    consume(TokenType::SEMICOLON, "Expected ';'");
    consume(TokenType::KEYWORD_SET, "Expected 'set'");
    node->inputDataSet = consume(TokenType::IDENTIFIER, "Expected input dataset name").text;
    consume(TokenType::SEMICOLON, "Expected ';'");

    // parse statements until 'run'
    while (!match(TokenType::KEYWORD_RUN)) {
        auto stmt = parseStatement();
        if (stmt) node->statements.push_back(std::move(stmt));
    }
    consume(TokenType::SEMICOLON, "Expected ';' after run");

    return node;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    // For simplicity, try a few known statements: assignment, if-then, output
    Token t = peek();
    if (t.type == TokenType::IDENTIFIER) {
        return parseAssignment();
    }
    else if (t.type == TokenType::KEYWORD_IF) {
        return parseIfThen();
    }
    else if (t.type == TokenType::KEYWORD_OUTPUT) {
        return parseOutput();
    }
    else {
        // skip unknown
        advance();
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    // var = expr;
    auto node = std::make_unique<AssignmentNode>();
    node->varName = consume(TokenType::IDENTIFIER, "Expected variable name").text;
    consume(TokenType::EQUALS, "Expected '='");
    node->expression = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';'");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseIfThen() {
    // if expr then <statements>;
    auto node = std::make_unique<IfThenNode>();
    consume(TokenType::KEYWORD_IF, "Expected 'if'");
    node->condition = parseExpression();
    consume(TokenType::KEYWORD_THEN, "Expected 'then'");
    // parse a single statement after then (for simplicity)
    auto stmt = parseStatement();
    if (stmt) node->thenStatements.push_back(std::move(stmt));
    return node;
}

std::unique_ptr<ASTNode> Parser::parseOutput() {
    auto node = std::make_unique<OutputNode>();
    consume(TokenType::KEYWORD_OUTPUT, "Expected 'output'");
    consume(TokenType::SEMICOLON, "Expected ';'");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    // Very simplified: just parse a single primary for now
    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    Token t = peek();
    if (t.type == TokenType::NUMBER || t.type == TokenType::STRING) {
        advance();
        auto node = std::make_unique<LiteralNode>();
        node->value = t.text;
        return node;
    }
    else if (t.type == TokenType::IDENTIFIER) {
        advance();
        auto node = std::make_unique<VariableNode>();
        node->varName = t.text;
        return node;
    }
    else {
        // For simplicity, return null if unexpected
        return nullptr;
    }
}
