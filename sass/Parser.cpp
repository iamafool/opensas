#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

Parser::Parser(const std::vector<Token> &t) : tokens(t) {}

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

Token Parser::consume(TokenType type, const std::string &errMsg) {
    if (peek().type == type) return advance();
    std::ostringstream oss;
    oss << "Parser error at line " << peek().line << ", column " << peek().col << ": " << errMsg;
    throw std::runtime_error(oss.str());
}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (peek().type != TokenType::EOF_TOKEN) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        }
        catch (const std::runtime_error &e) {
            // Handle parse error, possibly log it and skip to next statement
            std::cerr << "Parse error: " << e.what() << "\n";
            // Implement error recovery if desired
            // For simplicity, break here
            break;
        }
    }
    return program;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseProgram() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (peek().type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        } else {
            // Unknown or unsupported statement, skip or handle error
            throw std::runtime_error("Unsupported or invalid statement.");
        }
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    try {
        Token t = peek();
        switch (t.type) {
        case TokenType::KEYWORD_DATA:
            return parseDataStep();
        case TokenType::KEYWORD_OPTIONS:
            return parseOptions();
        case TokenType::KEYWORD_LIBNAME:
            return parseLibname();
        case TokenType::KEYWORD_TITLE:
            return parseTitle();
        case TokenType::IDENTIFIER:
            return parseAssignment();
        case TokenType::KEYWORD_IF:
            return parseIfThen();
        case TokenType::KEYWORD_OUTPUT:
            return parseOutput();
        default:
            throw std::runtime_error("Unknown statement starting with token: " + t.text);
        }
    }
    catch (const std::runtime_error& e) {
        // Log the error
        // logLogger->error("Parser error: {}", e.what());

        // Attempt to skip to the next semicolon to recover
        while (peek().type != TokenType::SEMICOLON && peek().type != TokenType::EOF_TOKEN) {
            advance();
        }

        // Skip the semicolon
        if (peek().type == TokenType::SEMICOLON) {
            advance();
        }

        // Return nullptr or a special ErrorNode if implemented
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseDataStep() {
    // data <dataset>; set <source>; ... run;
    auto node = std::make_unique<DataStepNode>();
    consume(TokenType::KEYWORD_DATA, "Expected 'data'");
    node->outputDataSet = consume(TokenType::IDENTIFIER, "Expected dataset name").text;
    consume(TokenType::SEMICOLON, "Expected ';' after dataset name");

    consume(TokenType::KEYWORD_SET, "Expected 'set'");
    node->inputDataSet = consume(TokenType::IDENTIFIER, "Expected input dataset name").text;
    consume(TokenType::SEMICOLON, "Expected ';' after input dataset name");

    // Parse statements until 'run;'
    while (!(peek().type == TokenType::KEYWORD_RUN)) {
        auto stmt = parseStatement();
        if (stmt) node->statements.push_back(std::move(stmt));
        else break; // Or handle differently
    }
    consume(TokenType::KEYWORD_RUN, "Expected 'run'");
    consume(TokenType::SEMICOLON, "Expected ';' after 'run'");

    return node;
}

std::unique_ptr<ASTNode> Parser::parseOptions() {
    // options option1=value1 option2=value2;
    auto node = std::make_unique<OptionsNode>();
    consume(TokenType::KEYWORD_OPTIONS, "Expected 'options'");

    while (peek().type != TokenType::SEMICOLON && peek().type != TokenType::EOF_TOKEN) {
        // Parse option name
        std::string optionName = consume(TokenType::IDENTIFIER, "Expected option name").text;
        // Expect '='
        consume(TokenType::EQUALS, "Expected '=' after option name");
        // Parse option value, could be string or number
        std::string optionValue;
        if (peek().type == TokenType::STRING) {
            optionValue = consume(TokenType::STRING, "Expected string value for option").text;
        } else if (peek().type == TokenType::NUMBER || peek().type == TokenType::IDENTIFIER) {
            optionValue = consume(peek().type, "Expected value for option").text;
        } else {
            throw std::runtime_error("Invalid option value for option: " + optionName);
        }
        node->options.emplace_back(optionName, optionValue);
    }

    consume(TokenType::SEMICOLON, "Expected ';' after options statement");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseLibname() {
    // libname libref 'path';
    auto node = std::make_unique<LibnameNode>();
    consume(TokenType::KEYWORD_LIBNAME, "Expected 'libname'");

    node->libref = consume(TokenType::IDENTIFIER, "Expected libref").text;
    // Optionally, there could be more options or syntax here

    // Expect the path, which is typically a string
    if (peek().type == TokenType::STRING) {
        node->path = consume(TokenType::STRING, "Expected path for libname").text;
    } else {
        throw std::runtime_error("Expected path string for libname");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after libname statement");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseTitle() {
    // title 'Your Title';
    auto node = std::make_unique<TitleNode>();
    consume(TokenType::KEYWORD_TITLE, "Expected 'title'");

    // Expect the title string
    if (peek().type == TokenType::STRING) {
        node->title = consume(TokenType::STRING, "Expected title string").text;
    } else {
        throw std::runtime_error("Expected string for title");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after title statement");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    // var = expr;
    auto node = std::make_unique<AssignmentNode>();
    node->varName = consume(TokenType::IDENTIFIER, "Expected variable name").text;
    consume(TokenType::EQUALS, "Expected '='");
    node->expression = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseIfThen() {
    // if expr then <statements>;
    auto node = std::make_unique<IfThenNode>();
    consume(TokenType::KEYWORD_IF, "Expected 'if'");
    node->condition = parseExpression();
    consume(TokenType::KEYWORD_THEN, "Expected 'then'");

    // Parse a single statement after then (for simplicity)
    auto stmt = parseStatement();
    if (stmt) node->thenStatements.push_back(std::move(stmt));

    return node;
}

std::unique_ptr<ASTNode> Parser::parseOutput() {
    // output;
    auto node = std::make_unique<OutputNode>();
    consume(TokenType::KEYWORD_OUTPUT, "Expected 'output'");
    consume(TokenType::SEMICOLON, "Expected ';' after 'output'");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    // For simplicity, implement a basic expression parser (e.g., only variables and literals, no precedence)
    // Extend this to handle full SAS expressions, operators, functions, etc.
    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    Token t = peek();
    if (t.type == TokenType::NUMBER || t.type == TokenType::STRING) {
        advance();
        auto node = std::make_unique<LiteralNode>();
        node->value = t.text;
        return node;
    } else if (t.type == TokenType::IDENTIFIER) {
        advance();
        auto node = std::make_unique<VariableNode>();
        node->varName = t.text;
        return node;
    } else {
        throw std::runtime_error("Invalid expression starting with token: " + t.text);
    }
}
