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
        case TokenType::KEYWORD_MERGE:
            return parseMerge(); // Handle MERGE statements
        case TokenType::KEYWORD_BY:
            return parseBy(); // Handle BY statements
        case TokenType::KEYWORD_DO:
            return parseDoLoop(); // Handle DO loops
        case TokenType::KEYWORD_DOLOOP:
            return parseDo();
        case TokenType::KEYWORD_ENDDO:
            return parseEndDo();
        case TokenType::IDENTIFIER:
            return parseAssignment();
        case TokenType::KEYWORD_IF:
            return parseIfElseIf(); // Updated to handle IF-ELSE IF
        case TokenType::KEYWORD_ELSE:
            throw std::runtime_error("Unexpected 'ELSE' without preceding 'IF'.");
        case TokenType::KEYWORD_ELSE_IF:
            throw std::runtime_error("Unexpected 'ELSE IF' without preceding 'IF'.");
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
        consume(TokenType::EQUAL, "Expected '=' after option name");
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
    consume(TokenType::EQUAL, "Expected '='");
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
    if (t.type == TokenType::NUMBER) {
        advance();
        return std::make_unique<NumberNode>(std::stod(t.text));
    }
    else if (t.type == TokenType::STRING) {
        advance();
        return std::make_unique<StringNode>(t.text);
    }
    else if (t.type == TokenType::IDENTIFIER) {
        // Check if it's a function call
        if (tokens.size() > pos + 1 && tokens[pos + 1].type == TokenType::LPAREN) {
            return parseFunctionCall();
        }
        // Check if it's an array element reference
        else if (tokens.size() > pos + 1 && tokens[pos + 1].type == TokenType::LBRACKET) {
            auto arrayElement = std::make_unique<ArrayElementNode>();
            arrayElement->arrayName = consume(TokenType::IDENTIFIER, "Expected array name").text;
            consume(TokenType::LBRACKET, "Expected '[' after array name");
            arrayElement->index = parseExpression();
            consume(TokenType::RBRACKET, "Expected ']' after array index");
            return arrayElement;
        }
        else {
            advance();
            return std::make_unique<VariableNode>(t.text);
        }
    }
    else if (t.type == TokenType::LPAREN) {
        advance(); // Consume '('
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    else {
        std::ostringstream oss;
        oss << "Unexpected token: " << t.text << " at line " << t.line << ", column " << t.col;
        throw std::runtime_error(oss.str());
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

    Token t = peek();
    if (t.type == TokenType::KEYWORD_SORT) {
        return parseProcSort();
    }
    else if (t.type == TokenType::KEYWORD_MEANS) {
        return parseProcMeans();
    }
    else {
        node->procName = consume(TokenType::IDENTIFIER, "Expected PROC name").text;
        consume(TokenType::SEMICOLON, "Expected ';' after PROC name");

        // For simplicity, handle only 'print' PROC with optional DATA= option
        if (node->procName == "print") {
            // Expect 'data=<dataset>'
            if (peek().type == TokenType::IDENTIFIER && peek().text == "data") {
                advance(); // Consume 'data'
                consume(TokenType::EQUAL, "Expected '=' after 'data'");
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
        else {
            // Unsupported PROC
            throw std::runtime_error("Unsupported PROC: " + node->procName);
        }
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
    // array <arrayName>{<size>} <var1> <var2> ...;
    auto arrayNode = std::make_unique<ArrayNode>();
    consume(TokenType::KEYWORD_ARRAY, "Expected 'array' keyword");

    // Parse array name
    arrayNode->arrayName = consume(TokenType::IDENTIFIER, "Expected array name").text;

    // Parse size: {<size>}
    consume(TokenType::LBRACE, "Expected '{' before array size");
    Token sizeToken = consume(TokenType::NUMBER, "Expected numeric array size");
    arrayNode->size = static_cast<int>(std::stod(sizeToken.text));
    consume(TokenType::RBRACE, "Expected '}' after array size");

    // Parse variable list
    while (peek().type == TokenType::IDENTIFIER) {
        arrayNode->variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name in array").text);
    }

    // Expect semicolon
    consume(TokenType::SEMICOLON, "Expected ';' after array declaration");

    return arrayNode;
}


std::unique_ptr<ASTNode> Parser::parseDo() {
    // do <variable> = <start> to <end> by <increment>;
    auto node = std::make_unique<DoNode>();
    consume(TokenType::KEYWORD_DO, "Expected 'do'");
    node->loopVar = consume(TokenType::IDENTIFIER, "Expected loop variable").text;
    consume(TokenType::EQUAL, "Expected '=' in DO statement");
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

std::unique_ptr<ASTNode> Parser::parseIfElse() {
    // if <condition> then <statements> else <statements>;
    auto node = std::make_unique<IfElseNode>();
    consume(TokenType::KEYWORD_IF, "Expected 'if'");
    node->condition = parseExpression();
    consume(TokenType::KEYWORD_THEN, "Expected 'then' after condition");

    // Parse 'then' statements
    // For simplicity, assume a single statement; can be extended to handle blocks
    auto stmt = parseStatement();
    if (stmt) node->thenStatements.push_back(std::move(stmt));

    // Check for 'else'
    if (peek().type == TokenType::KEYWORD_ELSE) {
        consume(TokenType::KEYWORD_ELSE, "Expected 'else'");
        auto elseStmt = parseStatement();
        if (elseStmt) node->elseStatements.push_back(std::move(elseStmt));
    }

    return node;
}


std::unique_ptr<ASTNode> Parser::parseIfElseIf() {
    // Parse the primary IF condition
    auto node = std::make_unique<IfElseIfNode>();
    consume(TokenType::KEYWORD_IF, "Expected 'if'");
    node->condition = parseExpression();
    consume(TokenType::KEYWORD_THEN, "Expected 'then' after condition");

    // Parse 'then' statements
    // For simplicity, assume a single statement; can be extended to handle blocks
    auto stmt = parseStatement();
    if (stmt) node->thenStatements.push_back(std::move(stmt));

    // Handle multiple 'ELSE IF' branches
    while (peek().type == TokenType::KEYWORD_ELSE_IF) {
        consume(TokenType::KEYWORD_ELSE_IF, "Expected 'else if'");
        std::unique_ptr<ASTNode> elseIfCondition = parseExpression();
        consume(TokenType::KEYWORD_THEN, "Expected 'then' after 'else if' condition");

        std::vector<std::unique_ptr<ASTNode>> elseIfStmts;
        auto elseIfStmt = parseStatement();
        if (elseIfStmt) elseIfStmts.push_back(std::move(elseIfStmt));

        node->elseIfBranches.emplace_back(std::move(elseIfCondition), std::move(elseIfStmts));
    }

    // Handle 'ELSE' branch
    if (peek().type == TokenType::KEYWORD_ELSE) {
        consume(TokenType::KEYWORD_ELSE, "Expected 'else'");
        auto elseStmt = parseStatement();
        if (elseStmt) node->elseStatements.push_back(std::move(elseStmt));
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    consume(TokenType::KEYWORD_DO, "Expected 'do' to start a block");
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (peek().type != TokenType::KEYWORD_ENDDO && peek().type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt) statements.push_back(std::move(stmt));
    }
    consume(TokenType::KEYWORD_ENDDO, "Expected 'enddo' to close the block");
    consume(TokenType::SEMICOLON, "Expected ';' after 'enddo'");

    // Create a block node or return the list of statements
    // For simplicity, return a BlockNode if defined, or handle appropriately
    // Assuming BlockNode is defined:
    auto block = std::make_unique<BlockNode>();
    block->statements = std::move(statements);
    return block;
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall() {
    // Function call: function_name(arg1, arg2, ...)
    auto funcCall = std::make_unique<FunctionCallNode>();
    funcCall->functionName = consume(TokenType::IDENTIFIER, "Expected function name").text;
    consume(TokenType::LPAREN, "Expected '(' after function name");

    // Parse arguments
    if (peek().type != TokenType::RPAREN) { // Handle functions with no arguments
        while (true) {
            auto arg = parseExpression();
            funcCall->arguments.push_back(std::move(arg));
            if (peek().type == TokenType::COMMA) {
                consume(TokenType::COMMA, "Expected ',' between function arguments");
            }
            else {
                break;
            }
        }
    }

    consume(TokenType::RPAREN, "Expected ')' after function arguments");
    return funcCall;
}

std::unique_ptr<ASTNode> Parser::parseMerge() {
    // MERGE dataset1 dataset2 ...;
    auto mergeNode = std::make_unique<MergeStatementNode>();
    consume(TokenType::KEYWORD_MERGE, "Expected 'MERGE' keyword");

    // Parse dataset names
    while (peek().type == TokenType::IDENTIFIER) {
        mergeNode->datasets.push_back(consume(TokenType::IDENTIFIER, "Expected dataset name in MERGE statement").text);
    }

    // Expect semicolon
    consume(TokenType::SEMICOLON, "Expected ';' after MERGE statement");

    return mergeNode;
}

std::unique_ptr<ASTNode> Parser::parseBy() {
    // BY var1 var2 ...;
    auto byNode = std::make_unique<ByStatementNode>();
    consume(TokenType::KEYWORD_BY, "Expected 'BY' keyword");

    // Parse variable names
    while (peek().type == TokenType::IDENTIFIER) {
        byNode->variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name in BY statement").text);
    }

    // Expect semicolon
    consume(TokenType::SEMICOLON, "Expected ';' after BY statement");

    return byNode;
}

std::unique_ptr<ASTNode> Parser::parseDoLoop() {
    // DO [WHILE(condition)] [UNTIL(condition)];
    auto doLoopNode = std::make_unique<DoLoopNode>();
    consume(TokenType::KEYWORD_DO, "Expected 'DO' keyword");

    // Check for optional WHILE or UNTIL
    if (match(TokenType::KEYWORD_WHILE)) {
        doLoopNode->isWhile = true;
        consume(TokenType::KEYWORD_WHILE, "Expected 'WHILE' after 'DO'");
        consume(TokenType::LPAREN, "Expected '(' after 'WHILE'");
        doLoopNode->condition = parseExpression(); // Parse condition expression
        consume(TokenType::RPAREN, "Expected ')' after 'WHILE' condition");
    }
    else if (match(TokenType::KEYWORD_UNTIL)) {
        doLoopNode->isWhile = false;
        consume(TokenType::KEYWORD_UNTIL, "Expected 'UNTIL' after 'DO'");
        consume(TokenType::LPAREN, "Expected '(' after 'UNTIL'");
        doLoopNode->condition = parseExpression(); // Parse condition expression
        consume(TokenType::RPAREN, "Expected ')' after 'UNTIL' condition");
    }
    else {
        doLoopNode->isWhile = true; // Default to DO WHILE if no condition specified
        doLoopNode->condition = nullptr;
    }

    // Parse the body of the DO loop (a block of statements)
    doLoopNode->body = std::make_unique<BlockNode>();

    while (!match(TokenType::KEYWORD_END) && pos < tokens.size()) {
        doLoopNode->body->statements.push_back(parseStatement());
    }

    consume(TokenType::KEYWORD_END, "Expected 'END' to close 'DO' loop");
    consume(TokenType::SEMICOLON, "Expected ';' after 'END'");

    return doLoopNode;
}

std::unique_ptr<ASTNode> Parser::parseProcSort() {
    auto procSortNode = std::make_unique<ProcSortNode>();
    consume(TokenType::KEYWORD_SORT, "Expected 'SORT' keyword after 'PROC'");

    // Parse DATA= option
    if (match(TokenType::KEYWORD_DATA)) {
        consume(TokenType::KEYWORD_DATA, "Expected 'DATA=' option in PROC SORT");
        Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after 'DATA='");
        procSortNode->inputDataSet = dataToken.text;
    }
    else {
        throw std::runtime_error("PROC SORT requires a DATA= option");
    }

    // Parse OUT= option (optional)
    if (match(TokenType::KEYWORD_OUT)) {
        consume(TokenType::KEYWORD_OUT, "Expected 'OUT=' option in PROC SORT");
        Token outToken = consume(TokenType::IDENTIFIER, "Expected dataset name after 'OUT='");
        procSortNode->outputDataSet = outToken.text;
    }

    // Parse BY statement
    if (match(TokenType::KEYWORD_BY)) {
        consume(TokenType::KEYWORD_BY, "Expected 'BY' keyword in PROC SORT");
        while (peek().type == TokenType::IDENTIFIER) {
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in BY statement");
            procSortNode->byVariables.push_back(varToken.text);
        }
    }
    else {
        throw std::runtime_error("PROC SORT requires a BY statement");
    }

    // Parse optional WHERE statement
    if (match(TokenType::KEYWORD_WHERE)) {
        consume(TokenType::KEYWORD_WHERE, "Expected 'WHERE' keyword in PROC SORT");
        consume(TokenType::LPAREN, "Expected '(' after 'WHERE'");
        procSortNode->whereCondition = parseExpression(); // Parse condition expression
        consume(TokenType::RPAREN, "Expected ')' after 'WHERE' condition");
    }

    // Parse optional NODUPKEY and DUPLICATES options
    while (match(TokenType::KEYWORD_NODUPKEY) || match(TokenType::KEYWORD_DUPLICATES)) {
        if (match(TokenType::KEYWORD_NODUPKEY)) {
            consume(TokenType::KEYWORD_NODUPKEY, "Expected 'NODUPKEY' keyword");
            procSortNode->nodupkey = true;
        }
        if (match(TokenType::KEYWORD_DUPLICATES)) {
            consume(TokenType::KEYWORD_DUPLICATES, "Expected 'DUPLICATES' keyword");
            procSortNode->duplicates = true;
        }
    }

    // Expect RUN; statement
    consume(TokenType::KEYWORD_RUN, "Expected 'RUN;' to terminate PROC SORT");
    consume(TokenType::SEMICOLON, "Expected ';' after 'RUN'");

    return procSortNode;
}

std::unique_ptr<ASTNode> Parser::parseProcMeans() {
    auto procMeansNode = std::make_unique<ProcMeansNode>();
    consume(TokenType::KEYWORD_MEANS, "Expected 'MEANS' keyword after 'PROC'");

    // Parse DATA= option
    if (match(TokenType::KEYWORD_DATA)) {
        consume(TokenType::KEYWORD_DATA, "Expected 'DATA=' option in PROC MEANS");
        Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after 'DATA='");
        procMeansNode->inputDataSet = dataToken.text;
    }
    else {
        throw std::runtime_error("PROC MEANS requires a DATA= option");
    }

    // Parse statistical options (N, MEAN, MEDIAN, STD, MIN, MAX)
    while (match(TokenType::KEYWORD_N) ||
        match(TokenType::KEYWORD_MEAN) ||
        match(TokenType::KEYWORD_MEDIAN) ||
        match(TokenType::KEYWORD_STD) ||
        match(TokenType::KEYWORD_MIN) ||
        match(TokenType::KEYWORD_MAX)) {
        Token statToken = advance();
        switch (statToken.type) {
        case TokenType::KEYWORD_N:
            procMeansNode->statistics.push_back("N");
            break;
        case TokenType::KEYWORD_MEAN:
            procMeansNode->statistics.push_back("MEAN");
            break;
        case TokenType::KEYWORD_MEDIAN:
            procMeansNode->statistics.push_back("MEDIAN");
            break;
        case TokenType::KEYWORD_STD:
            procMeansNode->statistics.push_back("STD");
            break;
        case TokenType::KEYWORD_MIN:
            procMeansNode->statistics.push_back("MIN");
            break;
        case TokenType::KEYWORD_MAX:
            procMeansNode->statistics.push_back("MAX");
            break;
        default:
            break;
        }
    }

    // Parse VAR statement
    if (match(TokenType::KEYWORD_VAR)) {
        consume(TokenType::KEYWORD_VAR, "Expected 'VAR' keyword in PROC MEANS");
        while (peek().type == TokenType::IDENTIFIER) {
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in VAR statement");
            procMeansNode->varVariables.push_back(varToken.text);
        }
    }
    else {
        throw std::runtime_error("PROC MEANS requires a VAR statement");
    }

    // Parse optional OUTPUT statement
    if (match(TokenType::KEYWORD_OUTPUT)) {
        consume(TokenType::KEYWORD_OUTPUT, "Expected 'OUTPUT' keyword in PROC MEANS");
        if (match(TokenType::KEYWORD_OUT)) {
            consume(TokenType::KEYWORD_OUT, "Expected 'OUT=' option in OUTPUT statement");
            Token outToken = consume(TokenType::IDENTIFIER, "Expected dataset name after 'OUT='");
            procMeansNode->outputDataSet = outToken.text;
        }

        // Parse output options like N=, MEAN=, etc.
        while (match(TokenType::IDENTIFIER)) {
            Token optionToken = consume(TokenType::IDENTIFIER, "Expected output option in OUTPUT statement");
            if (match(TokenType::EQUAL)) {
                consume(TokenType::EQUAL, "Expected '=' after output option");
                Token valueToken = consume(TokenType::IDENTIFIER, "Expected value after '=' in output option");
                procMeansNode->outputOptions[optionToken.text] = valueToken.text;
            }
            else {
                throw std::runtime_error("Expected '=' after output option in OUTPUT statement");
            }
        }
    }

    // Expect RUN; statement
    consume(TokenType::KEYWORD_RUN, "Expected 'RUN;' to terminate PROC MEANS");
    consume(TokenType::SEMICOLON, "Expected ';' after 'RUN'");

    return procMeansNode;
}
