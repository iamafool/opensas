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

std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (peek().type != TokenType::EOF_TOKEN) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        }
        catch (const std::runtime_error& e) {
            // Handle parse error, possibly log it and skip to next statement
            std::cerr << "Parse error: " << e.what() << "\n";
            // Implement error recovery if desired
            // For simplicity, skip tokens until next semicolon
            while (peek().type != TokenType::SEMICOLON && peek().type != TokenType::EOF_TOKEN) {
                advance();
            }
            if (peek().type == TokenType::SEMICOLON) {
                advance(); // Skip semicolon
            }
        }
    }
    return program;
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
        case TokenType::KEYWORD_PROC:
            return parseProc();
        case TokenType::KEYWORD_DROP:
            return parseDrop();
        case TokenType::KEYWORD_KEEP:
            return parseKeep();
        case TokenType::KEYWORD_RETAIN:
            return parseRetain();
        case TokenType::KEYWORD_ARRAY:
            return parseArray();
        case TokenType::KEYWORD_DO:
            return parseDo();
        case TokenType::KEYWORD_ENDDO:
            return parseEndDo();
        case TokenType::IDENTIFIER:
            return parseAssignment();
        case TokenType::KEYWORD_IF:
            return parseIfThen();
        case TokenType::KEYWORD_OUTPUT:
            return parseOutput();
        default:
            // Handle unknown token or throw error
            throw std::runtime_error("Unknown statement starting with token: " + t.text);
        }
    }
    catch (const std::runtime_error& e) {
        // Log the error and attempt to recover by skipping tokens until next semicolon
        // logLogger->error("Parser error: {}", e.what());

        while (peek().type != TokenType::SEMICOLON && peek().type != TokenType::EOF_TOKEN) {
            advance();
        }

        if (peek().type == TokenType::SEMICOLON) {
            advance(); // Skip semicolon
        }

        return nullptr; // Return nullptr to indicate skipped statement
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
    while (peek().type != TokenType::KEYWORD_RUN && peek().type != TokenType::EOF_TOKEN) {
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
        }
        else if (peek().type == TokenType::NUMBER || peek().type == TokenType::IDENTIFIER) {
            optionValue = consume(peek().type, "Expected value for option").text;
        }
        else {
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
    }
    else {
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
    }
    else {
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

std::unique_ptr<ASTNode> Parser::parseExpression(int precedence) {
    auto left = parsePrimary();

    while (true) {
        Token t = peek();
        std::string op = t.text;
        int currentPrecedence = getPrecedence(op);
        if (currentPrecedence < precedence) break;

        // Handle right-associative operators if any (e.g., exponentiation)
        bool rightAssociative = false; // Adjust as needed

        // Consume the operator
        advance();

        // Determine next precedence
        int nextPrecedence = rightAssociative ? currentPrecedence : currentPrecedence + 1;

        auto right = parseExpression(nextPrecedence);

        // Create BinaryOpNode
        auto binOp = std::make_unique<BinaryOpNode>();
        binOp->left = std::move(left);
        binOp->right = std::move(right);
        binOp->op = op;
        left = std::move(binOp);
    }

    return left;
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
        // Check if it's a function call
        Token next = peek(1);
        if (next.type == TokenType::LPAREN) {
            // Function call
            std::string funcName = t.text;
            advance(); // Consume function name
            consume(TokenType::LPAREN, "Expected '(' after function name");
            auto arg = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after function argument");
            auto funcCall = std::make_unique<FunctionCallNode>();
            funcCall->funcName = funcName;
            funcCall->argument = std::move(arg);
            return funcCall;
        }
        else {
            // Variable reference
            advance();
            auto node = std::make_unique<VariableNode>();
            node->varName = t.text;
            return node;
        }
    }
    else if (t.type == TokenType::LPAREN) {
        advance(); // Consume '('
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    else {
        throw std::runtime_error("Invalid expression starting with token: " + t.text);
    }
}

int Parser::getPrecedence(const std::string& op) const {
    if (op == "or") return 1;
    if (op == "and") return 2;
    if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=") return 3;
    if (op == "+" || op == "-") return 4;
    if (op == "*" || op == "/") return 5;
    if (op == "**") return 6;
    return 0;
}

std::unique_ptr<ASTNode> Parser::parseProc() {
    // proc <procName>; <procStatements>; run;
    auto node = std::make_unique<ProcNode>();
    consume(TokenType::KEYWORD_PROC, "Expected 'proc'");
    node->procName = consume(TokenType::IDENTIFIER, "Expected PROC name").text;
    consume(TokenType::SEMICOLON, "Expected ';' after PROC name");

    // For simplicity, handle only 'print' PROC with optional DATA= option
    if (node->procName == "print") {
        // Expect 'data=<dataset>'
        if (peek().type == TokenType::IDENTIFIER && peek().text == "data") {
            advance(); // Consume 'data'
            consume(TokenType::EQUALS, "Expected '=' after 'data'");
            node->datasetName = consume(TokenType::IDENTIFIER, "Expected dataset name").text;
            consume(TokenType::SEMICOLON, "Expected ';' after dataset name");
        }

        // Parse PROC-specific statements until 'run;'
        while (peek().type != TokenType::KEYWORD_RUN && peek().type != TokenType::EOF_TOKEN) {
            // Implement PROC-specific parsing as needed
            // For simplicity, skip or throw error
            throw std::runtime_error("Unsupported PROC statement: " + peek().text);
        }

        consume(TokenType::KEYWORD_RUN, "Expected 'run'");
        consume(TokenType::SEMICOLON, "Expected ';' after 'run'");
    }
    else if (node->procName == "sort") {
        // Handle PROC SORT
        auto sortNode = std::make_unique<ProcSortNode>();
        // Expect 'data=<dataset>'
        if (peek().type == TokenType::IDENTIFIER && peek().text == "data") {
            consume(TokenType::IDENTIFIER, "Expected 'data' in PROC SORT");
            consume(TokenType::EQUALS, "Expected '=' after 'data'");
            sortNode->datasetName = consume(TokenType::IDENTIFIER, "Expected dataset name").text;
        }
        else {
            throw std::runtime_error("Expected 'data=<dataset>' in PROC SORT");
        }

        // Expect 'by var1 var2 ...;'
        consume(TokenType::KEYWORD_BY, "Expected 'by' in PROC SORT");
        while (peek().type == TokenType::IDENTIFIER) {
            sortNode->byVariables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name in 'by' statement").text);
        }
        consume(TokenType::SEMICOLON, "Expected ';' after 'by' statement");

        // Parse until 'run;'
        while (peek().type != TokenType::KEYWORD_RUN && peek().type != TokenType::EOF_TOKEN) {
            // Currently, PROC SORT doesn't support additional statements
            // Throw error if unexpected statements are found
            throw std::runtime_error("Unsupported statement in PROC SORT");
        }

        consume(TokenType::KEYWORD_RUN, "Expected 'run'");
        consume(TokenType::SEMICOLON, "Expected ';' after 'run'");

        // Convert ProcSortNode to ProcNode for consistency
        auto finalSortNode = std::make_unique<ProcSortNode>();
        finalSortNode->datasetName = sortNode->datasetName;
        finalSortNode->byVariables = sortNode->byVariables;
        return finalSortNode;
    }
    else if (node->procName == "means") {
        // Handle PROC MEANS
        auto meansNode = std::make_unique<ProcMeansNode>();
        // Expect 'data=<dataset>'
        if (peek().type == TokenType::IDENTIFIER && peek().text == "data") {
            consume(TokenType::IDENTIFIER, "Expected 'data' in PROC MEANS");
            consume(TokenType::EQUALS, "Expected '=' after 'data'");
            meansNode->datasetName = consume(TokenType::IDENTIFIER, "Expected dataset name").text;
        }
        else {
            throw std::runtime_error("Expected 'data=<dataset>' in PROC MEANS");
        }

        // Expect 'var var1 var2 ...;'
        consume(TokenType::KEYWORD_VAR, "Expected 'var' in PROC MEANS");
        while (peek().type == TokenType::IDENTIFIER) {
            meansNode->varNames.push_back(consume(TokenType::IDENTIFIER, "Expected variable name in 'var' statement").text);
        }
        consume(TokenType::SEMICOLON, "Expected ';' after 'var' statement");

        // Parse until 'run;'
        while (peek().type != TokenType::KEYWORD_RUN && peek().type != TokenType::EOF_TOKEN) {
            // Currently, PROC MEANS doesn't support additional statements
            // Throw error if unexpected statements are found
            throw std::runtime_error("Unsupported statement in PROC MEANS");
        }

        consume(TokenType::KEYWORD_RUN, "Expected 'run'");
        consume(TokenType::SEMICOLON, "Expected ';' after 'run'");

        // Convert ProcMeansNode to ProcNode for consistency
        auto finalMeansNode = std::make_unique<ProcMeansNode>();
        finalMeansNode->datasetName = meansNode->datasetName;
        finalMeansNode->varNames = meansNode->varNames;
        return finalMeansNode;
    }
    else {
        // Unsupported PROC
        throw std::runtime_error("Unsupported PROC: " + node->procName);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseDrop() {
    // drop var1 var2 ...;
    auto node = std::make_unique<DropNode>();
    consume(TokenType::KEYWORD_DROP, "Expected 'drop'");
    while (peek().type == TokenType::IDENTIFIER) {
        node->variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").text);
    }
    consume(TokenType::SEMICOLON, "Expected ';' after drop statement");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseKeep() {
    // keep var1 var2 ...;
    auto node = std::make_unique<KeepNode>();
    consume(TokenType::KEYWORD_KEEP, "Expected 'keep'");
    while (peek().type == TokenType::IDENTIFIER) {
        node->variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").text);
    }
    consume(TokenType::SEMICOLON, "Expected ';' after keep statement");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseRetain() {
    // retain var1 var2 ...;
    auto node = std::make_unique<RetainNode>();
    consume(TokenType::KEYWORD_RETAIN, "Expected 'retain'");
    while (peek().type == TokenType::IDENTIFIER) {
        node->variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").text);
    }
    consume(TokenType::SEMICOLON, "Expected ';' after retain statement");
    return node;
}


std::unique_ptr<ASTNode> Parser::parseArray() {
    // array arr {size} var1 var2 ...;
    auto node = std::make_unique<ArrayNode>();
    consume(TokenType::KEYWORD_ARRAY, "Expected 'array'");
    node->arrayName = consume(TokenType::IDENTIFIER, "Expected array name").text;
    consume(TokenType::LBRACE, "Expected '{' after array name");
    std::string sizeStr = consume(TokenType::NUMBER, "Expected array size").text;
    node->size = std::stoi(sizeStr);
    consume(TokenType::RBRACE, "Expected '}' after array size");
    while (peek().type == TokenType::IDENTIFIER) {
        node->variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name in array").text);
    }
    consume(TokenType::SEMICOLON, "Expected ';' after array statement");
    return node;
}


std::unique_ptr<ASTNode> Parser::parseDo() {
    // do <variable> = <start> to <end> by <increment>;
    auto node = std::make_unique<DoNode>();
    consume(TokenType::KEYWORD_DO, "Expected 'do'");
    node->loopVar = consume(TokenType::IDENTIFIER, "Expected loop variable").text;
    consume(TokenType::EQUALS, "Expected '=' in DO statement");
    node->startExpr = parseExpression();
    consume(TokenType::KEYWORD_TO, "Expected 'to' in DO statement");
    node->endExpr = parseExpression();

    // Optional: BY <increment>
    if (peek().type == TokenType::KEYWORD_BY) {
        consume(TokenType::KEYWORD_BY, "Expected 'by' in DO statement");
        node->incrementExpr = parseExpression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after DO statement");

    // Parse nested statements until 'enddo;'
    while (peek().type != TokenType::KEYWORD_ENDDO && peek().type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt) node->statements.push_back(std::move(stmt));
    }

    consume(TokenType::KEYWORD_ENDDO, "Expected 'enddo'");
    consume(TokenType::SEMICOLON, "Expected ';' after 'enddo'");

    return node;
}

std::unique_ptr<ASTNode> Parser::parseEndDo() {
    // Should be handled in parseDo()
    throw std::runtime_error("'enddo' should be handled within DO statement parsing.");
}