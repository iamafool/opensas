#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"
#include <vector>
#include <memory>

namespace sass {
    // An enum to indicate parse status
    enum class ParseStatus {
        PARSE_SUCCESS,
        PARSE_INCOMPLETE,
        PARSE_ERROR
    };

    // A parse result that can hold either a valid node, or indicate incomplete or error
    struct ParseResult {
        ParseStatus status;
        std::unique_ptr<ASTNode> node;
        std::string errorMessage;
    };

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);

        std::unique_ptr<ASTNode> parse();
        std::unique_ptr<ProgramNode> parseProgram(); // To handle multiple global and data statements
        ParseResult parseStatement();

    private:
        const std::vector<Token>& tokens;
        size_t pos = 0;

        Token peek(int offset = 0) const;
        Token advance();
        bool match(TokenType type);
        Token consume(TokenType type, const std::string& errMsg);

        // Grammar rules
        // Attempts to parse a single statement from the current token stream
        // Returns a ParseResult indicating success, incomplete, or error.
        std::unique_ptr<ASTNode> parseDataStep();
        std::unique_ptr<ASTNode> parseOptions();
        std::unique_ptr<ASTNode> parseLibname();
        std::unique_ptr<ASTNode> parseTitle();
        std::unique_ptr<ASTNode> parseProc();
        std::unique_ptr<ASTNode> parseAssignment();
        std::unique_ptr<ASTNode> parseIfElseIf();
        std::unique_ptr<ASTNode> parseIfThen();
        std::unique_ptr<ASTNode> parseIfElse();
        std::unique_ptr<ASTNode> parseOutput();
        std::unique_ptr<ASTNode> parseDrop();
        std::unique_ptr<ASTNode> parseKeep();
        std::unique_ptr<ASTNode> parseRetain();
        std::unique_ptr<ASTNode> parseArray();
        std::unique_ptr<ASTNode> parseDo();
        std::unique_ptr<ASTNode> parseEndDo();
        std::unique_ptr<ASTNode> parseBlock();
        std::unique_ptr<ASTNode> parseFunctionCall();
        std::unique_ptr<ASTNode> parseMerge();
        std::unique_ptr<ASTNode> parseBy();
        std::unique_ptr<ASTNode> parseDoLoop();
        std::unique_ptr<ASTNode> parseProcSort();
        std::unique_ptr<ASTNode> parseProcMeans();
        std::unique_ptr<ASTNode> parseProcFreq();
        std::unique_ptr<ASTNode> parseProcPrint();
        std::unique_ptr<ASTNode> parseProcSQL();
        std::unique_ptr<SQLStatementNode> parseSQLStatement();
        std::unique_ptr<ASTNode> parseLetStatement();
        std::unique_ptr<ASTNode> parseMacroDefinition();
        std::unique_ptr<ASTNode> parseMacroCall();

        // Expression parsing with precedence
        std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
        std::unique_ptr<ASTNode> parsePrimary();
        int getPrecedence(const std::string& op) const;

        // Helper to quickly return an INCOMPLETE result
        ParseResult incompleteResult() {
            return { ParseStatus::PARSE_INCOMPLETE, nullptr, "" };
        }
    };

}

#endif // PARSER_H
