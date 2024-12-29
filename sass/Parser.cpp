#include "Parser.h"
#include "utility.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace sass {
Parser::Parser(const std::vector<Token> &t) : tokens(t) {}

Token Parser::peek(int offset) const {
    if (pos + offset < tokens.size()) {
        return tokens[pos + offset];
    }

    // Return an EOF token if out of range
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
            if (stmt.status == ParseStatus::PARSE_SUCCESS) {
                program->statements.push_back(std::move(stmt.node));
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

ParseResult Parser::parseStatement() {
	Token t = peek();
	std::unique_ptr<ASTNode> astNode = nullptr;
	try {
		switch (t.type) {
        case TokenType::KEYWORD_INPUT:
            advance();
            astNode = parseInput(); break;
        case TokenType::KEYWORD_DATALINES:
            advance();
            consume(TokenType::SEMICOLON, "Expected ';' after datalines");
            astNode = parseDatalines(); break;
		case TokenType::KEYWORD_DATA:
			astNode = parseDataStep(); break;
		case TokenType::KEYWORD_OPTIONS:
			astNode = parseOptions(); break;
		case TokenType::KEYWORD_LIBNAME:
			astNode = parseLibname(); break;
		case TokenType::KEYWORD_TITLE:
			astNode = parseTitle(); break;
		case TokenType::KEYWORD_PROC:
			astNode = parseProc(); break;
		case TokenType::KEYWORD_DROP:
			astNode = parseDrop(); break;
		case TokenType::KEYWORD_KEEP:
			astNode = parseKeep(); break;
		case TokenType::KEYWORD_RETAIN:
			astNode = parseRetain(); break;
		case TokenType::KEYWORD_ARRAY:
			astNode = parseArray(); break;
		case TokenType::KEYWORD_MERGE:
			astNode = parseMerge(); break; // Handle MERGE statements
		case TokenType::KEYWORD_BY:
			astNode = parseBy(); break; // Handle BY statements
		case TokenType::KEYWORD_DO:
			astNode = parseDoLoop(); break; // Handle DO loops
		case TokenType::KEYWORD_DOLOOP:
			astNode = parseDo(); break;
		case TokenType::KEYWORD_ENDDO:
			astNode = parseEndDo(); break;
		case TokenType::IDENTIFIER:
			astNode = parseAssignment(); break;
		case TokenType::KEYWORD_IF:
			astNode = parseIfElseIf(); break; // Updated to handle IF-ELSE IF
		case TokenType::KEYWORD_ELSE:
			throw std::runtime_error("Unexpected 'ELSE' without preceding 'IF'.");
		case TokenType::KEYWORD_ELSE_IF:
			throw std::runtime_error("Unexpected 'ELSE IF' without preceding 'IF'.");
		case TokenType::KEYWORD_OUTPUT:
			astNode = parseOutput(); break;
		case TokenType::KEYWORD_MACRO_LET:
			astNode = parseLetStatement(); break;
		case TokenType::KEYWORD_MACRO_MACRO:
			astNode = parseMacroDefinition(); break;
		case TokenType::EOF_TOKEN: {
			// No tokens at all -> incomplete or just end?
			// If the user typed nothing, we might say incomplete or just success with no statement.
			return incompleteResult();
		}
		default:
			// Something else we don't recognize -> error
			return { ParseStatus::PARSE_ERROR, nullptr, "Unknown statement" };
		}
	}
	catch (const std::runtime_error& e) {
		// If an exception was thrown, treat it as an error
		return { ParseStatus::PARSE_ERROR, nullptr, e.what() };
	}

	if (astNode) {
		// Full success
		return { ParseStatus::PARSE_SUCCESS, std::move(astNode), "" };
	}
	else {
		// Means we detected incomplete
		return incompleteResult();
	}
}

std::unique_ptr<ASTNode> Parser::parseDataStep() {
    auto dataNode = std::make_unique<DataStepNode>();

    // consume 'data'
    consume(TokenType::KEYWORD_DATA, "Expected 'data'");

    // We expect an identifier for dataset name
    if (peek().type == TokenType::EOF_TOKEN) {
        // Not enough tokens -> incomplete
        return nullptr;
    }

    Token dsNameTok = advance();
    if (dsNameTok.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected dataset name after 'data'");
    }

    // Optional semicolon
    // In real SAS, you do typically: data someDs; ...
    if (peek().type == TokenType::SEMICOLON) {
        advance(); // consume the semicolon
    }
    else {
        // If missing semicolon, let's treat it as incomplete 
        // (or you can treat it as an error, depending on how strict you want to be)
        return nullptr;
    }

    bool foundRun = false;
    while (!foundRun) {
        Token tok = peek();
        if (tok.type == TokenType::EOF_TOKEN) {
            // We ran out of tokens -> incomplete
            return nullptr;
        }
        else if (tok.type == TokenType::KEYWORD_RUN) {
            // consume 'run'
            advance();
            // next token should be semicolon
            if (peek().type == TokenType::SEMICOLON) {
                advance(); // consume semicolon
                foundRun = true;
            }
            else {
                // Missing semicolon after run -> incomplete or error
                return nullptr;
            }
        }
        else {
            auto parseResult = parseStatement();
            if (parseResult.status == ParseStatus::PARSE_SUCCESS)
                dataNode->statements.push_back(std::move(parseResult.node));
        }
    }

    // If we get here, we found 'run;'
    dataNode->outputDataSet = dsNameTok.text;
    // In real code, you'd also store any statements parsed inside the data step.
    // For now, we just fill in the dataset name.

    return dataNode;
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

    node->libref = to_upper(consume(TokenType::IDENTIFIER, "Expected libref").text);
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
    if (stmt.status == ParseStatus::PARSE_SUCCESS) node->thenStatements.push_back(std::move(stmt.node));

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
    // First parse the "primary" expression
    auto left = parsePrimary();

    while (true) {
        Token t = peek();
        if (t.type == TokenType::EOF_TOKEN) break; // no more tokens

        // e.g., if t.text == "+" or "*", check precedence
        std::string op = t.text;
        int currentPrecedence = getPrecedence(op);

        // If this operator has lower precedence than 'precedence', we stop
        if (currentPrecedence < precedence) break;

        // Otherwise, consume the operator
        advance(); // eat that operator token

        // Handle right-associative operators if any (e.g., exponentiation)
        bool rightAssociative = false; // Adjust as needed

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
    if (op == "=") return -1;

    if (op == "or") return 1;
    if (op == "and") return 2;
    if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=") return 3;
    if (op == "+" || op == "-") return 4;
    if (op == "*" || op == "/") return 5;
    if (op == "**") return 6;

    // If not recognized, return -1 so we treat it as no operator
    return -1;
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
    else if (t.type == TokenType::KEYWORD_FREQ) {
        return parseProcFreq();
    }
    else if (t.type == TokenType::KEYWORD_PRINT) {
        return parseProcPrint();
    }
    else if (t.type == TokenType::KEYWORD_SQL) {
        return parseProcSQL();
    }
    else {
        throw std::runtime_error("Unsupported PROC type: " + t.text);
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
        if (stmt.status == ParseStatus::PARSE_SUCCESS) node->statements.push_back(std::move(stmt.node));
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
    if (stmt.status == ParseStatus::PARSE_SUCCESS) node->thenStatements.push_back(std::move(stmt.node));

    // Check for 'else'
    if (peek().type == TokenType::KEYWORD_ELSE) {
        consume(TokenType::KEYWORD_ELSE, "Expected 'else'");
        auto elseStmt = parseStatement();
        if (elseStmt.status == ParseStatus::PARSE_SUCCESS) node->elseStatements.push_back(std::move(elseStmt.node));
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
    if (stmt.status == ParseStatus::PARSE_SUCCESS) node->thenStatements.push_back(std::move(stmt.node));

    // Handle multiple 'ELSE IF' branches
    while (peek().type == TokenType::KEYWORD_ELSE_IF) {
        consume(TokenType::KEYWORD_ELSE_IF, "Expected 'else if'");
        std::unique_ptr<ASTNode> elseIfCondition = parseExpression();
        consume(TokenType::KEYWORD_THEN, "Expected 'then' after 'else if' condition");

        std::vector<std::unique_ptr<ASTNode>> elseIfStmts;
        auto elseIfStmt = parseStatement();
        if (elseIfStmt.status == ParseStatus::PARSE_SUCCESS) elseIfStmts.push_back(std::move(elseIfStmt.node));

        node->elseIfBranches.emplace_back(std::move(elseIfCondition), std::move(elseIfStmts));
    }

    // Handle 'ELSE' branch
    if (peek().type == TokenType::KEYWORD_ELSE) {
        consume(TokenType::KEYWORD_ELSE, "Expected 'else'");
        auto elseStmt = parseStatement();
        if (elseStmt.status == ParseStatus::PARSE_SUCCESS) node->elseStatements.push_back(std::move(elseStmt.node));
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    consume(TokenType::KEYWORD_DO, "Expected 'do' to start a block");
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (peek().type != TokenType::KEYWORD_ENDDO && peek().type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt.status == ParseStatus::PARSE_SUCCESS) statements.push_back(std::move(stmt.node));
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
        auto parseResult = parseStatement();
        if (parseResult.status == ParseStatus::PARSE_SUCCESS)
            doLoopNode->body->statements.push_back(std::move(parseResult.node));
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

std::unique_ptr<ASTNode> Parser::parseProcFreq() {
    auto procFreqNode = std::make_unique<ProcFreqNode>();
    consume(TokenType::KEYWORD_FREQ, "Expected 'FREQ' keyword after 'PROC'");

    // Parse DATA= option
    if (match(TokenType::KEYWORD_DATA)) {
        consume(TokenType::KEYWORD_DATA, "Expected 'DATA=' option in PROC FREQ");
        Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after 'DATA='");
        procFreqNode->inputDataSet = dataToken.text;
    }
    else {
        throw std::runtime_error("PROC FREQ requires a DATA= option");
    }

    // Parse TABLES statement
    if (match(TokenType::KEYWORD_TABLES)) {
        consume(TokenType::KEYWORD_TABLES, "Expected 'TABLES' keyword in PROC FREQ");
        while (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::STAR) {
            std::string table;
            // Parse table specification, e.g., var1 or var1*var2
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in TABLES statement");
            table += varToken.text;

            if (match(TokenType::STAR)) {
                consume(TokenType::STAR, "Expected '*' in TABLES statement for cross-tabulation");
                Token var2Token = consume(TokenType::IDENTIFIER, "Expected second variable name in TABLES statement");
                table += "*" + var2Token.text;
            }

            // Parse options after '/' if present
            std::vector<std::string> options;
            if (match(TokenType::SLASH)) { // Assuming '/' is tokenized as SLASH
                consume(TokenType::SLASH, "Expected '/' before TABLES options");
                while (peek().type == TokenType::IDENTIFIER) {
                    Token optionToken = consume(TokenType::IDENTIFIER, "Expected option in TABLES statement");
                    options.push_back(optionToken.text);
                    // Handle options with parameters if necessary, e.g., chisq
                }
            }

            procFreqNode->tables.emplace_back(std::make_pair(table, options));
        }
    }
    else {
        throw std::runtime_error("PROC FREQ requires a TABLES statement");
    }

    // Parse optional WHERE statement
    if (match(TokenType::KEYWORD_WHERE)) {
        consume(TokenType::KEYWORD_WHERE, "Expected 'WHERE' keyword in PROC FREQ");
        consume(TokenType::LPAREN, "Expected '(' after 'WHERE'");
        procFreqNode->whereCondition = parseExpression(); // Parse condition expression
        consume(TokenType::RPAREN, "Expected ')' after 'WHERE' condition");
    }

    // Expect RUN; statement
    consume(TokenType::KEYWORD_RUN, "Expected 'RUN;' to terminate PROC FREQ");
    consume(TokenType::SEMICOLON, "Expected ';' after 'RUN'");

    return procFreqNode;
}


std::unique_ptr<ASTNode> Parser::parseProcPrint() {
    bool foundRun = false;
    auto procPrintNode = std::make_unique<ProcPrintNode>();
    consume(TokenType::KEYWORD_PRINT, "Expected 'PRINT' keyword after 'PROC'");

    // Parse options (optional)
    while (match(TokenType::KEYWORD_DATA) ||
        match(TokenType::KEYWORD_OBS) ||
        match(TokenType::KEYWORD_NOOBS) ||
        match(TokenType::KEYWORD_LABEL)) {
        // Parse DATA= option
        if (match(TokenType::KEYWORD_DATA)) {
            consume(TokenType::EQUAL, "Expected '=' after 'DATA'");
            Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after 'DATA='");
            procPrintNode->inputDataSet = dataToken.text;
        }
        if (match(TokenType::KEYWORD_OBS)) {
            consume(TokenType::KEYWORD_OBS, "Expected 'OBS=' option");
            Token eqToken = consume(TokenType::EQUAL, "Expected '=' after 'OBS'");
            Token numToken = consume(TokenType::NUMBER, "Expected number after 'OBS='");
            procPrintNode->options["OBS"] = numToken.text;
        }
        if (match(TokenType::KEYWORD_NOOBS)) {
            consume(TokenType::KEYWORD_NOOBS, "Expected 'NOOBS' option");
            procPrintNode->options["NOOBS"] = "YES";
        }
        if (match(TokenType::KEYWORD_LABEL)) {
            consume(TokenType::KEYWORD_LABEL, "Expected 'LABEL' option");
            procPrintNode->options["LABEL"] = "YES";
        }
    }

    consume(TokenType::SEMICOLON, "Expected ';' to end proc print");

    // Parse VAR statement (optional)
    if (match(TokenType::KEYWORD_VAR)) {
        consume(TokenType::KEYWORD_VAR, "Expected 'VAR' keyword in PROC PRINT");
        while (peek().type == TokenType::IDENTIFIER) {
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in VAR statement");
            procPrintNode->varVariables.push_back(varToken.text);
        }
    }

    // Expect RUN; statement
    if (match(TokenType::KEYWORD_RUN)) {
        foundRun = true;
        consume(TokenType::SEMICOLON, "Expected ';' after 'RUN'");
    }


    if (foundRun) return procPrintNode;
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseProcSQL() {
    auto procSQLNode = std::make_unique<ProcSQLNode>();
    consume(TokenType::KEYWORD_SQL, "Expected 'SQL' keyword after 'PROC'");

    // Parse SQL statements until 'QUIT;' is encountered
    while (!(match(TokenType::KEYWORD_QUIT) && peek().type == TokenType::SEMICOLON)) {
        auto sqlStmt = parseSQLStatement();
        if (sqlStmt) {
            procSQLNode->statements.emplace_back(std::move(sqlStmt));
        }
        else {
            throw std::runtime_error("Unsupported SQL statement in PROC SQL.");
        }
    }

    // Consume 'QUIT;' statement
    consume(TokenType::KEYWORD_QUIT, "Expected 'QUIT' to terminate PROC SQL");
    consume(TokenType::SEMICOLON, "Expected ';' after 'QUIT'");

    return procSQLNode;
}

std::unique_ptr<SQLStatementNode> Parser::parseSQLStatement() {
    Token t = peek();
    if (t.type == TokenType::KEYWORD_SELECT) {
        // Parse SELECT statement
        auto selectStmt = std::make_unique<SelectStatementNode>();
        consume(TokenType::KEYWORD_SELECT, "Expected 'SELECT' keyword");

        // Parse selected columns
        while (true) {
            Token varToken = consume(TokenType::IDENTIFIER, "Expected column name in SELECT statement");
            selectStmt->selectColumns.push_back(varToken.text);

            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA, "Expected ',' between columns in SELECT statement");
            }
            else {
                break;
            }
        }

        // Parse FROM clause
        consume(TokenType::KEYWORD_FROM, "Expected 'FROM' keyword in SELECT statement");
        while (true) {
            Token tableToken = consume(TokenType::IDENTIFIER, "Expected table name in FROM clause");
            selectStmt->fromTables.push_back(tableToken.text);

            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA, "Expected ',' between tables in FROM clause");
            }
            else {
                break;
            }
        }

        // Parse optional WHERE clause
        if (match(TokenType::KEYWORD_WHERE)) {
            consume(TokenType::KEYWORD_WHERE, "Expected 'WHERE' keyword");
            selectStmt->whereCondition = parseExpression(); // Parse condition expression
        }

        // Parse optional GROUP BY clause
        if (match(TokenType::KEYWORD_GROUP)) {
            consume(TokenType::KEYWORD_GROUP, "Expected 'GROUP' keyword");
            consume(TokenType::KEYWORD_BY, "Expected 'BY' keyword after 'GROUP'");
            while (true) {
                Token groupVarToken = consume(TokenType::IDENTIFIER, "Expected column name in GROUP BY clause");
                selectStmt->groupByColumns.push_back(groupVarToken.text);

                if (match(TokenType::COMMA)) {
                    consume(TokenType::COMMA, "Expected ',' between columns in GROUP BY clause");
                }
                else {
                    break;
                }
            }
        }

        // Parse optional HAVING clause
        if (match(TokenType::KEYWORD_HAVING)) {
            consume(TokenType::KEYWORD_HAVING, "Expected 'HAVING' keyword");
            selectStmt->havingCondition = parseExpression(); // Parse HAVING condition expression
        }

        // Parse optional ORDER BY clause
        if (match(TokenType::KEYWORD_ORDER)) {
            consume(TokenType::KEYWORD_ORDER, "Expected 'ORDER' keyword");
            consume(TokenType::KEYWORD_BY, "Expected 'BY' keyword after 'ORDER'");
            while (true) {
                Token orderVarToken = consume(TokenType::IDENTIFIER, "Expected column name in ORDER BY clause");
                selectStmt->orderByColumns.push_back(orderVarToken.text);

                if (match(TokenType::COMMA)) {
                    consume(TokenType::COMMA, "Expected ',' between columns in ORDER BY clause");
                }
                else {
                    break;
                }
            }
        }

        // Consume semicolon at the end of the statement
        consume(TokenType::SEMICOLON, "Expected ';' after SELECT statement");

        return selectStmt;
    }
    else if (t.type == TokenType::KEYWORD_CREATE) {
        // Parse CREATE TABLE statement
        auto createStmt = std::make_unique<CreateTableStatementNode>();
        consume(TokenType::KEYWORD_CREATE, "Expected 'CREATE' keyword");

        consume(TokenType::KEYWORD_TABLE, "Expected 'TABLE' keyword after 'CREATE'");

        Token tableNameToken = consume(TokenType::IDENTIFIER, "Expected table name after 'CREATE TABLE'");
        createStmt->tableName = tableNameToken.text;

        consume(TokenType::LPAREN, "Expected '(' after table name in CREATE TABLE statement");

        // Parse column definitions
        while (true) {
            Token columnToken = consume(TokenType::IDENTIFIER, "Expected column name in CREATE TABLE statement");
            createStmt->columns.push_back(columnToken.text);

            // Optionally, parse data type definitions (e.g., varchar, int)
            // This implementation focuses on column names. Extend as needed.

            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA, "Expected ',' between columns in CREATE TABLE statement");
            }
            else {
                break;
            }
        }

        consume(TokenType::RPAREN, "Expected ')' after column definitions in CREATE TABLE statement");
        consume(TokenType::SEMICOLON, "Expected ';' after CREATE TABLE statement");

        return createStmt;
    }

    // Implement other SQL statement parsers (INSERT, UPDATE, DELETE) similarly

    else {
        // Unsupported SQL statement
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseLetStatement() {
    consume(TokenType::KEYWORD_MACRO_LET, "Expected '%let'");
    std::string varName = consume(TokenType::IDENTIFIER, "Expected macro variable name").text;
    consume(TokenType::EQUAL, "Expected '=' after variable name");
    std::string value = consume(TokenType::STRING, "Expected value for macro variable").text;
    consume(TokenType::SEMICOLON, "Expected ';' after '%let' statement");

    auto node = std::make_unique<MacroVariableAssignmentNode>();
    node->varName = varName;
    node->value = value;
    return node;
}

std::unique_ptr<ASTNode> Parser::parseMacroDefinition() {
    consume(TokenType::KEYWORD_MACRO_MACRO, "Expected '%macro'");
    std::string macroName = consume(TokenType::IDENTIFIER, "Expected macro name").text;
    consume(TokenType::LPAREN, "Expected '(' after macro name");

    // Parse parameters
    std::vector<std::string> parameters;
    if (peek().type != TokenType::RPAREN) { // Handle macros with parameters
        while (true) {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name").text);
            if (peek().type == TokenType::COMMA) {
                consume(TokenType::COMMA, "Expected ',' between parameters");
            }
            else {
                break;
            }
        }
    }
    consume(TokenType::RPAREN, "Expected ')' after parameters");

    auto macroNode = std::make_unique<MacroDefinitionNode>();
    macroNode->macroName = macroName;
    macroNode->parameters = parameters;

    // Parse macro body
    while (peek().type != TokenType::KEYWORD_MACRO_MEND && peek().type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt.status == ParseStatus::PARSE_SUCCESS) macroNode->body.push_back(std::move(stmt.node));
    }
    consume(TokenType::KEYWORD_MACRO_MEND, "Expected '%mend'");
    consume(TokenType::SEMICOLON, "Expected ';' after '%mend'");

    return macroNode;
}

std::unique_ptr<ASTNode> Parser::parseMacroCall() {
    Token macroNameToken = consume(TokenType::IDENTIFIER, "Expected macro name");

    auto node = std::make_unique<MacroCallNode>();
    node->macroName = macroNameToken.text;

    if (peek().type == TokenType::LPAREN) {
        consume(TokenType::LPAREN, "Expected '(' after macro name");

        while (peek().type != TokenType::RPAREN) {
            auto arg = parseExpression();
            node->arguments.push_back(std::move(arg));

            if (peek().type == TokenType::COMMA) {
                consume(TokenType::COMMA, "Expected ',' between arguments");
            }
        }

        consume(TokenType::RPAREN, "Expected ')' after macro arguments");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after macro call");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseInput() {
    // we already consumed "input" token
    auto node = std::make_unique<InputNode>();

    // gather variable names until semicolon
    while (true) {
        Token t = peek();
        if (t.type == TokenType::SEMICOLON || t.type == TokenType::EOF_TOKEN) {
            break;
        }
        // Expect an identifier: e.g. name, age, name$
        if (t.type == TokenType::IDENTIFIER) {
            // This is a variable name
            std::string varName = t.text;
            advance(); // consume

            // Next token might be DOLLAR => string var
            Token nextT = peek();
            bool isString = false;
            if (nextT.type == TokenType::DOLLAR) {
                // then we consume the DOLLAR token
                advance();
                isString = true;
            }

            // store the variable in node->variables
            // maybe store them as (name, isString)
            node->variables.push_back(std::make_pair(varName, isString));
        }
        else {
            throw std::runtime_error("Expected variable name in INPUT statement, got: " + t.text);
        }
    }

    // expect semicolon
    consume(TokenType::SEMICOLON, "Expected ';' after INPUT statement");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseDatalines() {
    // We already consumed KEYWORD_DATALINES in parseStatement(),
    // Next token should be SEMICOLON, which your parseStatement might handle, 
    // or we do:
    // consume(TokenType::SEMICOLON, "Expected ';' after DATALINES");
    // Then the next token we see should be DATALINES_CONTENT
    Token t = peek();

    if (t.type == TokenType::EOF_TOKEN)
        return nullptr;

    if (t.type != TokenType::DATALINES_CONTENT) {
        throw std::runtime_error("Expected DATALINES_CONTENT token after 'datalines;'");
    }
    advance(); // consume it

    auto node = std::make_unique<DatalinesNode>();
    // Let's split the text by lines
    std::vector<std::string> lines;
    {
        std::istringstream iss(t.text);
        std::string line;
        while (std::getline(iss, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
    }
    node->lines = lines;
    return node;
}

}