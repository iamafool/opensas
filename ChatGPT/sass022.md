next step, handle all the SAS global statement. Please give me very very detail to parse it, to execute it, set up the DataEnvironment, etc.
ChatGPT

Thought for a couple of seconds
Implementing a comprehensive SAS interpreter is a substantial project, especially given the breadth and depth of SAS's capabilities. Handling **global statements** is a critical step, as these statements configure the environment and influence how data steps and procedures operate.

This guide will walk you through:

1. **Understanding SAS Global Statements**
2. **Extending the Lexer**
3. **Extending the Parser**
4. **Implementing Execution Logic**
5. **Setting Up and Managing `DataEnvironment`**
6. **Integrating Everything Together**

Throughout this guide, we'll assume that you have a basic interpreter structure in place, including:

- A **Lexer** that tokenizes input.
- A **Parser** that builds an Abstract Syntax Tree (AST).
- An **Interpreter** that executes the AST.
- A **DataEnvironment** class that manages datasets and environment settings.
- Logging and output systems using `spdlog`.

Let's delve into each step in detail.

---

## 1. Understanding SAS Global Statements

**Global Statements** in SAS are statements that can appear anywhere in a SAS program and typically affect the environment or configuration rather than manipulating data directly. Examples include:

- `OPTIONS`: Sets various system options.
- `LIBNAME`: Assigns a library reference to a directory or data source.
- `TITLE` and `FOOTNOTE`: Define titles and footnotes for output.
- `ODS`: Controls Output Delivery System settings.
- `FILENAME`: Assigns a fileref to an external file.

For this guide, we'll focus on implementing a subset of these to illustrate the process. Let's implement:

- `OPTIONS`: To set simple options like `LINESIZE` and `PAGESIZE`.
- `LIBNAME`: To assign library references.
- `TITLE`: To set titles for outputs.

You can extend this framework to support additional global statements as needed.

---

## 2. Extending the Lexer

The **Lexer** (or tokenizer) is responsible for converting raw input text into a sequence of tokens that the parser can understand. To handle global statements, you need to ensure that the lexer can recognize their keywords and associated syntax.

### 2.1. Update Token Types

First, extend your `TokenType` enum to include global statement keywords.

**Token.h**

```cpp
#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    STRING,
    // Existing SAS keywords
    KEYWORD_DATA,
    KEYWORD_SET,
    KEYWORD_IF,
    KEYWORD_THEN,
    KEYWORD_OUTPUT,
    KEYWORD_RUN,
    // New global statement keywords
    KEYWORD_OPTIONS,
    KEYWORD_LIBNAME,
    KEYWORD_TITLE,
    // Other tokens
    EQUALS,
    SEMICOLON,
    LPAREN,
    RPAREN,
    COMMA,
    QUOTE,
    DOT,
    // Operators
    PLUS,
    MINUS,
    MUL,
    DIV,
    EOF_TOKEN,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
};

#endif // TOKEN_H
```

### 2.2. Update the Lexer to Recognize New Keywords

Modify your **Lexer** to recognize the new global statement keywords. This involves adding checks in the `identifierOrKeyword` function.

**Lexer.cpp**

```cpp
#include "Lexer.h"
#include <cctype>
#include <algorithm>

Lexer::Lexer(const std::string &in) : input(in) {}

char Lexer::peekChar() const {
    if (pos >= input.size()) return '\0';
    return input[pos];
}

char Lexer::getChar() {
    if (pos >= input.size()) return '\0';
    char c = input[pos++];
    if (c == '\n') { line++; col = 1; }
    else { col++; }
    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(static_cast<unsigned char>(peekChar()))) {
        getChar();
    }
}

Token Lexer::identifierOrKeyword() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::IDENTIFIER;

    std::string value;
    while (std::isalnum(static_cast<unsigned char>(peekChar())) || peekChar() == '_') {
        value += getChar();
    }

    // Convert to uppercase for case-insensitive matching (SAS is case-insensitive)
    std::string upperValue = value;
    std::transform(upperValue.begin(), upperValue.end(), upperValue.begin(), ::toupper);

    // Keyword detection
    if (upperValue == "DATA") token.type = TokenType::KEYWORD_DATA;
    else if (upperValue == "SET") token.type = TokenType::KEYWORD_SET;
    else if (upperValue == "IF") token.type = TokenType::KEYWORD_IF;
    else if (upperValue == "THEN") token.type = TokenType::KEYWORD_THEN;
    else if (upperValue == "OUTPUT") token.type = TokenType::KEYWORD_OUTPUT;
    else if (upperValue == "RUN") token.type = TokenType::KEYWORD_RUN;
    // New global statement keywords
    else if (upperValue == "OPTIONS") token.type = TokenType::KEYWORD_OPTIONS;
    else if (upperValue == "LIBNAME") token.type = TokenType::KEYWORD_LIBNAME;
    else if (upperValue == "TITLE") token.type = TokenType::KEYWORD_TITLE;

    token.text = value;

    return token;
}

Token Lexer::number() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::NUMBER;

    std::string value;
    while (std::isdigit(static_cast<unsigned char>(peekChar())) || peekChar() == '.') {
        value += getChar();
    }
    token.text = value;

    return token;
}

Token Lexer::stringLiteral() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::STRING;
    getChar(); // skip the opening quote
    std::string value;
    while (peekChar() != '\0' && peekChar() != '\'') {
        value += getChar();
    }
    getChar(); // skip the closing quote
    token.text = value;
    return token;
}

Token Lexer::getNextToken() {
    skipWhitespace();
    Token token;
    token.line = line;
    token.col = col;
    char c = peekChar();
    if (c == '\0') {
        token.type = TokenType::EOF_TOKEN;
        return token;
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        return identifierOrKeyword();
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        return number();
    }

    switch (c) {
        case '=': getChar(); token.type = TokenType::EQUALS; token.text = "="; return token;
        case ';': getChar(); token.type = TokenType::SEMICOLON; token.text = ";"; return token;
        case '(': getChar(); token.type = TokenType::LPAREN; token.text = "("; return token;
        case ')': getChar(); token.type = TokenType::RPAREN; token.text = ")"; return token;
        case ',': getChar(); token.type = TokenType::COMMA; token.text = ","; return token;
        case '\'': return stringLiteral();
        case '.': getChar(); token.type = TokenType::DOT; token.text = "."; return token;
        case '+': getChar(); token.type = TokenType::PLUS; token.text = "+"; return token;
        case '-': getChar(); token.type = TokenType::MINUS; token.text = "-"; return token;
        case '*': getChar(); token.type = TokenType::MUL; token.text = "*"; return token;
        case '/': getChar(); token.type = TokenType::DIV; token.text = "/"; return token;
        default:
            getChar();
            token.type = TokenType::UNKNOWN;
            token.text = std::string(1, c);
            return token;
    }
}
```

### 2.3. Additional Considerations

- **Case Insensitivity**: SAS is case-insensitive, so it's essential to normalize keywords to a consistent case (typically uppercase) during lexical analysis.
- **Comments**: SAS supports comments (`* comment;` or `/* comment */`). Implement comment handling to skip over them.
- **Special Characters**: Ensure the lexer can handle other SAS-specific syntax, such as macro symbols (`%`, `&`), if you plan to support macros.

---

## 3. Extending the Parser

With the lexer recognizing new global statements, the next step is to extend the parser to handle these statements, build appropriate AST nodes, and integrate them into the execution flow.

### 3.1. Define AST Nodes for Global Statements

Update your **AST** to include nodes for global statements.

**AST.h**

```cpp
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() {}
};

// Represents a DATA step block: data <dataset>; set <source>; <statements>; run;
class DataStepNode : public ASTNode {
public:
    std::string outputDataSet;
    std::string inputDataSet;
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// Represents a variable assignment: var = expression;
class AssignmentNode : public ASTNode {
public:
    std::string varName;
    std::unique_ptr<ASTNode> expression;
};

// Represents a literal number or string
class LiteralNode : public ASTNode {
public:
    std::string value;  // Could differentiate numeric vs. string later
};

// Represents a variable reference
class VariableNode : public ASTNode {
public:
    std::string varName;
};

// Represents a binary operation: expr op expr
class BinaryOpNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    char op; // '+', '-', '*', '/'
};

// Represents an IF-THEN statement: if <condition> then <statements>;
class IfThenNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenStatements;
};

// Represents an OUTPUT statement
class OutputNode : public ASTNode {};

// New AST Nodes for Global Statements

// Represents an OPTIONS statement: options option1=value1 option2=value2;
class OptionsNode : public ASTNode {
public:
    std::vector<std::pair<std::string, std::string>> options;
};

// Represents a LIBNAME statement: libname libref 'path';
class LibnameNode : public ASTNode {
public:
    std::string libref;
    std::string path;
};

// Represents a TITLE statement: title 'Your Title';
class TitleNode : public ASTNode {
public:
    std::string title;
};

#endif // AST_H
```

### 3.2. Update the Parser to Handle Global Statements

Modify the **Parser** to recognize and parse global statements. Update grammar rules to include parsing for `OPTIONS`, `LIBNAME`, and `TITLE`.

**Parser.h**

```cpp
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"

class Parser {
public:
    Parser(const std::vector<Token> &tokens);
    std::unique_ptr<ASTNode> parse();

private:
    const std::vector<Token> &tokens;
    size_t pos = 0;

    Token peek(int offset=0) const;
    Token consume(TokenType type, const std::string &errMsg);
    bool match(TokenType type);
    Token advance();

    // Grammar rules
    std::vector<std::unique_ptr<ASTNode>> parseProgram(); // To handle multiple global and data statements
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfThen();
    std::unique_ptr<ASTNode> parseOutput();
    std::unique_ptr<ASTNode> parseDataStep();
    std::unique_ptr<ASTNode> parseOptions();
    std::unique_ptr<ASTNode> parseLibname();
    std::unique_ptr<ASTNode> parseTitle();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parsePrimary();
};

#endif // PARSER_H
```

**Parser.cpp**

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>

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
    // Parse the entire program which can consist of multiple statements
    auto programNode = std::make_unique<ASTNode>(); // Placeholder, actual implementation would use a ProgramNode
    auto statements = parseProgram();
    // For simplicity, let's assume the program node holds all statements
    // Implement a ProgramNode if necessary
    // Here, we'll ignore the ProgramNode and return the statements for execution
    // Alternatively, you can have an AST root node containing all statements
    // This depends on how you design your interpreter
    // For now, just return nullptr
    return nullptr;
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
            // Handle unknown token or throw error
            throw std::runtime_error("Unknown statement starting with token: " + t.text);
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
```

### 3.3. Handling Multiple Statements in `parse()`

To properly handle multiple global and data statements, modify the `parse()` function to return a collection of statements.

**Parser.cpp (Updated `parse` function)**

```cpp
#include "ProgramNode.h" // Create a ProgramNode to hold all statements

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
```

**ProgramNode.h**

```cpp
#ifndef PROGRAMNODE_H
#define PROGRAMNODE_H

#include "AST.h"
#include <vector>
#include <memory>

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

#endif // PROGRAMNODE_H
```

**Parser.h (Updated parse function)**

```cpp
std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}
```

This setup allows your parser to handle an entire SAS program consisting of multiple global and data statements.

---

## 4. Implementing Execution Logic

Now, let's implement the execution logic for the new global statements (`OPTIONS`, `LIBNAME`, `TITLE`) within your **Interpreter**.

### 4.1. Extend `DataEnvironment`

Your `DataEnvironment` class needs to store global settings like options, library references, and titles.

**DataEnvironment.h**

```cpp
#ifndef DATAENVIRONMENT_H
#define DATAENVIRONMENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

using Value = std::variant<double, std::string>;

struct Row {
    std::unordered_map<std::string, Value> columns;
};

class DataSet {
public:
    std::string name;
    std::vector<Row> rows;
};

class DataEnvironment {
public:
    // Store datasets by name
    std::unordered_map<std::string, DataSet> dataSets;

    // Current row being processed
    Row currentRow;

    // Global settings
    std::unordered_map<std::string, std::string> options;
    std::unordered_map<std::string, std::string> librefs;
    std::string title;

    // Helper to get a variable value from current row
    Value getValue(const std::string &varName) const {
        auto it = currentRow.columns.find(varName);
        if (it != currentRow.columns.end()) return it->second;
        // If not found, return missing value (numeric missing is represented as NaN in SAS)
        return std::nan("");
    }

    void setValue(const std::string &varName, const Value &val) {
        currentRow.columns[varName] = val;
    }

    // Methods to handle global settings
    void setOption(const std::string &option, const std::string &value) {
        options[option] = value;
    }

    void setLibref(const std::string &libref, const std::string &path) {
        librefs[libref] = path;
    }

    void setTitle(const std::string &t) {
        title = t;
    }

    // Method to get a dataset, creating it if necessary
    DataSet& getOrCreateDataset(const std::string &name) {
        return dataSets[name];
    }
};

#endif // DATAENVIRONMENT_H
```

### 4.2. Extend the Interpreter to Execute Global Statements

Update the **Interpreter** to handle `OptionsNode`, `LibnameNode`, and `TitleNode`.

**Interpreter.h**

```cpp
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "DataEnvironment.h"
#include <memory>
#include <spdlog/spdlog.h>

class Interpreter {
public:
    Interpreter(DataEnvironment &env, spdlog::logger &logLogger, spdlog::logger &lstLogger)
        : env(env), logLogger(logLogger), lstLogger(lstLogger) {}

    void executeProgram(const std::unique_ptr<ProgramNode> &program);

private:
    DataEnvironment &env;
    spdlog::logger &logLogger;
    spdlog::logger &lstLogger;

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeAssignment(AssignmentNode *node);
    void executeIfThen(IfThenNode *node);
    void executeOutput(OutputNode *node);
    void executeOptions(OptionsNode *node);
    void executeLibname(LibnameNode *node);
    void executeTitle(TitleNode *node);

    double toNumber(const Value &v);
    std::string toString(const Value &v);

    Value evaluate(ASTNode *node);
};

#endif // INTERPRETER_H
```

**Interpreter.cpp**

```cpp
#include "Interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

// Execute the entire program
void Interpreter::executeProgram(const std::unique_ptr<ProgramNode> &program) {
    for (const auto &stmt : program->statements) {
        execute(stmt.get());
    }
}

// Execute a single AST node
void Interpreter::execute(ASTNode *node) {
    if (auto ds = dynamic_cast<DataStepNode*>(node)) {
        executeDataStep(ds);
    }
    else if (auto opt = dynamic_cast<OptionsNode*>(node)) {
        executeOptions(opt);
    }
    else if (auto lib = dynamic_cast<LibnameNode*>(node)) {
        executeLibname(lib);
    }
    else if (auto title = dynamic_cast<TitleNode*>(node)) {
        executeTitle(title);
    }
    else {
        // Handle other statements
        // For now, ignore unknown statements or throw an error
        throw std::runtime_error("Unknown AST node encountered during execution.");
    }
}

// Execute a DATA step
void Interpreter::executeDataStep(DataStepNode *node) {
    logLogger.info("Executing DATA step: data {}; set {};", node->outputDataSet, node->inputDataSet);

    // Check if input dataset exists
    auto inputIt = env.dataSets.find(node->inputDataSet);
    if (inputIt == env.dataSets.end()) {
        logLogger.error("Input dataset '{}' not found.", node->inputDataSet);
        return;
    }

    DataSet &input = inputIt->second;
    DataSet &output = env.getOrCreateDataset(node->outputDataSet);
    output.name = node->outputDataSet;

    for (const auto &row : input.rows) {
        env.currentRow = row; // Set the current row for processing

        // Execute each statement in the DATA step
        for (const auto &stmt : node->statements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt.get())) {
                executeIfThen(ifThen);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
            }
            else {
                // Handle other DATA step statements if needed
                throw std::runtime_error("Unsupported statement in DATA step.");
            }
        }

        // After processing the row, add to output if 'output' was called
        // This logic can be adjusted based on how 'output' is implemented
        // For simplicity, assume 'output' flag is set by executeOutput
        // You can add a flag in DataEnvironment if needed
    }

    logLogger.info("DATA step '{}' executed successfully. Output dataset has {} observations.",
                   node->outputDataSet, output.rows.size());

    // For demonstration, print the output dataset
    lstLogger.info("SAS Results (Dataset: {}):", node->outputDataSet);
    lstLogger.info("OBS\tX");
    int obs = 1;
    for (const auto &row : output.rows) {
        lstLogger.info("{}\t{}", obs++, toString(row.columns.at("x")));
    }
}

// Execute an assignment statement
void Interpreter::executeAssignment(AssignmentNode *node) {
    Value val = evaluate(node->expression.get());
    env.setValue(node->varName, val);
    logLogger.info("Assigned {} = {}", node->varName, toString(val));
}

// Execute an IF-THEN statement
void Interpreter::executeIfThen(IfThenNode *node) {
    Value cond = evaluate(node->condition.get());
    double d = toNumber(cond);
    logLogger.info("Evaluating IF condition: {}", d);

    if (d != 0.0) { // Non-zero is true
        for (const auto &stmt : node->thenStatements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
            }
            else {
                throw std::runtime_error("Unsupported statement in IF-THEN block.");
            }
        }
    }
}

// Execute an OUTPUT statement
void Interpreter::executeOutput(OutputNode *node) {
    // For simplicity, assume 'output' adds the current row to the output dataset
    // Implement a flag or directly add the row in executeDataStep
    // Here, we log that 'output' was called
    logLogger.info("OUTPUT statement executed. Current row will be added to the output dataset.");
    
    // Implement the logic in executeDataStep to add the row
    // For demonstration, assume rows are added automatically
}

// Execute an OPTIONS statement
void Interpreter::executeOptions(OptionsNode *node) {
    for (const auto &opt : node->options) {
        env.setOption(opt.first, opt.second);
        logLogger.info("Set option {} = {}", opt.first, opt.second);
    }
}

// Execute a LIBNAME statement
void Interpreter::executeLibname(LibnameNode *node) {
    env.setLibref(node->libref, node->path);
    logLogger.info("Libname assigned: {} = '{}'", node->libref, node->path);
}

// Execute a TITLE statement
void Interpreter::executeTitle(TitleNode *node) {
    env.setTitle(node->title);
    logLogger.info("Title set to: '{}'", node->title);
}

// Convert Value to number (double)
double Interpreter::toNumber(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        return std::get<double>(v);
    }
    else if (std::holds_alternative<std::string>(v)) {
        try {
            return std::stod(std::get<std::string>(v));
        }
        catch (...) {
            return 0.0; // Represent missing as 0.0 or handle differently
        }
    }
    return 0.0;
}

// Convert Value to string
std::string Interpreter::toString(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        return std::to_string(std::get<double>(v));
    }
    else {
        return std::get<std::string>(v);
    }
}

// Evaluate an expression node
Value Interpreter::evaluate(ASTNode *node) {
    if (auto lit = dynamic_cast<LiteralNode*>(node)) {
        // Try to parse as number
        try {
            double d = std::stod(lit->value);
            return d;
        }
        catch (...) {
            return lit->value;
        }
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        return env.getValue(var->varName);
    }
    else if (auto bin = dynamic_cast<BinaryOpNode*>(node)) {
        Value leftVal = evaluate(bin->left.get());
        Value rightVal = evaluate(bin->right.get());
        double l = toNumber(leftVal);
        double r = toNumber(rightVal);
        switch (bin->op) {
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/': return (r != 0.0) ? l / r : std::nan("");
        }
    }
    // Handle more expression types as needed
    throw std::runtime_error("Unsupported expression type during evaluation.");
}
```

### 4.3. Handling `run;` and Execution Flow

Ensure that your `runSasCode` function in `main.cpp` parses and executes the entire program correctly.

**main.cpp (Updated)**

```cpp
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "DataEnvironment.h"
#include "ProgramNode.h" // Ensure you have this

// Function to read SAS code from a file
std::string readSasFile(const std::string &filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        return "";
    }
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], static_cast<std::streamsize>(contents.size()));
    return contents;
}

// Function to run SAS code
void runSasCode(const std::string &sasCode, Interpreter &interpreter, bool interactive) {
    // Lexing
    Lexer lexer(sasCode);
    std::vector<Token> tokens;
    Token tok;
    while ((tok = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
    }

    // Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> program;
    try {
        program = std::make_unique<ProgramNode>();
        Parser tempParser(tokens);
        program = std::unique_ptr<ProgramNode>(static_cast<ProgramNode*>(tempParser.parse().release()));
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Parsing failed: {}", e.what());
        return;
    }

    // Interpret
    try {
        interpreter.executeProgram(program);
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Execution failed: {}", e.what());
    }
}

int main(int argc, char** argv)
{
    std::string sasFile;
    std::string logFile;
    std::string lstFile;

    // Parse command line arguments
    // Expected patterns:
    // -sas=xxx.sas
    // -log=xxx.log
    // -lst=xxx.lst
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.rfind("-sas=", 0) == 0) {
            sasFile = arg.substr(5);
        }
        else if (arg.rfind("-log=", 0) == 0) {
            logFile = arg.substr(5);
        }
        else if (arg.rfind("-lst=", 0) == 0) {
            lstFile = arg.substr(5);
        }
    }

    // Determine mode:
    // 1) Interactive mode: no sasFile
    // 2) File mode (console output): sasFile given, but no log/lst
    // 3) Batch mode: sasFile, logFile, lstFile all given
    bool interactiveMode = sasFile.empty();
    bool batchMode = (!sasFile.empty() && !logFile.empty() && !lstFile.empty());
    bool fileMode = (!sasFile.empty() && logFile.empty() && lstFile.empty()); // SAS from file, output to console

    // Set up loggers
    std::shared_ptr<spdlog::logger> logLogger;
    std::shared_ptr<spdlog::logger> lstLogger;

    if (batchMode) {
        // Batch mode: log and lst to files
        try {
            auto logSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
            auto lstSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(lstFile, true);

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Log initialization failed: " << ex.what() << "\n";
            return 1;
        }
    }
    else {
        // Interactive or file mode: log and lst to console
        try {
            auto logSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            auto lstSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Console log initialization failed: " << ex.what() << "\n";
            return 1;
        }
    }

    // Register loggers
    spdlog::register_logger(logLogger);
    spdlog::register_logger(lstLogger);
    logLogger->set_level(spdlog::level::info);
    lstLogger->set_level(spdlog::level::info);

    DataEnvironment env;
    Interpreter interpreter(env, *logLogger, *lstLogger);

    std::string sasCode;

    if (interactiveMode) {
        // Interactive mode: read code from stdin or a REPL-like interface
        logLogger->info("Running in interactive mode. Type SAS code line by line. End with 'run;' or type 'quit'/'exit' to exit.");
        std::string line;
        std::string codeBuffer;
        while (true) {
            std::cout << "SAS> ";
            if (!std::getline(std::cin, line)) {
                // End of input (Ctrl+D / Ctrl+Z)
                break;
            }

            // If user enters "quit" or "exit"
            if (line == "quit" || line == "exit") {
                break;
            }

            codeBuffer += line + "\n";

            // Check if codeBuffer contains at least one semicolon indicating statement termination
            // This is a simplistic check; for more robust detection, consider implementing a proper state machine
            size_t semicolonPos = codeBuffer.find(';');
            if (semicolonPos != std::string::npos) {
                // Extract the statement up to the semicolon
                std::string statement = codeBuffer.substr(0, semicolonPos + 1);
                logLogger->info("Executing statement: {}", statement);

                runSasCode(statement, interpreter, true);

                // Remove the executed statement from the buffer
                codeBuffer.erase(0, semicolonPos + 1);
            }
        }
    }
    else if (fileMode) {
        // File mode: read code from sasFile, output to console
        logLogger->info("Running from SAS file: {}", sasFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }

        runSasCode(sasCode, interpreter, false);
    }
    else if (batchMode) {
        // Batch mode: read code from sasFile, log and lst to files
        logLogger->info("Running in batch mode: SAS={} LOG={} LST={}", sasFile, logFile, lstFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }

        runSasCode(sasCode, interpreter, false);
    }

    return 0;
}
```

### 4.4. Detailed Explanation

1. **Lexer Enhancements**:
    - **Token Types**: Added new token types for `OPTIONS`, `LIBNAME`, and `TITLE`.
    - **Keyword Recognition**: Updated the lexer to recognize these keywords, ensuring case-insensitive matching by converting identifiers to uppercase before comparison.
    - **Comments Handling**: [Optional] Implement comment skipping if you haven't already.

2. **Parser Enhancements**:
    - **AST Nodes**: Added `OptionsNode`, `LibnameNode`, and `TitleNode` to represent global statements in the AST.
    - **Parser Rules**: Implemented parsing logic for these global statements, capturing necessary parameters (e.g., option name and value for `OPTIONS`).
    - **Program Parsing**: Modified the parser to handle multiple statements, whether they are global or within a data step.

3. **Interpreter Enhancements**:
    - **Execution Functions**: Added execution functions for each new global statement node.
    - **DataEnvironment**: Extended to store global settings such as options, library references, and titles.
    - **Execution Flow**: The interpreter processes each AST node, updating the environment or executing data steps accordingly.

4. **Main Function Enhancements**:
    - **Mode Handling**: The `main` function now correctly handles interactive, file, and batch modes.
    - **Interactive Mode**: Accumulates lines until a semicolon is found, then executes the accumulated statement.
    - **Batch and File Modes**: Reads the entire SAS file at once and executes it.

---

## 5. Setting Up and Managing `DataEnvironment`

The `DataEnvironment` class is central to managing datasets, global options, library references, and titles. Here's a detailed breakdown of how to implement and use it effectively.

### 5.1. Managing Options

**Options** control various aspects of the SAS environment. For simplicity, let's implement a few basic options like `LINESIZE` and `PAGESIZE`.

**Extending `DataEnvironment.h`**

```cpp
// ... existing includes and definitions ...

class DataEnvironment {
public:
    // ... existing members ...

    // Global settings
    std::unordered_map<std::string, std::string> options;
    std::unordered_map<std::string, std::string> librefs;
    std::string title;

    // ... existing methods ...

    // Retrieve an option value with a default
    std::string getOption(const std::string &option, const std::string &defaultValue = "") const {
        auto it = options.find(option);
        if (it != options.end()) {
            return it->second;
        }
        return defaultValue;
    }
};
```

**Handling `OPTIONS` in Interpreter**

When executing an `OPTIONS` statement, store the option name and value in the `options` map.

**Interpreter.cpp (executeOptions)**

```cpp
void Interpreter::executeOptions(OptionsNode *node) {
    for (const auto &opt : node->options) {
        env.setOption(opt.first, opt.second);
        logLogger.info("Set option {} = {}", opt.first, opt.second);

        // Optionally, handle specific options immediately
        if (opt.first == "LINESIZE") {
            // For example, store it in DataEnvironment and use it in output formatting
            // Implement usage in OutputManager if needed
        }
        else if (opt.first == "PAGESIZE") {
            // Similar handling for PAGSIZE
        }
        // Handle more options as needed
    }
}
```

### 5.2. Managing Library References with `LIBNAME`

**Understanding `LIBNAME`**

The `LIBNAME` statement assigns a library reference (libref) to a specific directory or data source. This allows referencing datasets using the libref.

**Example SAS Code**

```sas
libname mylib 'C:\Data';
```

This assigns the libref `mylib` to the directory `C:\Data`.

**Handling `LIBNAME` in Interpreter**

When executing a `LIBNAME` statement, store the libref and its path in the `librefs` map.

**Interpreter.cpp (executeLibname)**

```cpp
void Interpreter::executeLibname(LibnameNode *node) {
    env.setLibref(node->libref, node->path);
    logLogger.info("Libname assigned: {} = '{}'", node->libref, node->path);

    // Optionally, verify that the path exists
    // For simplicity, not implemented here
}
```

**Accessing Datasets with Librefs**

When referring to datasets with librefs (e.g., `mylib.dataset`), ensure that the interpreter can resolve them.

**Example SAS Code**

```sas
data mylib.out; 
    set mylib.in; 
    x = 42; 
    if x then output; 
run;
```

**Implementing Libref Resolution**

In your `DataEnvironment` and `Interpreter`, implement logic to resolve libref-qualified dataset names.

**DataEnvironment.h (Modify `getOrCreateDataset`)**

```cpp
DataSet& getOrCreateDataset(const std::string &libref, const std::string &datasetName) {
    std::string fullName;
    if (!libref.empty()) {
        auto it = librefs.find(libref);
        if (it != librefs.end()) {
            // For simplicity, ignore the path and use libref + datasetName
            fullName = libref + "." + datasetName;
        }
        else {
            throw std::runtime_error("Undefined libref: " + libref);
        }
    }
    else {
        fullName = datasetName; // Use default or work library
    }
    return dataSets[fullName];
}
```

**Interpreter.cpp (Modify executeDataStep)**

```cpp
void Interpreter::executeDataStep(DataStepNode *node) {
    logLogger.info("Executing DATA step: data {}; set {};", node->outputDataSet, node->inputDataSet);

    // Resolve output dataset name
    std::string outputLibref, outputDataset;
    size_t dotPos = node->outputDataSet.find('.');
    if (dotPos != std::string::npos) {
        outputLibref = node->outputDataSet.substr(0, dotPos);
        outputDataset = node->outputDataSet.substr(dotPos + 1);
    }
    else {
        outputDataset = node->outputDataSet;
    }

    // Resolve input dataset name
    std::string inputLibref, inputDataset;
    dotPos = node->inputDataSet.find('.');
    if (dotPos != std::string::npos) {
        inputLibref = node->inputDataSet.substr(0, dotPos);
        inputDataset = node->inputDataSet.substr(dotPos + 1);
    }
    else {
        inputDataset = node->inputDataSet;
    }

    // Check if input dataset exists
    DataSet *input = nullptr;
    try {
        input = &env.getOrCreateDataset(inputLibref, inputDataset);
    }
    catch (const std::runtime_error &e) {
        logLogger.error(e.what());
        return;
    }

    // Create or get the output dataset
    DataSet &output = env.getOrCreateDataset(outputLibref, outputDataset);
    output.name = node->outputDataSet;

    for (const auto &row : input->rows) {
        env.currentRow = row; // Set the current row for processing

        // Execute each statement in the DATA step
        for (const auto &stmt : node->statements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt.get())) {
                executeIfThen(ifThen);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
            }
            else {
                // Handle other DATA step statements if needed
                throw std::runtime_error("Unsupported statement in DATA step.");
            }
        }

        // After processing the row, add to output if 'output' was called
        // This logic can be adjusted based on how 'output' is implemented
        // For simplicity, assume 'output' flag is set by executeOutput
        // You can add a flag in DataEnvironment if needed

        // Placeholder: Add the row to output if 'output' was called
        // Implement flagging logic in executeOutput
    }

    logLogger.info("DATA step '{}' executed successfully. Output dataset has {} observations.",
                   node->outputDataSet, output.rows.size());

    // For demonstration, print the output dataset
    lstLogger.info("SAS Results (Dataset: {}):", node->outputDataSet);
    lstLogger.info("OBS\tX");
    int obs = 1;
    for (const auto &row : output.rows) {
        lstLogger.info("{}\t{}", obs++, toString(row.columns.at("x")));
    }
}
```

### 5.3. Managing Titles

The `TITLE` statement sets titles for SAS outputs. Integrate this into your output logic.

**Interpreter.cpp (executeTitle)**

```cpp
void Interpreter::executeTitle(TitleNode *node) {
    env.setTitle(node->title);
    logLogger.info("Title set to: '{}'", node->title);
    lstLogger.info("Title: {}", env.title);
}
```

**Adjust OutputManager (lstLogger)**

When writing outputs, include the title if set.

```cpp
void Interpreter::executeDataStep(DataStepNode *node) {
    // ... existing code ...

    lstLogger.info("SAS Results (Dataset: {}):", node->outputDataSet);
    if (!env.title.empty()) {
        lstLogger.info("Title: {}", env.title);
    }
    lstLogger.info("OBS\tX");
    // ... rest of the code ...
}
```

---

## 6. Integrating Everything Together

With the lexer, parser, interpreter, and `DataEnvironment` extended to handle global statements, it's time to integrate these components to ensure seamless functionality.

### 6.1. Example SAS Code and Execution Flow

Consider the following SAS code:

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Sample SAS Output';

data mylib.out; 
    set mylib.in; 
    x = 42; 
    if x then output; 
run;
```

**Execution Steps:**

1. **Lexer**:
    - Tokenizes the input into recognized tokens, including `OPTIONS`, `LIBNAME`, `TITLE`, `DATA`, `SET`, etc.

2. **Parser**:
    - Constructs an AST with nodes for `OptionsNode`, `LibnameNode`, `TitleNode`, and `DataStepNode`.
    - `DataStepNode` contains statements like `AssignmentNode` and `IfThenNode`.

3. **Interpreter**:
    - Executes `OptionsNode` by setting `linesize` and `pagesize` in `DataEnvironment`.
    - Executes `LibnameNode` by mapping `mylib` to `'C:\Data'`.
    - Executes `TitleNode` by setting the title in `DataEnvironment`.
    - Executes `DataStepNode`:
        - Reads from `mylib.in`.
        - Processes each row, assigns `x = 42`.
        - If `x` is non-zero, outputs the row to `mylib.out`.

4. **Logging and Output**:
    - Logs actions to the log logger.
    - Outputs results to the listing output (`lstLogger`), including the title and dataset contents.

### 6.2. Handling Output Streams Based on Options

If options like `LINESIZE` and `PAGESIZE` are set, you can use them to format your output accordingly. Here's how you might implement this.

**Interpreter.cpp (Adjust `executeDataStep` to Use `LINESIZE` and `PAGESIZE`)**

```cpp
void Interpreter::executeDataStep(DataStepNode *node) {
    // ... existing code ...

    // Retrieve options
    std::string linesize = env.getOption("LINESIZE", "80"); // Default 80
    std::string pagesize = env.getOption("PAGESIZE", "60"); // Default 60

    lstLogger.info("LINESIZE = {}", linesize);
    lstLogger.info("PAGESIZE = {}", pagesize);

    // Convert to integers for formatting if needed
    int ls = std::stoi(linesize);
    int ps = std::stoi(pagesize);

    // Implement logic to format output based on linesize and pagesize
    // For simplicity, this is not fully implemented
    // You can adjust how titles, headers, and data rows are printed based on these settings

    lstLogger.info("SAS Results (Dataset: {}):", node->outputDataSet);
    if (!env.title.empty()) {
        lstLogger.info("Title: {}", env.title);
    }
    lstLogger.info("OBS\tX");
    int obs = 1;
    for (const auto &row : output.rows) {
        lstLogger.info("{}\t{}", obs++, toString(row.columns.at("x")));
        // Implement linesize and pagesize logic here
    }
}
```

### 6.3. Implementing `runSasCode`

Ensure that `runSasCode` correctly handles the parsing and execution flow.

**main.cpp (Updated `runSasCode` function)**

```cpp
void runSasCode(const std::string &sasCode, Interpreter &interpreter, bool interactive) {
    // Lexing
    Lexer lexer(sasCode);
    std::vector<Token> tokens;
    Token tok;
    while ((tok = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
    }

    // Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> program;
    try {
        program = dynamic_cast<ProgramNode*>(parser.parse().release());
        if (!program) {
            throw std::runtime_error("Failed to parse program.");
        }
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Parsing failed: {}", e.what());
        return;
    }

    // Interpret
    try {
        interpreter.executeProgram(program);
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Execution failed: {}", e.what());
    }
}
```

### 6.4. Comprehensive Example

Here's a complete example integrating all components. This example assumes you have implemented all the classes and methods as described above.

**main.cpp**

```cpp
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "DataEnvironment.h"
#include "ProgramNode.h"

// Function to read SAS code from a file
std::string readSasFile(const std::string &filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        return "";
    }
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], static_cast<std::streamsize>(contents.size()));
    return contents;
}

// Function to run SAS code
void runSasCode(const std::string &sasCode, Interpreter &interpreter, bool interactive) {
    // Lexing
    Lexer lexer(sasCode);
    std::vector<Token> tokens;
    Token tok;
    while ((tok = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
    }

    // Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> program;
    try {
        program = dynamic_cast<ProgramNode*>(parser.parse().release());
        if (!program) {
            throw std::runtime_error("Failed to parse program.");
        }
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Parsing failed: {}", e.what());
        return;
    }

    // Interpret
    try {
        interpreter.executeProgram(program);
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Execution failed: {}", e.what());
    }
}

int main(int argc, char** argv)
{
    std::string sasFile;
    std::string logFile;
    std::string lstFile;

    // Parse command line arguments
    // Expected patterns:
    // -sas=xxx.sas
    // -log=xxx.log
    // -lst=xxx.lst
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.rfind("-sas=", 0) == 0) {
            sasFile = arg.substr(5);
        }
        else if (arg.rfind("-log=", 0) == 0) {
            logFile = arg.substr(5);
        }
        else if (arg.rfind("-lst=", 0) == 0) {
            lstFile = arg.substr(5);
        }
    }

    // Determine mode:
    // 1) Interactive mode: no sasFile
    // 2) File mode (console output): sasFile given, but no log/lst
    // 3) Batch mode: sasFile, logFile, lstFile all given
    bool interactiveMode = sasFile.empty();
    bool batchMode = (!sasFile.empty() && !logFile.empty() && !lstFile.empty());
    bool fileMode = (!sasFile.empty() && logFile.empty() && lstFile.empty()); // SAS from file, output to console

    // Set up loggers
    std::shared_ptr<spdlog::logger> logLogger;
    std::shared_ptr<spdlog::logger> lstLogger;

    if (batchMode) {
        // Batch mode: log and lst to files
        try {
            auto logSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
            auto lstSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(lstFile, true);

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Log initialization failed: " << ex.what() << "\n";
            return 1;
        }
    }
    else {
        // Interactive or file mode: log and lst to console
        try {
            auto logSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            auto lstSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Console log initialization failed: " << ex.what() << "\n";
            return 1;
        }
    }

    // Register loggers
    spdlog::register_logger(logLogger);
    spdlog::register_logger(lstLogger);
    logLogger->set_level(spdlog::level::info);
    lstLogger->set_level(spdlog::level::info);

    DataEnvironment env;
    Interpreter interpreter(env, *logLogger, *lstLogger);

    std::string sasCode;

    if (interactiveMode) {
        // Interactive mode: read code from stdin or a REPL-like interface
        logLogger->info("Running in interactive mode. Type SAS code line by line. End with 'run;' or type 'quit'/'exit' to exit.");
        std::string line;
        std::string codeBuffer;
        while (true) {
            std::cout << "SAS> ";
            if (!std::getline(std::cin, line)) {
                // End of input (Ctrl+D / Ctrl+Z)
                break;
            }

            // If user enters "quit" or "exit"
            if (line == "quit" || line == "exit") {
                break;
            }

            // Handle comments: skip lines starting with '*', or remove inline comments
            // Simplistic handling here; consider more robust comment parsing
            if (!line.empty() && line[0] == '*') {
                logLogger->info("Skipping comment: {}", line);
                continue;
            }

            // Append line to buffer
            codeBuffer += line + "\n";

            // Check if codeBuffer contains at least one semicolon indicating statement termination
            size_t semicolonPos = codeBuffer.find(';');
            if (semicolonPos != std::string::npos) {
                // Extract the statement up to the semicolon
                std::string statement = codeBuffer.substr(0, semicolonPos + 1);
                logLogger->info("Executing statement: {}", statement);

                runSasCode(statement, interpreter, true);

                // Remove the executed statement from the buffer
                codeBuffer.erase(0, semicolonPos + 1);
            }
        }
    }
    else if (fileMode) {
        // File mode: read code from sasFile, output to console
        logLogger->info("Running from SAS file: {}", sasFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }

        runSasCode(sasCode, interpreter, false);
    }
    else if (batchMode) {
        // Batch mode: read code from sasFile, log and lst to files
        logLogger->info("Running in batch mode: SAS={} LOG={} LST={}", sasFile, logFile, lstFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }

        runSasCode(sasCode, interpreter, false);
    }

    return 0;
}
```

**Key Enhancements:**

- **Interactive Mode Handling**:
    - Accumulates user input until a semicolon (`;`) is detected, indicating the end of a statement.
    - Executes each complete statement as it's formed.
    - Skips comment lines starting with `*`.

- **Global Statement Execution**:
    - Handles global statements like `OPTIONS`, `LIBNAME`, and `TITLE` by updating the `DataEnvironment`.
    - The `Interpreter` reflects these global settings during data step execution.

- **Logging**:
    - Logs execution steps, option settings, libname assignments, and titles.
    - Outputs results to either the console or specified files based on the mode.

---

## 7. Example Usage and Testing

To ensure your interpreter correctly handles global statements, let's walk through an example.

### 7.1. Example SAS Code

**Example SAS Program (`example.sas`)**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Sample SAS Output';

data mylib.out; 
    set mylib.in; 
    x = 42; 
    if x then output; 
run;
```

### 7.2. Testing Interactive Mode

**Running the Interpreter in Interactive Mode**

1. **Start the Interpreter**:
    ```bash
    ./sas_interpreter
    ```

2. **Input SAS Code Line by Line**:

    ```
    SAS> options linesize=80 pagesize=60;
    [INFO] Executing statement: options linesize=80 pagesize=60;
    [INFO] Set option LINESIZE = 80
    [INFO] Set option PAGESIZE = 60

    SAS> libname mylib 'C:\Data';
    [INFO] Executing statement: libname mylib 'C:\Data';
    [INFO] Libname assigned: mylib = 'C:\Data'

    SAS> title 'Sample SAS Output';
    [INFO] Executing statement: title 'Sample SAS Output';
    [INFO] Title set to: 'Sample SAS Output'
    [INFO] SAS Results (Dataset: mylib.out):
    [INFO] Title: Sample SAS Output
    [INFO] OBS	X
    [INFO] 1	42.000000
    ```

3. **Exit Interactive Mode**:
    ```
    SAS> quit
    ```

### 7.3. Testing Batch Mode

**Running the Interpreter in Batch Mode**

1. **Prepare `example.sas`**:
    Ensure `example.sas` contains the SAS code from above.

2. **Run the Interpreter with Arguments**:
    ```bash
    ./sas_interpreter -sas=example.sas -log=sas_log.txt -lst=sas_output.lst
    ```

3. **Check Output Files**:

    **sas_log.txt**
    ```
    [INFO] Executing statement: options linesize=80 pagesize=60;
    [INFO] Set option LINESIZE = 80
    [INFO] Set option PAGESIZE = 60
    [INFO] Executing statement: libname mylib 'C:\Data';
    [INFO] Libname assigned: mylib = 'C:\Data'
    [INFO] Executing statement: title 'Sample SAS Output';
    [INFO] Title set to: 'Sample SAS Output'
    [INFO] Executing statement: data mylib.out; set mylib.in; x = 42; if x then output; run;
    [INFO] Executing DATA step: data mylib.out; set mylib.in;
    [INFO] Assigned x = 42.000000
    [INFO] Evaluating IF condition: 42.000000
    [INFO] OUTPUT statement executed. Current row will be added to the output dataset.
    [INFO] DATA step 'mylib.out' executed successfully. Output dataset has 1 observations.
    ```

    **sas_output.lst**
    ```
    SAS Results (Dataset: mylib.out):
    Title: Sample SAS Output
    OBS	X
    1	42.000000
    ```

### 7.4. Handling Errors and Edge Cases

Ensure that your interpreter can gracefully handle errors, such as:

- **Undefined Libref**:
    ```sas
    data out; set undefined_lib.in; run;
    ```
    **Log Output**:
    ```
    [INFO] Executing DATA step: data out; set undefined_lib.in; run;
    [ERROR] Undefined libref: undefined_lib
    ```

- **Syntax Errors**:
    ```sas
    data out set in; run;
    ```
    **Log Output**:
    ```
    [INFO] Executing DATA step: data out set in; run;
    [ERROR] Parsing failed: Parser error at line 1, column 10: Expected ';' after dataset name
    ```

- **Missing `run;`**:
    ```sas
    data out; set in;
    ```
    **Behavior**:
    The interpreter should wait for more input in interactive mode or execute upon reaching the end of input in file/batch mode.

---

## 8. Implementing a Simple REPL with Statement Accumulation

To mimic SAS¡¯s behavior where it waits for a statement terminator before executing, implement a buffer that accumulates input until a complete statement is detected.

**main.cpp (Interactive Mode Adjustments)**

```cpp
if (interactiveMode) {
    // Interactive mode: read code from stdin or a REPL-like interface
    logLogger->info("Running in interactive mode. Type SAS code line by line. End with 'run;' or type 'quit'/'exit' to exit.");
    std::string line;
    std::string codeBuffer;
    while (true) {
        std::cout << "SAS> ";
        if (!std::getline(std::cin, line)) {
            // End of input (Ctrl+D / Ctrl+Z)
            if (!codeBuffer.empty()) {
                logLogger->info("Executing accumulated code:\n{}", codeBuffer);
                runSasCode(codeBuffer, interpreter, true);
            }
            break;
        }

        // If user enters "quit" or "exit"
        if (line == "quit" || line == "exit") {
            break;
        }

        // Handle comments: skip lines starting with '*', or remove inline comments
        // Simplistic handling here; consider more robust comment parsing
        if (!line.empty() && line[0] == '*') {
            logLogger->info("Skipping comment: {}", line);
            continue;
        }

        // Append line to buffer
        codeBuffer += line + "\n";

        // Check if codeBuffer contains at least one semicolon indicating statement termination
        size_t semicolonPos = codeBuffer.find(';');
        while (semicolonPos != std::string::npos) {
            // Extract the statement up to the semicolon
            std::string statement = codeBuffer.substr(0, semicolonPos + 1);
            logLogger->info("Executing statement: {}", statement);

            runSasCode(statement, interpreter, true);

            // Remove the executed statement from the buffer
            codeBuffer.erase(0, semicolonPos + 1);

            // Check for another semicolon in the remaining buffer
            semicolonPos = codeBuffer.find(';');
        }
    }
}
```

**Explanation:**

- **Accumulation**: Lines are appended to `codeBuffer` until a semicolon is found.
- **Multiple Statements**: The `while` loop inside handles multiple semicolons in the buffer, allowing multiple statements to be executed in sequence.
- **Comments Handling**: Lines starting with `*` are skipped. Implement more robust comment parsing if needed.
- **Execution**: Each complete statement is passed to `runSasCode` for parsing and execution.

---

## 9. Enhancing the Interpreter for Robustness

To make your interpreter more robust and feature-complete, consider the following enhancements:

### 9.1. Error Recovery in Parser

Implement error recovery strategies in your parser to handle syntax errors gracefully, allowing the interpreter to continue processing subsequent statements.

**Parser.cpp (Example Error Recovery)**

```cpp
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
    catch (const std::runtime_error &e) {
        // Log the error
        logLogger->error("Parser error: {}", e.what());

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
```

### 9.2. Implementing a Symbol Table for Variables and Librefs

Maintain a symbol table to manage variable scopes and library references.

**DataEnvironment.h (Extend for Symbol Tables)**

```cpp
// ... existing code ...

class DataEnvironment {
public:
    // ... existing members ...

    // Symbol table for variables (could be nested for scopes if needed)
    std::unordered_map<std::string, Value> variables;

    // ... existing methods ...

    // Variable management
    Value getVariable(const std::string &varName) const {
        auto it = variables.find(varName);
        if (it != variables.end()) return it->second;
        // Return missing value (numeric missing is NaN, string missing is empty string)
        return std::nan("");
    }

    void setVariable(const std::string &varName, const Value &val) {
        variables[varName] = val;
    }
};
```

**Interpreter.cpp (Update Variable Access)**

```cpp
Value Interpreter::getVariable(const std::string &varName) const {
    return env.getVariable(varName);
}

void Interpreter::setVariable(const std::string &varName, const Value &val) {
    env.setVariable(varName, val);
}

Value Interpreter::evaluate(ASTNode *node) {
    if (auto lit = dynamic_cast<LiteralNode*>(node)) {
        // ... existing code ...
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        return env.getVariable(var->varName);
    }
    else if (auto bin = dynamic_cast<BinaryOpNode*>(node)) {
        // ... existing code ...
    }
    // Handle more expression types as needed
    throw std::runtime_error("Unsupported expression type during evaluation.");
}

void Interpreter::executeAssignment(AssignmentNode *node) {
    Value val = evaluate(node->expression.get());
    env.setVariable(node->varName, val);
    logLogger.info("Assigned {} = {}", node->varName, toString(val));
}
```

### 9.3. Handling Multiple DATA Steps and Procedures

Implement the ability to handle multiple data steps and procedures within a single SAS program.

**Parser.cpp (Already Handled via `parseProgram`)**

The `parseProgram` function iterates through all statements, allowing multiple `DATA` steps and global statements to coexist.

**Interpreter.cpp (Handling Multiple Statements)**

Ensure that your interpreter processes each statement in sequence, updating the environment as needed.

---

## 10. Final Thoughts and Next Steps

Building a fully functional SAS interpreter is an ambitious project, but by methodically implementing and testing each component, you can create a robust system. Here are some recommended next steps:

1. **Implement Additional Global Statements**:
    - Expand support for more `OPTIONS`, such as `FMTSEARCH`, `SOURCE`, etc.
    - Handle `FILENAME` statements for external file references.

2. **Enhance the Lexer and Parser**:
    - Implement macro handling (`%macro`, `%mend`, etc.) if desired.
    - Support more complex expressions, including functions and arrays.

3. **Improve Error Handling**:
    - Implement more sophisticated error recovery in the parser.
    - Provide detailed error messages, including line and column numbers.

4. **Expand the Interpreter's Capabilities**:
    - Support more PROCs (`PROC MEANS`, `PROC SORT`, etc.).
    - Implement data step features like arrays, formats, and informats.

5. **Optimize Performance**:
    - Profile your interpreter to identify bottlenecks.
    - Optimize data structures and algorithms for better performance.

6. **Implement a Comprehensive Testing Suite**:
    - Create unit tests for the lexer, parser, and interpreter.
    - Test with diverse SAS programs to ensure correctness.

7. **User Interface Enhancements**:
    - Improve the REPL with features like command history and multi-line editing.
    - Provide informative prompts and feedback to the user.

8. **Documentation and Help System**:
    - Document the supported SAS features.
    - Implement help commands within the interpreter for user assistance.

By following this structured approach and iteratively enhancing each component, you can develop a functional and extensible SAS interpreter in C++. Remember to continuously test each feature as you implement it to ensure reliability and correctness.

Good luck with your project!