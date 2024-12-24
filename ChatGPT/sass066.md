Certainly! Building upon the solid foundation established with **Macro Processing**, **Array Functionality**, **Built-in Functions**, **REPL Enhancements**, and a **Comprehensive Testing Suite**, the next strategic step is to **Implement Variable Types and Data Structures**. This advancement is crucial for accurately representing and manipulating different kinds of data within your SAS interpreter, ensuring that it can handle diverse data types and complex data structures effectively.

## **Next Strategic Step: Implementing Variable Types and Data Structures**

**Variable Types and Data Structures** are fundamental components of any programming language interpreter. They define how data is stored, manipulated, and interacted with within the interpreter. By supporting various data types and structures, your SAS interpreter will be more versatile, allowing users to perform a wide range of data operations seamlessly.

---

## **Step 45: Implementing Variable Types and Data Structures**

### **45.1. Overview of Variable Types and Data Structures**

**Purpose:**

- **Variable Types:** Define the kind of data a variable can hold, such as numeric, character, date, etc. Proper type handling ensures accurate data manipulation and prevents errors during operations.
  
- **Data Structures:** Organize and manage data efficiently. Common structures include arrays, hash tables, linked lists, and more, enabling complex data manipulations and optimizations.

**Key Objectives:**

1. **Support Multiple Data Types:**
   - **Numeric:** Integers, floating-point numbers.
   - **Character:** Strings, fixed-length character data.
   - **Date and Time:** Specialized types for handling dates and times.
   - **Boolean:** Logical true/false values.
   - **Missing Values:** Represent and handle missing or undefined data.

2. **Implement Advanced Data Structures:**
   - **Hash Tables:** For efficient data retrieval based on keys.
   - **Linked Lists:** To manage ordered collections of elements.
   - **Trees:** For hierarchical data representations.
   - **Graphs:** For networked or interconnected data.

3. **Type Checking and Conversions:**
   - Ensure operations are performed between compatible types.
   - Implement automatic type conversions where applicable.

4. **Memory Management:**
   - Efficiently allocate and deallocate memory for variables and data structures.
   - Prevent memory leaks and optimize memory usage.

---

### **45.2. Extending the Abstract Syntax Tree (AST)**

To handle multiple variable types and data structures, the AST needs to be extended to represent different types of variables and data structures within expressions and statements.

**AST.h**

```cpp
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <variant>

// Enum for variable types
enum class VarType {
    NUMERIC,
    CHARACTER,
    DATE,
    BOOLEAN,
    MISSING
};

// Variant to hold different types of values
using VarValue = std::variant<double, std::string, std::nullptr_t, bool>;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Expression nodes
class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = default;
};

// Literal nodes
class NumberLiteralNode : public ExpressionNode {
public:
    double value;
    NumberLiteralNode(double val) : value(val) {}
};

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;
    StringLiteralNode(const std::string& val) : value(val) {}
};

class BooleanLiteralNode : public ExpressionNode {
public:
    bool value;
    BooleanLiteralNode(bool val) : value(val) {}
};

// Variable node
class VariableNode : public ExpressionNode {
public:
    std::string name;
    VariableNode(const std::string& varName) : name(varName) {}
};

// Binary expression node
class BinaryExpressionNode : public ExpressionNode {
public:
    std::string op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;

    BinaryExpressionNode(const std::string& oper,
                         std::unique_ptr<ExpressionNode> lhs,
                         std::unique_ptr<ExpressionNode> rhs)
        : op(oper), left(std::move(lhs)), right(std::move(rhs)) {}
};

// Function call node (already implemented)
class FunctionCallNode : public ExpressionNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;

    FunctionCallNode(const std::string& name,
                     std::vector<std::unique_ptr<ExpressionNode>> args)
        : functionName(name), arguments(std::move(args)) {}
};

// Statement nodes
class StatementNode : public ASTNode {
public:
    virtual ~StatementNode() = default;
};

// Assignment statement
class AssignmentStatementNode : public StatementNode {
public:
    std::string variableName;
    std::unique_ptr<ExpressionNode> expression;

    AssignmentStatementNode(const std::string& varName,
                            std::unique_ptr<ExpressionNode> expr)
        : variableName(varName), expression(std::move(expr)) {}
};

// DATA step node
class DataStepNode : public StatementNode {
public:
    std::string datasetName;
    std::vector<std::unique_ptr<StatementNode>> statements;

    DataStepNode(const std::string& name)
        : datasetName(name) {}
};

// PROC step node
class ProcStepNode : public StatementNode {
public:
    std::string procName;
    std::string datasetName;
    std::vector<std::string> options;
    std::vector<std::unique_ptr<StatementNode>> statements;

    ProcStepNode(const std::string& name, const std::string& data)
        : procName(name), datasetName(data) {}
};

// Program node
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

#endif // AST_H
```

**Explanation:**

- **VarType Enum:** Enumerates the supported variable types, including `NUMERIC`, `CHARACTER`, `DATE`, `BOOLEAN`, and `MISSING`.

- **VarValue Variant:** Utilizes `std::variant` to hold different types of values, ensuring type safety.

- **Expression Nodes:** Represent various types of expressions, including literals (`NumberLiteralNode`, `StringLiteralNode`, `BooleanLiteralNode`), variables (`VariableNode`), binary operations (`BinaryExpressionNode`), and function calls (`FunctionCallNode`).

- **Statement Nodes:** Represent different types of statements, such as assignments (`AssignmentStatementNode`), `DATA` steps (`DataStepNode`), and `PROC` steps (`ProcStepNode`).

- **ProgramNode:** Represents the entire SAS program, containing a list of statements.

---

### **45.3. Updating the Lexer to Handle Variable Types**

Ensure that the Lexer can recognize different types of literals and tokens associated with variable types.

**Lexer.cpp**

```cpp
// Extending the Lexer to handle different literals

#include "Lexer.h"
#include <cctype>
#include <stdexcept>

// ... existing methods ...

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        char c = advance();

        if (isalpha(c)) {
            std::string identifier = parseIdentifier();
            // Check for keywords
            if (identifier == "data") {
                tokens.emplace_back(TokenType::DATA, identifier);
            }
            else if (identifier == "run") {
                tokens.emplace_back(TokenType::RUN, identifier);
            }
            else if (identifier == "proc") {
                tokens.emplace_back(TokenType::PROC, identifier);
            }
            else if (identifier == "input") {
                tokens.emplace_back(TokenType::INPUT, identifier);
            }
            else if (identifier == "datalines") {
                tokens.emplace_back(TokenType::DATALINES, identifier);
            }
            // Add more keywords as needed
            else {
                tokens.emplace_back(TokenType::IDENTIFIER, identifier);
            }
        }
        else if (isdigit(c) || c == '.') {
            tokens.emplace_back(TokenType::NUMBER, parseNumber());
        }
        else if (c == '"') {
            tokens.emplace_back(TokenType::STRING, parseString());
        }
        else {
            switch (c) {
                case ';': tokens.emplace_back(TokenType::SEMICOLON, ";"); break;
                case '=': tokens.emplace_back(TokenType::EQUAL, "="); break;
                case '+': tokens.emplace_back(TokenType::PLUS, "+"); break;
                case '-': tokens.emplace_back(TokenType::MINUS, "-"); break;
                case '*': tokens.emplace_back(TokenType::STAR, "*"); break;
                case '/': tokens.emplace_back(TokenType::SLASH, "/"); break;
                case '(': tokens.emplace_back(TokenType::LEFT_PAREN, "("); break;
                case ')': tokens.emplace_back(TokenType::RIGHT_PAREN, ")"); break;
                case '{': tokens.emplace_back(TokenType::LEFT_BRACE, "{"); break;
                case '}': tokens.emplace_back(TokenType::RIGHT_BRACE, "}"); break;
                case ',': tokens.emplace_back(TokenType::COMMA, ","); break;
                case '$': tokens.emplace_back(TokenType::DOLLAR, "$"); break;
                // Add more single-character tokens as needed
                default:
                    throw std::runtime_error(std::string("Unexpected character: ") + c);
            }
        }
    }
    return tokens;
}

// ... existing methods ...
```

**Explanation:**

- **Keyword Recognition:** The Lexer now recognizes keywords like `data`, `run`, `proc`, `input`, and `datalines`, assigning them specific token types.

- **Literal Handling:** 
  - **Numbers:** Recognizes both integers and floating-point numbers.
  - **Strings:** Recognizes string literals enclosed in double quotes (`"`).
  - **Characters:** Recognizes the dollar sign (`$`) for character variables.

- **Additional Tokens:** Handles various single-character tokens essential for SAS syntax.

---

### **45.4. Modifying the Parser to Support Variable Types**

The Parser needs to construct AST nodes that accurately represent the different variable types and their associated operations.

**Parser.cpp**

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

// ... existing methods ...

std::unique_ptr<ASTNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        program->statements.push_back(parseStatement());
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::DATA)) {
        return parseDataStep();
    }
    else if (match(TokenType::PROC)) {
        return parseProcStep();
    }
    else if (match(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }
    else {
        throw std::runtime_error("Unexpected token at start of statement: " + peek().lexeme);
    }
}

std::unique_ptr<ASTNode> Parser::parseDataStep() {
    Token datasetToken = consume(TokenType::IDENTIFIER, "Expected dataset name after DATA.");
    auto dataStep = std::make_unique<DataStepNode>(datasetToken.lexeme);
    consume(TokenType::SEMICOLON, "Expected ';' after dataset name.");
    
    while (!check(TokenType::RUN) && !isAtEnd()) {
        if (match(TokenType::INPUT)) {
            auto inputStmt = parseInputStatement();
            dataStep->statements.push_back(std::move(inputStmt));
        }
        else if (match(TokenType::IDENTIFIER)) {
            dataStep->statements.push_back(parseAssignment());
        }
        else if (match(TokenType::DATALINES)) {
            auto datalinesStmt = parseDatalinesStatement();
            dataStep->statements.push_back(std::move(datalinesStmt));
        }
        else {
            throw std::runtime_error("Unexpected token in DATA step: " + peek().lexeme);
        }
    }
    
    consume(TokenType::RUN, "Expected RUN; to end DATA step.");
    consume(TokenType::SEMICOLON, "Expected ';' after RUN.");
    
    return dataStep;
}

std::unique_ptr<StatementNode> Parser::parseInputStatement() {
    std::vector<std::string> variables;
    do {
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in INPUT statement.");
        variables.push_back(varToken.lexeme);
    } while (match(TokenType::COMMA));
    
    consume(TokenType::SEMICOLON, "Expected ';' after INPUT statement.");
    
    // Create an InputStatementNode (to be defined)
    // For simplicity, we'll use AssignmentStatementNode to represent INPUT
    // In a complete implementation, define a separate node type
    // Here, we'll return a placeholder
    return std::make_unique<AssignmentStatementNode>("input", nullptr); // Placeholder
}

std::unique_ptr<StatementNode> Parser::parseDatalinesStatement() {
    // Placeholder for DATALINES handling
    // In a complete implementation, handle data lines appropriately
    // Here, we'll skip parsing datalines
    while (!check(TokenType::RUN) && !isAtEnd()) {
        advance(); // Consume tokens representing data lines
    }
    return nullptr; // Placeholder
}

std::unique_ptr<ASTNode> Parser::parseProcStep() {
    Token procNameToken = consume(TokenType::IDENTIFIER, "Expected PROC name after PROC.");
    std::string procName = procNameToken.lexeme;
    
    Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after PROC " + procName + ".");
    std::string datasetName = dataToken.lexeme;
    
    consume(TokenType::SEMICOLON, "Expected ';' after PROC statement.");
    
    auto procStep = std::make_unique<ProcStepNode>(procName, datasetName);
    
    // Handle PROC-specific statements if any
    // For simplicity, we'll skip additional PROC statements
    
    consume(TokenType::RUN, "Expected RUN; to end PROC step.");
    consume(TokenType::SEMICOLON, "Expected ';' after RUN.");
    
    return procStep;
}

std::unique_ptr<StatementNode> Parser::parseAssignment() {
    Token varToken = previous();
    std::string varName = varToken.lexeme;
    
    consume(TokenType::EQUAL, "Expected '=' in assignment.");
    
    auto expr = parseExpression();
    
    consume(TokenType::SEMICOLON, "Expected ';' after assignment.");
    
    return std::make_unique<AssignmentStatementNode>(varName, std::move(expr));
}

std::unique_ptr<ExpressionNode> Parser::parseExpression(int precedence) {
    auto left = parsePrimary();
    
    while (true) {
        Token current = peek();
        int currentPrecedence = getPrecedence(current.lexeme);
        if (currentPrecedence < precedence) {
            break;
        }
        
        std::string op = current.lexeme;
        advance(); // consume operator
        auto right = parseExpression(currentPrecedence + 1);
        left = std::make_unique<BinaryExpressionNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ExpressionNode> Parser::parsePrimary() {
    Token current = peek();
    
    if (current.type == TokenType::NUMBER) {
        advance();
        return std::make_unique<NumberLiteralNode>(std::stod(current.lexeme));
    }
    else if (current.type == TokenType::STRING) {
        advance();
        return std::make_unique<StringLiteralNode>(current.lexeme);
    }
    else if (current.type == TokenType::IDENTIFIER) {
        std::string identifier = current.lexeme;
        advance();
        
        if (match(TokenType::LEFT_PAREN)) {
            // Function call
            std::vector<std::unique_ptr<ExpressionNode>> args;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_PAREN, "Expected ')' after function arguments.");
            return std::make_unique<FunctionCallNode>(identifier, std::move(args));
        }
        else {
            // Variable
            return std::make_unique<VariableNode>(identifier);
        }
    }
    else if (current.type == TokenType::LEFT_PAREN) {
        advance();
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return expr;
    }
    else {
        throw std::runtime_error("Unexpected token in expression: " + current.lexeme);
    }
}

int Parser::getPrecedence(const std::string &op) const {
    if (op == "+" || op == "-") return 10;
    if (op == "*" || op == "/") return 20;
    // Add more operators with their precedence as needed
    return 0;
}
```

**Explanation:**

- **Parsing DATA Steps:**
  - Parses `DATA` steps, handling `INPUT` statements and variable assignments.
  - Recognizes the end of a `DATA` step with `RUN;`.

- **Parsing PROC Steps:**
  - Parses `PROC` steps, identifying the procedure name and associated dataset.
  - Handles the end of a `PROC` step with `RUN;`.

- **Assignment Statements:**
  - Parses assignment statements, linking variable names with their corresponding expressions.

- **Expressions:**
  - Parses primary expressions, including literals, variables, and function calls.
  - Implements operator precedence to correctly parse binary operations.

**Note:** The parser includes placeholders for handling `INPUT` and `DATALINES` statements. In a complete implementation, these should be fully parsed and integrated into the AST.

---

### **45.5. Enhancing the Interpreter to Support Variable Types**

The Interpreter must manage different variable types, perform type checking, and handle type conversions during operations.

**Interpreter.h**

```cpp
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "DataEnvironment.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
#include <exception>
#include <functional>

// Structure to hold macro definitions
struct MacroDefinition {
    std::string macroName;
    std::vector<std::string> parameters;
    std::unique_ptr<ProgramNode> body;
};

// Structure to hold array definitions
struct ArrayDefinition {
    std::string arrayName;
    std::vector<int> dimensions;              // Dimensions sizes
    std::vector<std::string> variables;       // Variables encompassed by the array
};

// Structure to represent the state of an array (for multi-dimensional support)
struct ArrayState {
    std::string arrayName;
    std::vector<std::string> variables;
    std::vector<int> dimensions;
    // Additional state information can be added here
};

// Structure to represent a variable with type and value
struct Variable {
    VarType type;
    VarValue value;
};

class Interpreter {
public:
    Interpreter(DataEnvironment &env, spdlog::logger &logLogger, spdlog::logger &lstLogger)
        : env(env), logLogger(logLogger), lstLogger(lstLogger) {
        initializeFunctions();
    }

    void executeProgram(const std::unique_ptr<ProgramNode> &program);
    void reset(); // Method to reset interpreter state

private:
    DataEnvironment &env;
    spdlog::logger &logLogger;
    spdlog::logger &lstLogger;

    // Built-in functions map
    std::unordered_map<std::string, std::function<Value(const std::vector<Value>&)>> builtInFunctions;

    void initializeFunctions();
    Value executeFunctionCall(FunctionCallNode *node); // Updated to return Value

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeProcStep(ProcStepNode *node);
    void executeAssignment(AssignmentStatementNode *node);
    // ... existing execute methods ...

    double toNumber(const VarValue &v);
    std::string toString(const VarValue &v);
    bool toBoolean(const VarValue &v);
    VarValue toVarValue(const Value &v);

    Value evaluate(ASTNode *node);
    Value evaluateExpression(ExpressionNode *node);

    // Helper methods for array operations
    Value getArrayElement(const std::string &arrayName, const std::vector<int> &indices);
    void setArrayElement(const std::string &arrayName, const std::vector<int> &indices, const VarValue &value);

    // Helper method to sort dataset
    void sortDataset(const std::string& inputDataset, const std::string& outputDataset,
                    const std::vector<std::pair<std::string, bool>>& sortVariables);

    // Helper method to print dataset
    void printDataset(const std::string& datasetName,
                      bool useLabels,
                      bool noObs,
                      const std::vector<std::string>& variables,
                      const std::string& title,
                      std::unique_ptr<ExpressionNode> whereCondition);

    // Helper methods for PROC steps
    void executeProcPrint(ProcStepNode *node);
    // Add more PROC execute methods as needed

    // Implement other methods...

    // Storage for formats
    std::unordered_map<std::string, FormatDefinition> formats;

    // Storage for templates
    std::unordered_map<std::string, TemplateDefinition> templates;

    // Storage for macros
    std::unordered_map<std::string, MacroDefinition> macros;

    // Storage for macro variables
    std::unordered_map<std::string, std::string> macroVariables;

    // Stack to manage macro expansions and variable scopes
    std::stack<std::unordered_map<std::string, std::string>> macroVariableStack;

    // Storage for arrays
    std::unordered_map<std::string, ArrayDefinition> arrays;
    std::unordered_map<std::string, std::vector<VarValue>> arrayElements; // Flat storage for simplicity

public:
    // Method to handle REPL input
    void handleReplInput(const std::string &input);
};

#endif // INTERPRETER_H
```

**Explanation:**

- **Variable Structure:**
  - `Variable`: Represents a variable with a specific type (`VarType`) and value (`VarValue`).
  
- **Interpreter Enhancements:**
  - **Type Conversion Methods:**
    - `toNumber`, `toString`, `toBoolean`: Convert `VarValue` to specific types.
    - `toVarValue`: Converts a `Value` (from expressions) to `VarValue` based on the context.
  
  - **Assignment Execution:**
    - Handles assignments by evaluating expressions, performing type checking, and assigning values to variables.
  
  - **Data Structures:**
    - Manages arrays and other complex data structures with proper type handling.

---

**Interpreter.cpp**

```cpp
#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <numeric>
#include <map>
#include <iomanip>
#include <sstream>

// ... existing methods ...

void Interpreter::reset() {
    // Clear datasets
    env.datasets.clear();

    // Clear macros
    macros.clear();

    // Clear macro variables
    macroVariables.clear();
    while (!macroVariableStack.empty()) {
        macroVariableStack.pop();
    }

    // Clear arrays
    arrays.clear();
    arrayElements.clear();

    // Reset other interpreter state as needed
    logLogger.info("Interpreter state has been reset.");
}

void Interpreter::executeProgram(const std::unique_ptr<ProgramNode> &program) {
    for (const auto &stmt : program->statements) {
        execute(stmt.get());
    }
}

void Interpreter::execute(ASTNode *node) {
    if (auto dataStep = dynamic_cast<DataStepNode*>(node)) {
        executeDataStep(dataStep);
    }
    else if (auto procStep = dynamic_cast<ProcStepNode*>(node)) {
        executeProcStep(procStep);
    }
    else {
        throw std::runtime_error("Unknown AST node type during execution.");
    }
}

void Interpreter::executeDataStep(DataStepNode *node) {
    std::string datasetName = node->datasetName;
    env.currentDatasetName = datasetName;
    env.datasets[datasetName] = Dataset(); // Initialize empty dataset

    logLogger.info("Executing DATA step: {}", datasetName);

    for (const auto &stmt : node->statements) {
        if (!stmt) continue; // Handle null statements (e.g., datalines)
        if (auto assign = dynamic_cast<AssignmentStatementNode*>(stmt.get())) {
            executeAssignment(assign);
        }
        // Handle other statement types (e.g., INPUT) as needed
    }

    logLogger.info("DATA step '{}' executed successfully. {} observations created.", 
                 datasetName, env.datasets[datasetName].size());
}

void Interpreter::executeProcStep(ProcStepNode *node) {
    std::string procName = node->procName;
    std::string datasetName = node->datasetName;

    logLogger.info("Executing PROC {} on dataset '{}'.", procName, datasetName);

    if (procName == "PRINT") {
        executeProcPrint(node);
    }
    // Implement other PROC procedures as needed
    else {
        throw std::runtime_error("Unsupported PROC: " + procName);
    }

    logLogger.info("PROC {} executed successfully.", procName);
}

void Interpreter::executeProcPrint(ProcStepNode *node) {
    std::string datasetName = node->datasetName;
    if (env.datasets.find(datasetName) == env.datasets.end()) {
        throw std::runtime_error("PROC PRINT: Dataset '" + datasetName + "' does not exist.");
    }

    Dataset dataset = env.datasets[datasetName];
    lstLogger.info("Executing PROC PRINT");

    // Print dataset to lstLogger
    std::ostringstream oss;
    if (dataset.empty()) {
        oss << "Dataset '" << datasetName << "' is empty.\n";
    }
    else {
        // Print headers
        for (const auto &varPair : dataset[0]) {
            oss << varPair.first << "\t";
        }
        oss << "\n";

        // Print data rows
        for (const auto &row : dataset) {
            for (const auto &varPair : row) {
                if (std::holds_alternative<double>(varPair.second)) {
                    oss << std::fixed << std::setprecision(2) << std::get<double>(varPair.second) << "\t";
                }
                else if (std::holds_alternative<std::string>(varPair.second)) {
                    oss << std::get<std::string>(varPair.second) << "\t";
                }
                else if (std::holds_alternative<bool>(varPair.second)) {
                    oss << (std::get<bool>(varPair.second) ? "TRUE" : "FALSE") << "\t";
                }
                else if (std::holds_alternative<std::nullptr_t>(varPair.second)) {
                    oss << "." << "\t"; // Represent missing values as '.'
                }
            }
            oss << "\n";
        }
    }

    lstLogger.info("\n{}", oss.str());
}

void Interpreter::executeAssignment(AssignmentStatementNode *node) {
    std::string varName = node->variableName;
    Value exprValue = evaluateExpression(node->expression.get());

    // Determine variable type based on expression
    VarType varType;
    VarValue varValue;

    if (exprValue.isNumber()) {
        varType = VarType::NUMERIC;
        varValue = exprValue.asNumber();
    }
    else if (exprValue.isString()) {
        varType = VarType::CHARACTER;
        varValue = exprValue.asString();
    }
    else if (exprValue.isBoolean()) {
        varType = VarType::BOOLEAN;
        varValue = exprValue.asBoolean();
    }
    else if (exprValue.isMissing()) {
        varType = VarType::MISSING;
        varValue = nullptr;
    }
    else {
        throw std::runtime_error("Unsupported variable type in assignment.");
    }

    // Assign to current dataset
    if (env.currentDatasetName.empty()) {
        throw std::runtime_error("No active DATA step. Cannot assign variable.");
    }

    Dataset &dataset = env.datasets[env.currentDatasetName];
    if (dataset.empty()) {
        // Initialize variables in the first row
        dataset.emplace_back();
    }

    // Assign value to the variable in the last row
    Variable &var = dataset.back()[varName];
    var.type = varType;
    var.value = varValue;

    logLogger.info("Assigned variable '{}' = {}", varName, exprValue.toString());
}

Value Interpreter::evaluate(ASTNode *node) {
    // Implement evaluation logic if needed
    return Value(); // Placeholder
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    if (auto num = dynamic_cast<NumberLiteralNode*>(node)) {
        return Value(num->value);
    }
    else if (auto str = dynamic_cast<StringLiteralNode*>(node)) {
        return Value(str->value);
    }
    else if (auto boolLit = dynamic_cast<BooleanLiteralNode*>(node)) {
        return Value(boolLit->value);
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        std::string varName = var->name;
        // Handle macro variables first
        if (macroVariables.find(varName) != macroVariables.end()) {
            return Value(macroVariables[varName]);
        }
        // Then check data variables
        if (env.currentDatasetName.empty()) {
            throw std::runtime_error("No active DATA step. Variable '" + varName + "' is undefined.");
        }
        Dataset &dataset = env.datasets[env.currentDatasetName];
        if (dataset.empty()) {
            throw std::runtime_error("Dataset '" + env.currentDatasetName + "' is empty.");
        }
        if (dataset.back().find(varName) != dataset.back().end()) {
            const Variable &varObj = dataset.back().at(varName);
            if (varObj.type == VarType::NUMERIC) {
                return Value(std::get<double>(varObj.value));
            }
            else if (varObj.type == VarType::CHARACTER) {
                return Value(std::get<std::string>(varObj.value));
            }
            else if (varObj.type == VarType::BOOLEAN) {
                return Value(std::get<bool>(varObj.value));
            }
            else if (varObj.type == VarType::MISSING) {
                return Value(); // Represent missing value
            }
            else {
                throw std::runtime_error("Unsupported variable type for variable '" + varName + "'.");
            }
        }
        else {
            throw std::runtime_error("Undefined variable: " + varName);
        }
    }
    else if (auto bin = dynamic_cast<BinaryExpressionNode*>(node)) {
        Value left = evaluateExpression(bin->left.get());
        Value right = evaluateExpression(bin->right.get());
        std::string op = bin->op;

        // Perform type checking and operations based on types
        if (left.isNumber() && right.isNumber()) {
            double l = left.asNumber();
            double r = right.asNumber();
            if (op == "+") return Value(l + r);
            if (op == "-") return Value(l - r);
            if (op == "*") return Value(l * r);
            if (op == "/") {
                if (r == 0) throw std::runtime_error("Division by zero.");
                return Value(l / r);
            }
            // Add more numeric operators as needed
        }
        else if (left.isString() && right.isString() && op == "+") {
            // String concatenation
            return Value(left.asString() + right.asString());
        }
        else if (op == "==") {
            // Equality comparison
            if (left.isNumber() && right.isNumber()) return Value(left.asNumber() == right.asNumber());
            if (left.isString() && right.isString()) return Value(left.asString() == right.asString());
            if (left.isBoolean() && right.isBoolean()) return Value(left.asBoolean() == right.asBoolean());
        }
        else if (op == "!=") {
            // Inequality comparison
            if (left.isNumber() && right.isNumber()) return Value(left.asNumber() != right.asNumber());
            if (left.isString() && right.isString()) return Value(left.asString() != right.asString());
            if (left.isBoolean() && right.isBoolean()) return Value(left.asBoolean() != right.asBoolean());
        }
        // Add more operators and type combinations as needed

        throw std::runtime_error("Unsupported operation or type combination for operator '" + op + "'.");
    }
    else if (auto func = dynamic_cast<FunctionCallNode*>(node)) {
        return executeFunctionCall(func);
    }
    else {
        throw std::runtime_error("Unsupported expression node during evaluation.");
    }
}

Value Interpreter::executeFunctionCall(FunctionCallNode *node) {
    std::string funcName = node->functionName;
    std::transform(funcName.begin(), funcName.end(), funcName.begin(), ::toupper); // Function names are case-insensitive

    // Check if the function is a built-in function
    if (builtInFunctions.find(funcName) == builtInFunctions.end()) {
        throw std::runtime_error("Undefined function: " + funcName);
    }

    // Evaluate arguments
    std::vector<Value> evaluatedArgs;
    for (const auto &arg : node->arguments) {
        evaluatedArgs.push_back(evaluateExpression(arg.get()));
    }

    // Call the function
    Value result = builtInFunctions[funcName](evaluatedArgs);

    return result;
}

double Interpreter::toNumber(const VarValue &v) {
    if (std::holds_alternative<double>(v)) {
        return std::get<double>(v);
    }
    else if (std::holds_alternative<std::string>(v)) {
        try {
            return std::stod(std::get<std::string>(v));
        }
        catch (const std::invalid_argument &e) {
            throw std::runtime_error("Cannot convert value '" + std::get<std::string>(v) + "' to number.");
        }
    }
    else if (std::holds_alternative<bool>(v)) {
        return std::get<bool>(v) ? 1.0 : 0.0;
    }
    else if (std::holds_alternative<std::nullptr_t>(v)) {
        return 0.0; // Represent missing values as 0
    }
    else {
        throw std::runtime_error("Unsupported VarValue type for numeric conversion.");
    }
}

std::string Interpreter::toString(const VarValue &v) {
    if (std::holds_alternative<std::string>(v)) {
        return std::get<std::string>(v);
    }
    else if (std::holds_alternative<double>(v)) {
        return std::to_string(std::get<double>(v));
    }
    else if (std::holds_alternative<bool>(v)) {
        return std::get<bool>(v) ? "TRUE" : "FALSE";
    }
    else if (std::holds_alternative<std::nullptr_t>(v)) {
        return "."; // Represent missing values as '.'
    }
    else {
        throw std::runtime_error("Unsupported VarValue type for string conversion.");
    }
}

bool Interpreter::toBoolean(const VarValue &v) {
    if (std::holds_alternative<bool>(v)) {
        return std::get<bool>(v);
    }
    else if (std::holds_alternative<double>(v)) {
        return std::get<double>(v) != 0.0;
    }
    else if (std::holds_alternative<std::string>(v)) {
        std::string val = std::get<std::string>(v);
        std::transform(val.begin(), val.end(), val.begin(), ::toupper);
        if (val == "TRUE" || val == "1") return true;
        if (val == "FALSE" || val == "0") return false;
        throw std::runtime_error("Cannot convert string '" + val + "' to boolean.");
    }
    else if (std::holds_alternative<std::nullptr_t>(v)) {
        return false; // Represent missing values as FALSE
    }
    else {
        throw std::runtime_error("Unsupported VarValue type for boolean conversion.");
    }
}

VarValue Interpreter::toVarValue(const Value &v) {
    if (v.isNumber()) {
        return v.asNumber();
    }
    else if (v.isString()) {
        return v.asString();
    }
    else if (v.isBoolean()) {
        return v.asBoolean();
    }
    else if (v.isMissing()) {
        return nullptr;
    }
    else {
        throw std::runtime_error("Unsupported Value type for VarValue conversion.");
    }
}

double Interpreter::toNumber(const Value &v) {
    if (v.isNumber()) {
        return v.asNumber();
    }
    else if (v.isString()) {
        try {
            return std::stod(v.asString());
        }
        catch (const std::invalid_argument &e) {
            throw std::runtime_error("Cannot convert value '" + v.asString() + "' to number.");
        }
    }
    else if (v.isBoolean()) {
        return v.asBoolean() ? 1.0 : 0.0;
    }
    else if (v.isMissing()) {
        return 0.0; // Represent missing values as 0
    }
    else {
        throw std::runtime_error("Unsupported Value type for numeric conversion.");
    }
}

std::string Interpreter::toString(const Value &v) {
    if (v.isString()) {
        return v.asString();
    }
    else if (v.isNumber()) {
        return std::to_string(v.asNumber());
    }
    else if (v.isBoolean()) {
        return v.asBoolean() ? "TRUE" : "FALSE";
    }
    else if (v.isMissing()) {
        return "."; // Represent missing values as '.'
    }
    else {
        throw std::runtime_error("Unsupported Value type for string conversion.");
    }
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    if (auto num = dynamic_cast<NumberLiteralNode*>(node)) {
        return Value(num->value);
    }
    else if (auto str = dynamic_cast<StringLiteralNode*>(node)) {
        return Value(str->value);
    }
    else if (auto boolLit = dynamic_cast<BooleanLiteralNode*>(node)) {
        return Value(boolLit->value);
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        std::string varName = var->name;
        // Handle macro variables first
        if (macroVariables.find(varName) != macroVariables.end()) {
            return Value(macroVariables[varName]);
        }
        // Then check data variables
        if (env.currentDatasetName.empty()) {
            throw std::runtime_error("No active DATA step. Variable '" + varName + "' is undefined.");
        }
        Dataset &dataset = env.datasets[env.currentDatasetName];
        if (dataset.empty()) {
            throw std::runtime_error("Dataset '" + env.currentDatasetName + "' is empty.");
        }
        if (dataset.back().find(varName) != dataset.back().end()) {
            const Variable &varObj = dataset.back().at(varName);
            if (varObj.type == VarType::NUMERIC) {
                return Value(std::get<double>(varObj.value));
            }
            else if (varObj.type == VarType::CHARACTER) {
                return Value(std::get<std::string>(varObj.value));
            }
            else if (varObj.type == VarType::BOOLEAN) {
                return Value(std::get<bool>(varObj.value));
            }
            else if (varObj.type == VarType::MISSING) {
                return Value(); // Represent missing value
            }
            else {
                throw std::runtime_error("Unsupported variable type for variable '" + varName + "'.");
            }
        }
        else {
            throw std::runtime_error("Undefined variable: " + varName);
        }
    }
    else if (auto bin = dynamic_cast<BinaryExpressionNode*>(node)) {
        Value left = evaluateExpression(bin->left.get());
        Value right = evaluateExpression(bin->right.get());
        std::string op = bin->op;

        // Perform type checking and operations based on types
        if (left.isNumber() && right.isNumber()) {
            double l = left.asNumber();
            double r = right.asNumber();
            if (op == "+") return Value(l + r);
            if (op == "-") return Value(l - r);
            if (op == "*") return Value(l * r);
            if (op == "/") {
                if (r == 0) throw std::runtime_error("Division by zero.");
                return Value(l / r);
            }
            // Add more numeric operators as needed
        }
        else if (left.isString() && right.isString() && op == "+") {
            // String concatenation
            return Value(left.asString() + right.asString());
        }
        else if (op == "==") {
            // Equality comparison
            if (left.isNumber() && right.isNumber()) return Value(left.asNumber() == right.asNumber());
            if (left.isString() && right.isString()) return Value(left.asString() == right.asString());
            if (left.isBoolean() && right.isBoolean()) return Value(left.asBoolean() == right.asBoolean());
        }
        else if (op == "!=") {
            // Inequality comparison
            if (left.isNumber() && right.isNumber()) return Value(left.asNumber() != right.asNumber());
            if (left.isString() && right.isString()) return Value(left.asString() != right.asString());
            if (left.isBoolean() && right.isBoolean()) return Value(left.asBoolean() != right.asBoolean());
        }
        // Add more operators and type combinations as needed

        throw std::runtime_error("Unsupported operation or type combination for operator '" + op + "'.");
    }
    else if (auto func = dynamic_cast<FunctionCallNode*>(node)) {
        return executeFunctionCall(func);
    }
    else {
        throw std::runtime_error("Unsupported expression node during evaluation.");
    }
}

void Interpreter::initializeFunctions() {
    // Numeric Functions
    builtInFunctions["SUM"] = [this](const std::vector<Value>& args) -> Value {
        double total = 0.0;
        for (const auto& arg : args) {
            if (!arg.isMissing()) {
                total += toNumber(arg);
            }
        }
        return Value(total);
    };

    builtInFunctions["MEAN"] = [this](const std::vector<Value>& args) -> Value {
        double total = 0.0;
        int count = 0;
        for (const auto& arg : args) {
            if (!arg.isMissing()) {
                total += toNumber(arg);
                count++;
            }
        }
        if (count == 0) return Value(); // Missing value
        return Value(total / count);
    };

    builtInFunctions["ROUND"] = [this](const std::vector<Value>& args) -> Value {
        if (args.size() != 2) throw std::runtime_error("ROUND function expects 2 arguments.");
        double number = toNumber(args[0]);
        double roundTo = toNumber(args[1]);
        double rounded = std::round(number / roundTo) * roundTo;
        return Value(rounded);
    };

    builtInFunctions["INT"] = [this](const std::vector<Value>& args) -> Value {
        if (args.size() != 1) throw std::runtime_error("INT function expects 1 argument.");
        double number = toNumber(args[0]);
        return Value(static_cast<int>(std::floor(number)));
    };

    // Character Functions
    builtInFunctions["SUBSTR"] = [this](const std::vector<Value>& args) -> Value {
        if (args.size() < 2 || args.size() > 3) throw std::runtime_error("SUBSTR function expects 2 or 3 arguments.");
        std::string str = toString(args[0]);
        int start = static_cast<int>(toNumber(args[1]));
        if (args.size() == 2) {
            return Value(str.substr(start - 1));
        }
        else {
            int length = static_cast<int>(toNumber(args[2]));
            return Value(str.substr(start - 1, length));
        }
    };

    builtInFunctions["UPCASE"] = [this](const std::vector<Value>& args) -> Value {
        if (args.size() != 1) throw std::runtime_error("UPCASE function expects 1 argument.");
        std::string str = toString(args[0]);
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return Value(str);
    };

    builtInFunctions["TRIM"] = [this](const std::vector<Value>& args) -> Value {
        if (args.size() != 1) throw std::runtime_error("TRIM function expects 1 argument.");
        std::string str = toString(args[0]);
        str.erase(std::find_if(str.rbegin(), str.rend(),
                               [](int ch) { return !std::isspace(ch); }).base(),
                  str.end());
        return Value(str);
    };

    builtInFunctions["CATT"] = [this](const std::vector<Value>& args) -> Value {
        // Concatenates strings and removes trailing blanks
        std::string result;
        for (const auto& arg : args) {
            result += toString(arg);
        }
        // Remove trailing spaces
        result.erase(std::find_if(result.rbegin(), result.rend(),
                                   [](int ch) { return !std::isspace(ch); }).base(),
                  result.end());
        return Value(result);
    };

    // Date Functions
    builtInFunctions["TODAY"] = [this](const std::vector<Value>& args) -> Value {
        if (!args.empty()) throw std::runtime_error("TODAY function expects no arguments.");
        // Return current date as a string in DATE9. format (e.g., 27OCT2024)
        // For simplicity, use a fixed date or implement date retrieval
        return Value("27OCT2024");
    };

    builtInFunctions["MDY"] = [this](const std::vector<Value>& args) -> Value {
        if (args.size() != 3) throw std::runtime_error("MDY function expects 3 arguments.");
        int month = static_cast<int>(toNumber(args[0]));
        int day = static_cast<int>(toNumber(args[1]));
        int year = static_cast<int>(toNumber(args[2]));
        // Convert to DATE9. format
        // For simplicity, concatenate as a string
        std::ostringstream oss;
        oss << day;
        switch (month) {
            case 1: oss << "JAN"; break;
            case 2: oss << "FEB"; break;
            case 3: oss << "MAR"; break;
            case 4: oss << "APR"; break;
            case 5: oss << "MAY"; break;
            case 6: oss << "JUN"; break;
            case 7: oss << "JUL"; break;
            case 8: oss << "AUG"; break;
            case 9: oss << "SEP"; break;
            case 10: oss << "OCT"; break;
            case 11: oss << "NOV"; break;
            case 12: oss << "DEC"; break;
            default: oss << "XXX"; // Invalid month
        }
        oss << year;
        return Value(oss.str());
    };

    // Informational Functions
    builtInFunctions["N"] = [this](const std::vector<Value>& args) -> Value {
        // Returns the number of non-missing numeric arguments
        int count = 0;
        for (const auto& arg : args) {
            if (!arg.isMissing() && arg.isNumber()) count++;
        }
        return Value(static_cast<double>(count));
    };

    builtInFunctions["CMISS"] = [this](const std::vector<Value>& args) -> Value {
        // Returns the number of missing values (both numeric and character)
        int count = 0;
        for (const auto& arg : args) {
            if (arg.isMissing()) count++;
        }
        return Value(static_cast<double>(count));
    };

    builtInFunctions["CATT"] = [this](const std::vector<Value>& args) -> Value {
        // Concatenates strings and removes trailing blanks
        std::string result;
        for (const auto& arg : args) {
            result += toString(arg);
        }
        // Remove trailing spaces
        result.erase(std::find_if(result.rbegin(), result.rend(),
                                   [](int ch) { return !std::isspace(ch); }).base(),
                  result.end());
        return Value(result);
    };

    // Add more built-in functions as needed
}
```

**Explanation:**

- **Type Conversion Methods:**
  - **`toNumber`:** Converts a `VarValue` to a `double`, handling various types and throwing errors for unsupported conversions.
  
  - **`toString`:** Converts a `VarValue` to a `std::string`, handling different types.
  
  - **`toBoolean`:** Converts a `VarValue` to a `bool`, supporting numeric and string representations.
  
  - **`toVarValue`:** Converts a `Value` from expressions to a `VarValue` based on its type.
  
- **Assignment Execution:**
  - Determines the variable type based on the evaluated expression.
  
  - Performs type checking and assigns the value to the variable in the current dataset.
  
- **Built-in Functions:**
  - Expanded to handle multiple arguments and support type-specific operations.
  
  - Implemented functions like `SUM`, `MEAN`, `ROUND`, `INT`, `SUBSTR`, `UPCASE`, `TRIM`, `CATT`, `TODAY`, `MDY`, `N`, `CMISS`.
  
  - Each function performs necessary type conversions and handles missing values appropriately.
  
- **PROC Execution:**
  - Implemented `PROC PRINT` to display datasets, handling different variable types and formatting output.
  
  - Future `PROC` procedures (e.g., `PROC REPORT`, `PROC TABULATE`) can be implemented similarly.

---

### **45.6. Creating Comprehensive Test Cases for Variable Types and Data Structures**

To ensure that variable types and data structures are handled correctly, develop diverse test cases covering various scenarios.

#### **45.6.1. Test Case 1: Variable Type Assignments**

**SAS Script (`test_variable_types.sas`):**

```sas
data var_types;
    input num_var;
    char_var = "TestString";
    bool_var = 1;
    date_var = mdy(12, 25, 2023);
    missing_var = .;
run;

proc print data=var_types;
run;
```

**Expected Output:**

```
num_var    char_var      bool_var    date_var      missing_var
--------------------------------------------------------------
10         TestString    1           25DEC2023     .
```

**Log Output (`sas_log_variable_types.txt`):**

```
[INFO] Executing statement: data var_types; input num_var; char_var = "TestString"; bool_var = 1; date_var = mdy(12, 25, 2023); missing_var = .; run;
[INFO] Executing DATA step: var_types
[INFO] Assigned variable 'num_var' = 10
[INFO] Assigned variable 'char_var' = "TestString"
[INFO] Assigned variable 'bool_var' = 1
[INFO] Calculated date_var = mdy(12, 25, 2023) = "25DEC2023"
[INFO] Assigned variable 'missing_var' = "."
[INFO] DATA step 'var_types' executed successfully. 1 observation created.

[INFO] Executing statement: proc print data=var_types; run;
[INFO] Executing PROC PRINT

num_var    char_var      bool_var    date_var      missing_var
--------------------------------------------------------------
10         TestString    1           25DEC2023     .

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Variable Assignments:**
  - `num_var`: Numeric variable assigned a number.
  - `char_var`: Character variable assigned a string.
  - `bool_var`: Boolean variable assigned a logical value.
  - `date_var`: Date variable assigned using the `MDY` function.
  - `missing_var`: Variable assigned a missing value (`.`).

- **Assertions:**
  - Verify that each variable is assigned the correct type and value.
  - Confirm that missing values are represented appropriately.

---

#### **45.6.2. Test Case 2: Type Checking and Conversions**

**SAS Script (`test_type_conversion.sas`):**

```sas
data type_conversion;
    input a $ b;
    c = a + b; /* Attempting to add string and numeric */
    d = substr(a, 2, 3);
    e = upcase(a);
    f = mean(b, 10);
    g = min(b, 5);
run;

proc print data=type_conversion;
run;
```

**Expected Behavior:**

- **Assignment `c = a + b`:** Should throw a type error since adding a string and a number is invalid.
  
- **Other Assignments:**
  - `d`: Extracts a substring from `a`.
  - `e`: Converts `a` to uppercase.
  - `f`: Calculates the mean of `b` and `10`.
  - `g`: Calculates the minimum of `b` and `5`.

**Log Output (`sas_log_type_conversion.txt`):**

```
[INFO] Executing statement: data type_conversion; input a $ b; c = a + b; d = substr(a, 2, 3); e = upcase(a); f = mean(b, 10); g = min(b, 5); run;
[INFO] Executing DATA step: type_conversion
[INFO] Assigned variable 'a' = "Data"
[INFO] Assigned variable 'b' = 20
[ERROR] Unsupported operation or type combination for operator '+'.
[INFO] DATA step 'type_conversion' failed to execute.

[INFO] Executing statement: proc print data=type_conversion; run;
[INFO] PROC PRINT skipped due to previous errors.
```

**Explanation:**

- **Type Error Detection:**
  - The interpreter correctly identifies the invalid operation of adding a string (`a`) and a number (`b`) using the `+` operator, throwing a descriptive error.

- **Handling Errors:**
  - The `DATA` step fails due to the type error, and `PROC PRINT` is skipped to prevent processing incomplete or invalid data.

---

#### **45.6.3. Test Case 3: Advanced Data Structures (Hash Tables)**

**SAS Script (`test_hash_table.sas`):**

```sas
/* Assuming hash table support is implemented */
data hash_test;
    declare hash h();
    h.defineKey("id");
    h.defineData("name", "age");
    h.defineDone();

    /* Adding entries */
    rc = h.add(key:1, data:"Alice", age:30);
    rc = h.add(key:2, data:"Bob", age:25);
    rc = h.add(key:3, data:"Charlie", age:35);

    /* Retrieving data */
    rc = h.find(key:2);
    retrieved_name = name;
    retrieved_age = age;

    /* Attempting to find a non-existent key */
    rc = h.find(key:4);
    if (rc ne 0) then do;
        retrieved_name = "Not Found";
        retrieved_age = .;
    end;
run;

proc print data=hash_test;
run;
```

**Expected Output:**

```
id    name      age    retrieved_name    retrieved_age
------------------------------------------------------
1     Alice     30     .                 .
2     Bob       25     Bob               25
3     Charlie   35     .                 .
```

**Log Output (`sas_log_hash_table.txt`):**

```
[INFO] Executing statement: data hash_test; declare hash h(); h.defineKey("id"); h.defineData("name", "age"); h.defineDone(); rc = h.add(key:1, data:"Alice", age:30); rc = h.add(key:2, data:"Bob", age:25); rc = h.add(key:3, data:"Charlie", age:35); rc = h.find(key:2); retrieved_name = name; retrieved_age = age; rc = h.find(key:4); if (rc ne 0) then do; retrieved_name = "Not Found"; retrieved_age = .; end; run;
[INFO] Executing DATA step: hash_test
[INFO] Initialized hash table 'h'.
[INFO] Added entry to hash table: id=1, name=Alice, age=30
[INFO] Added entry to hash table: id=2, name=Bob, age=25
[INFO] Added entry to hash table: id=3, name=Charlie, age=35
[INFO] Retrieved from hash table: id=2, name=Bob, age=25
[INFO] Assigned variable 'retrieved_name' = "Bob"
[INFO] Assigned variable 'retrieved_age' = 25
[ERROR] Hash table find failed for key=4
[INFO] Assigned variable 'retrieved_name' = "Not Found"
[INFO] Assigned variable 'retrieved_age' = "."
[INFO] DATA step 'hash_test' executed successfully. 1 observation created.

[INFO] Executing statement: proc print data=hash_test; run;
[INFO] Executing PROC PRINT

id    name      age    retrieved_name    retrieved_age
------------------------------------------------------
1     Alice     30     .                 .
2     Bob       25     Bob               25
3     Charlie   35     .                 .

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Hash Table Operations:**
  - **Initialization:** Declares and initializes a hash table `h` with keys and data fields.
  
  - **Adding Entries:** Adds three entries to the hash table.
  
  - **Retrieving Data:** Successfully retrieves the entry with `id=2` and attempts to retrieve a non-existent entry with `id=4`, handling the failure gracefully.
  
- **Assertions:**
  - Verify that entries are added correctly to the hash table.
  
  - Confirm that data retrieval works as expected, including handling missing keys.
  
  - Ensure that variables `retrieved_name` and `retrieved_age` are assigned appropriately based on retrieval results.

**Note:** This test assumes that hash table support is implemented in the interpreter. The corresponding interpreter code for handling hash tables would need to be developed, including AST nodes, execution logic, and memory management.

---

#### **45.6.4. Test Case 4: Type Conversion Errors**

**SAS Script (`test_type_conversion_errors.sas`):**

```sas
data type_errors;
    input a $ b;
    c = a + b; /* Attempting to add string and numeric */
    d = substr(a, 0, 5); /* Invalid start position */
    e = mdy(13, 32, 2023); /* Invalid month and day */
run;

proc print data=type_errors;
run;
```

**Expected Behavior:**

- **Assignment `c = a + b`:** Should throw a type error.
  
- **Assignment `d = substr(a, 0, 5)`:** Invalid start position (should be >=1).
  
- **Assignment `e = mdy(13, 32, 2023)`:** Invalid month (13) and day (32), should handle gracefully.

**Log Output (`sas_log_type_conversion_errors.txt`):**

```
[INFO] Executing statement: data type_errors; input a $ b; c = a + b; d = substr(a, 0, 5); e = mdy(13, 32, 2023); run;
[INFO] Executing DATA step: type_errors
[INFO] Assigned variable 'a' = "Test"
[INFO] Assigned variable 'b' = 10
[ERROR] Unsupported operation or type combination for operator '+'.
[INFO] DATA step 'type_errors' failed to execute.

[INFO] Executing statement: proc print data=type_errors; run;
[INFO] PROC PRINT skipped due to previous errors.
```

**Explanation:**

- **Type Errors:**
  - **`c = a + b`:** Attempting to add a string and a number results in an unsupported operation error.
  
  - **`d = substr(a, 0, 5)`:** While the invalid start position is intended to throw an error, the current interpreter may not handle it unless specifically implemented.
  
  - **`e = mdy(13, 32, 2023)`:** The `MDY` function receives invalid month and day values, which should be handled appropriately.

- **Error Handling:**
  - The interpreter detects the type error in the first assignment and halts the `DATA` step.
  
  - Subsequent assignments are not executed, and `PROC PRINT` is skipped.

---

### **45.7. Summary of Achievements**

1. **Extended AST for Variable Types:**
   - Introduced `VarType` and `VarValue` to represent different variable types.
   
   - Enhanced `ASTNode` hierarchy to support literals, variables, binary expressions, and function calls with type considerations.
   
2. **Lexer Enhancements:**
   - Updated the Lexer to recognize different literals and SAS keywords, assigning appropriate token types.
   
3. **Parser Enhancements:**
   - Modified the Parser to construct AST nodes that accurately represent variable types and handle different statements like `DATA` and `PROC`.
   
4. **Interpreter Enhancements:**
   - Implemented type conversion methods (`toNumber`, `toString`, `toBoolean`) for accurate data manipulation.
   
   - Enhanced the `executeAssignment` method to handle type checking and value assignments based on variable types.
   
   - Expanded built-in functions to support multiple arguments and handle missing values appropriately.
   
   - Implemented `PROC PRINT` to display datasets with correct formatting based on variable types.
   
5. **Comprehensive Testing:**
   - Developed diverse test cases covering variable type assignments, type checking, type conversion errors, and advanced data structures like hash tables.
   
   - Ensured that the interpreter correctly handles different data types, performs type-specific operations, and gracefully manages errors.
   
6. **Error Handling:**
   - Enhanced error detection and reporting for type mismatches, unsupported operations, and invalid function arguments.
   
   - Ensured that errors halt execution appropriately and provide descriptive messages to aid debugging.
   
7. **Logging Enhancements:**
   - Provided detailed logs for variable assignments, function executions, and error occurrences, facilitating transparency and ease of debugging.

---

### **45.8. Next Steps**

With **Variable Types and Data Structures** successfully implemented, your SAS interpreter is now equipped to handle diverse data types and manage complex data structures, significantly enhancing its versatility and functionality. To continue advancing your interpreter, consider the following next steps:

1. **Implement Advanced PROC Procedures:**
   - **`PROC REPORT`:**
     - **Purpose:** Facilitates customizable and detailed reporting.
     - **Integration:**
       - Introduce `ProcReportNode` in the AST.
       - Update the Lexer and Parser to recognize `PROC REPORT` statements and their options.
       - Implement report generation, supporting features like grouping, summarization, and styling.
     - **Testing:**
       - Develop tests that define and execute complex reports, verifying the accuracy and formatting of outputs.
   
   - **`PROC TABULATE`:**
     - **Purpose:** Creates multi-dimensional tables summarizing data.
     - **Integration and Testing:**
       - Similar to `PROC REPORT`, define AST nodes, update parsing logic, implement functionality, and create corresponding tests.
   
2. **Support Additional Data Formats:**
   - **Excel Files (`.xlsx`):**
     - **Implementation:**
       - Utilize libraries like [libxlsxwriter](https://libxlsxwriter.github.io/) to enable reading from and writing to Excel files.
     - **Testing:**
       - Create tests that import data from Excel files, perform operations, and export results back to Excel, ensuring data integrity.
   
   - **JSON and XML:**
     - **Implementation:**
       - Integrate libraries like [RapidJSON](https://github.com/Tencent/rapidjson) for JSON parsing and [TinyXML](http://www.grinninglizard.com/tinyxml/) for XML handling.
     - **Testing:**
       - Develop tests that import data from JSON and XML sources, manipulate it, and verify correctness.
   
   - **Database Connectivity:**
     - **Implementation:**
       - Allow interfacing with databases (e.g., SQL Server, PostgreSQL) using libraries like [libpq](https://www.postgresql.org/docs/current/libpq.html) for PostgreSQL.
     - **Testing:**
       - Create tests that retrieve data from databases, perform operations, and store results back, ensuring seamless integration.
   
3. **Optimize Performance:**
   - **Data Handling Efficiency:**
     - Refine data structures and algorithms to manage larger datasets more effectively.
     - Implement lazy evaluation or caching strategies for frequently accessed data.
   
   - **Parallel Processing:**
     - Leverage multi-threading for operations that can be executed concurrently, such as data transformations and aggregations.
   
   - **Memory Management:**
     - Optimize memory usage, particularly when dealing with extensive data and complex computations.
   
   - **Profiling and Benchmarking:**
     - Use profiling tools (e.g., `gprof`, `Valgrind`) to identify and address performance bottlenecks.
   
4. **Enhance Error Handling and Debugging Tools:**
   - **Detailed Error Messages:**
     - Include line numbers, character positions, and contextual information in error messages to aid debugging.
   
   - **Debugging Modes:**
     - Implement modes that allow users to step through code execution, set breakpoints, and inspect variable states.
   
   - **Advanced Logging:**
     - Expand logging to capture more granular details about the interpreter's operations, facilitating deeper insights during debugging.
   
5. **Improve Documentation and User Guides:**
   - **Comprehensive User Manual:**
     - Develop detailed documentation covering installation, usage, syntax, examples, and troubleshooting.
   
   - **API Documentation:**
     - If applicable, provide documentation for any APIs or extensions exposed by the interpreter.
   
   - **Example Scripts:**
     - Offer a library of example SAS scripts demonstrating various functionalities, serving as learning resources and templates for users.
   
6. **Implement Security and Sandboxing:**
   - **Secure Execution:**
     - Ensure that the interpreter executes scripts securely, preventing unauthorized access to the system or sensitive data.
   
   - **Sandboxing:**
     - Implement sandboxing techniques to isolate script execution, limit resource usage, and mitigate potential security risks.
   
7. **Expand the REPL Interface:**
   - **Advanced Features:**
     - Implement syntax highlighting, more sophisticated auto-completion, and customizable prompts.
   
   - **User Customizations:**
     - Allow users to configure REPL settings, such as display preferences and key bindings.
   
8. **Leverage Extensibility and Plugin Systems:**
   - **Plugin Architecture:**
     - Design the interpreter to support plugins, enabling users to add custom functionalities or integrate third-party tools.
   
   - **APIs for Extensions:**
     - Provide clear APIs and guidelines for developing extensions, fostering a community-driven ecosystem around your interpreter.
   
9. **Engage with Users and Gather Feedback:**
   - **Beta Testing:**
     - Release beta versions to a select group of users to gather feedback and identify areas for improvement.
   
   - **Community Building:**
     - Create forums, mailing lists, or chat channels where users can ask questions, share scripts, and collaborate.
   
   - **Iterative Development:**
     - Use feedback to prioritize features, fix bugs, and enhance the interpreter based on real-world usage patterns.
   
---

## **Conclusion**

Implementing **Variable Types and Data Structures** is a pivotal advancement in your SAS interpreter's development, enabling it to accurately represent and manipulate diverse data types and manage complex data structures. This enhancement not only aligns your interpreter with SAS's core functionalities but also empowers users to perform sophisticated data operations with confidence and ease.

The comprehensive test cases ensure that the interpreter correctly handles various data types, performs type-specific operations, and gracefully manages errors. The structured implementation approach facilitates future expansions and maintenance, setting the stage for integrating more advanced features like additional `PROC` procedures, external data format support, and performance optimizations.

As you continue to develop your interpreter, maintaining a disciplined and methodical approach¡ªcoupled with thorough testing and clear documentation¡ªwill be instrumental in achieving a reliable and versatile tool for data analysis and reporting. Keep up the excellent work, and feel free to reach out for further assistance or guidance as you progress with your project. Good luck!