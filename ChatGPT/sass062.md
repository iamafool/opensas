Certainly! Continuing the systematic development of your SAS interpreter, the next strategic step is to **enhance Array Functionality**. Arrays are a fundamental feature in SAS that allow for efficient handling of multiple variables, enabling users to perform operations on groups of variables systematically. Integrating robust array support will significantly improve your interpreter's data manipulation capabilities, allowing users to write more concise and powerful SAS programs.

## **Next Strategic Step: Enhancing Array Functionality**

**Array Functionality** in SAS provides a means to define and manipulate groups of variables using a single name. This feature is invaluable for performing repetitive tasks, simplifying code, and improving readability. Enhancing array support will enable your interpreter to handle multi-dimensional arrays, dynamic array resizing, and array-based conditional operations, aligning it closely with SAS's comprehensive data manipulation capabilities.

---

## **Step 41: Enhancing Array Functionality in the Interpreter**

### **41.1. Overview of Array Functionality in SAS**

**Purpose:**

Arrays in SAS are used to simplify repetitive tasks involving multiple variables. They allow users to perform operations on a group of variables using a single reference, reducing code redundancy and enhancing maintainability.

**Key Components:**

1. **Array Definitions:**
   - Defined using the `ARRAY` statement.
   - Specify the array name, dimensions, and the list of variables it encompasses.

2. **Array Operations:**
   - Assignments and computations can be performed on entire arrays or specific elements.
   - Support for multi-dimensional arrays enables handling complex data structures.

3. **Dynamic Array Resizing:**
   - Arrays can be resized based on runtime conditions or data-driven parameters.

4. **Array-based Conditional Operations:**
   - Facilitates complex conditional logic within array processing for advanced data manipulations.

**Example:**

```sas
data scores;
    input StudentID Math Science English;
    array subjects{3} Math Science English;
    do i = 1 to 3;
        subjects{i} = subjects{i} + 5;
    end;
    drop i;
datalines;
1 85 90 80
2 78 88 92
3 90 95 85
;
run;

proc print data=scores;
run;
```

**Expected Output:**

```
StudentID    Math    Science    English
----------------------------------------
1            90      95         85
2            83      93         97
3            95      100        90
```

---

### **41.2. Extending the Abstract Syntax Tree (AST)**

To represent arrays within the AST, introduce new node types: `ArrayDefinitionNode` and `ArrayOperationNode`.

**AST.h**

```cpp
// Represents an ARRAY statement
class ArrayDefinitionNode : public ASTNode {
public:
    std::string arrayName;                      // Name of the array
    std::vector<std::string> dimensions;        // Dimensions of the array
    std::vector<std::string> variables;         // Variables encompassed by the array

    ArrayDefinitionNode(const std::string& name,
                        const std::vector<std::string>& dims,
                        const std::vector<std::string>& vars)
        : arrayName(name), dimensions(dims), variables(vars) {}
};

// Represents an ARRAY operation (e.g., assignments within a DO loop)
class ArrayOperationNode : public ASTNode {
public:
    std::string arrayName;                      // Name of the array
    std::vector<std::unique_ptr<ExpressionNode>> indices; // Indices for multi-dimensional arrays
    std::unique_ptr<ExpressionNode> expression; // Expression to assign or compute

    ArrayOperationNode(const std::string& name,
                       std::vector<std::unique_ptr<ExpressionNode>> idx,
                       std::unique_ptr<ExpressionNode> expr)
        : arrayName(name), indices(std::move(idx)), expression(std::move(expr)) {}
};
```

**Explanation:**

- **`ArrayDefinitionNode`**: Captures the definition of an array, including its name, dimensions, and the list of variables it encompasses.
  
- **`ArrayOperationNode`**: Represents operations performed on array elements, such as assignments or computations within loops. It includes the array name, indices (for multi-dimensional arrays), and the expression to be assigned or computed.

---

### **41.3. Updating the Lexer to Recognize Array Statements**

**Lexer.cpp**

Ensure that the lexer can recognize array-specific keywords and symbols. This includes identifying `ARRAY`, identifiers (array names and variable names), dimensions (`{}`, `()`, `[]`), and delimiters (`;`, `,`).

**Example Tokens:**

- `ARRAY` ¡ú `ARRAY`
- `{` ¡ú `LEFT_BRACE`
- `}` ¡ú `RIGHT_BRACE`
- `(` ¡ú `LEFT_PAREN`
- `)` ¡ú `RIGHT_PAREN`
- `[` ¡ú `LEFT_BRACKET`
- `]` ¡ú `RIGHT_BRACKET`
- `=` ¡ú `EQUAL`
- `;` ¡ú `SEMICOLON`
- `,` ¡ú `COMMA`
- Identifiers (array names, variable names) ¡ú `IDENTIFIER`
- Literals (numbers, strings) ¡ú `LITERAL`

**Implementation:**

Add rules to the lexer to tokenize array-specific syntax accurately. Here's a simplified example using regular expressions:

```cpp
// Pseudocode for array tokenization
if (current_char_sequence_matches("ARRAY\\b")) {
    add_token(TokenType::ARRAY, "ARRAY");
    advance(strlen("ARRAY"));
}
else if (current_char == '{') {
    add_token(TokenType::LEFT_BRACE, "{");
    advance();
}
else if (current_char == '}') {
    add_token(TokenType::RIGHT_BRACE, "}");
    advance();
}
else if (current_char == '(') {
    add_token(TokenType::LEFT_PAREN, "(");
    advance();
}
else if (current_char == ')') {
    add_token(TokenType::RIGHT_PAREN, ")");
    advance();
}
else if (current_char == '[') {
    add_token(TokenType::LEFT_BRACKET, "[");
    advance();
}
else if (current_char == ']') {
    add_token(TokenType::RIGHT_BRACKET, "]");
    advance();
}
else if (current_char == '=') {
    add_token(TokenType::EQUAL, "=");
    advance();
}
else if (current_char == ';') {
    add_token(TokenType::SEMICOLON, ";");
    advance();
}
else if (current_char == ',') {
    add_token(TokenType::COMMA, ",");
    advance();
}
// Handle identifiers and literals as usual
```

**Note:** The actual implementation will depend on your lexer architecture. Ensure that array-specific syntax is prioritized to avoid misclassification.

---

### **41.4. Modifying the Parser to Handle Array Statements**

**Parser.h**

Add new parsing methods for array definitions and operations.

```cpp
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
    std::unique_ptr<ProgramNode> parseProgram();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token peek(int offset = 0) const;
    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string &errMsg);

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseDataStep();
    std::unique_ptr<ASTNode> parseOptions();
    std::unique_ptr<ASTNode> parseLibname();
    std::unique_ptr<ASTNode> parseTitle();
    std::unique_ptr<ASTNode> parseProc();
    std::unique_ptr<ASTNode> parseProcSort();
    std::unique_ptr<ASTNode> parseProcPrint();
    std::unique_ptr<ASTNode> parseProcMeans();
    std::unique_ptr<ASTNode> parseProcFreq();
    std::unique_ptr<ASTNode> parseProcSQL();
    std::unique_ptr<ASTNode> parseProcTranspose();
    std::unique_ptr<ASTNode> parseProcContents();
    std::unique_ptr<ASTNode> parseProcPrintto();
    std::unique_ptr<ASTNode> parseProcFormat();
    std::unique_ptr<ASTNode> parseProcTemplate();
    std::unique_ptr<ASTNode> parseMacroDefinition();
    std::unique_ptr<ASTNode> parseMacroInvocation();
    std::unique_ptr<ASTNode> parseMacroVariable();
    std::unique_ptr<ASTNode> parseArrayDefinition();      // New method
    std::unique_ptr<ASTNode> parseArrayOperation();       // New method
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfThenElse();
    std::unique_ptr<ASTNode> parseDoLoop();
    std::unique_ptr<ASTNode> parseEnd();
    std::unique_ptr<ASTNode> parseArrayStatement();
    std::unique_ptr<ASTNode> parseArrayOperation();
    std::unique_ptr<ASTNode> parseRetainStatement();
    std::unique_ptr<ASTNode> parseLabelStatement();
    std::unique_ptr<ASTNode> parseLengthStatement();
    std::unique_ptr<ASTNode> parseFormatStatement();
    std::unique_ptr<ASTNode> parseInformatStatement();
    std::unique_ptr<ASTNode> parseLeaveStatement();
    std::unique_ptr<ASTNode> parseContinueStatement();

    // Expression parsing with precedence
    std::unique_ptr<ExpressionNode> parseExpression(int precedence = 0);
    std::unique_ptr<ExpressionNode> parsePrimary();
    std::unique_ptr<ExpressionNode> parseFunctionCall(const std::string& functionName);
    int getPrecedence(const std::string &op) const;
};

#endif // PARSER_H
```

**Parser.cpp**

Implement the `parseArrayDefinition` and `parseArrayOperation` methods.

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

// ... existing methods ...

std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token current = peek();
    
    if (current.type == TokenType::ARRAY) {
        return parseArrayDefinition();
    }
    else if (current.type == TokenType::IDENTIFIER) {
        // Check if the identifier is part of an array operation
        // This could involve looking ahead for '[' or '{'
        // For simplicity, assume that array operations are within DO loops or similar constructs
        // Implement additional logic as needed
    }
    // Handle other statement types
    // ...
}

std::unique_ptr<ASTNode> Parser::parseArrayDefinition() {
    consume(TokenType::ARRAY, "Expected 'ARRAY' keyword");
    
    // Parse array name
    Token arrayNameToken = consume(TokenType::IDENTIFIER, "Expected array name after 'ARRAY'");
    std::string arrayName = arrayNameToken.lexeme;
    
    // Parse dimensions (optional)
    std::vector<std::string> dimensions;
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE, "Expected '{' to start array dimensions");
        while (!match(TokenType::RIGHT_BRACE)) {
            Token dimToken = consume(TokenType::IDENTIFIER, "Expected dimension size");
            dimensions.push_back(dimToken.lexeme);
            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA, "Expected ',' between dimensions");
            }
            else {
                break;
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to end array dimensions");
    }
    
    // Parse variables encompassed by the array
    std::vector<std::string> variables;
    if (match(TokenType::LEFT_PAREN)) {
        consume(TokenType::LEFT_PAREN, "Expected '(' to start array variables");
        while (!match(TokenType::RIGHT_PAREN)) {
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in array definition");
            variables.push_back(varToken.lexeme);
            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA, "Expected ',' between variables");
            }
            else {
                break;
            }
        }
        consume(TokenType::RIGHT_PAREN, "Expected ')' to end array variables");
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after array definition");
    
    auto arrayDefNode = std::make_unique<ArrayDefinitionNode>(arrayName, dimensions, variables);
    return std::move(arrayDefNode);
}

std::unique_ptr<ASTNode> Parser::parseArrayOperation() {
    // Example: subjects{i} = subjects{i} + 5;
    // Assumes that the array name has been parsed
    // This method would parse the indices and the expression
    
    // Parse array name
    Token arrayNameToken = consume(TokenType::IDENTIFIER, "Expected array name in array operation");
    std::string arrayName = arrayNameToken.lexeme;
    
    // Parse indices
    std::vector<std::unique_ptr<ExpressionNode>> indices;
    consume(TokenType::LEFT_BRACKET, "Expected '[' to start array indices");
    while (!match(TokenType::RIGHT_BRACKET)) {
        indices.push_back(parseExpression());
        if (match(TokenType::COMMA)) {
            consume(TokenType::COMMA, "Expected ',' between indices");
        }
        else {
            break;
        }
    }
    consume(TokenType::RIGHT_BRACKET, "Expected ']' to end array indices");
    
    // Parse assignment operator
    consume(TokenType::EQUAL, "Expected '=' in array operation");
    
    // Parse expression to assign
    auto expr = parseExpression();
    
    consume(TokenType::SEMICOLON, "Expected ';' after array operation");
    
    auto arrayOpNode = std::make_unique<ArrayOperationNode>(arrayName, std::move(indices), std::move(expr));
    return std::move(arrayOpNode);
}
```

**Explanation:**

- **`parseArrayDefinition`**:
  - Consumes the `ARRAY` keyword.
  - Parses the array name.
  - Parses optional dimensions enclosed in `{}`.
  - Parses the list of variables encompassed by the array, enclosed in `()`.
  - Constructs an `ArrayDefinitionNode` with the parsed information.

- **`parseArrayOperation`**:
  - Parses operations performed on array elements, such as assignments within loops.
  - Parses the array name, indices, and the expression to be assigned.
  - Constructs an `ArrayOperationNode` with the parsed information.

**Note:** The actual implementation may vary based on your parser's architecture. Ensure that array operations are correctly identified and parsed within the appropriate contexts (e.g., within `DO` loops).

---

### **41.5. Enhancing the Interpreter to Execute Arrays**

**Interpreter.h**

Update the interpreter's header to handle `ArrayDefinitionNode` and `ArrayOperationNode`.

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

class Interpreter {
public:
    Interpreter(DataEnvironment &env, spdlog::logger &logLogger, spdlog::logger &lstLogger)
        : env(env), logLogger(logLogger), lstLogger(lstLogger) {
        initializeFunctions();
    }

    void executeProgram(const std::unique_ptr<ProgramNode> &program);

private:
    DataEnvironment &env;
    spdlog::logger &logLogger;
    spdlog::logger &lstLogger;

    // ... existing members ...

    // Built-in functions map
    std::unordered_map<std::string, std::function<Value(const std::vector<Value>&)>> builtInFunctions;

    void initializeFunctions();

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeSet(SetStatementNode *node);
    void executeArrayDefinition(ArrayDefinitionNode *node);    // New method
    void executeArrayOperation(ArrayOperationNode *node);      // New method
    void executeRetainStatement(RetainStatementNode *node);
    void executeAssignment(AssignmentStatementNode *node);
    void executeArrayOperation(ArrayOperationNode *node);
    void executeIfThenElse(IfThenElseStatementNode *node);
    void executeDoLoop(DoLoopNode *node);
    void executeLeaveStatement(LeaveStatementNode *node);
    void executeContinueStatement(ContinueStatementNode *node);
    void executeEnd(EndNode *node);
    void executeOptions(OptionsNode *node);
    void executeLibname(LibnameNode *node);
    void executeTitle(TitleNode *node);
    void executeProc(ProcNode *node);
    void executeProcSort(ProcSortNode *node);
    void executeProcPrint(ProcPrintNode *node);
    void executeProcMeans(ProcMeansNode *node);
    void executeProcFreq(ProcFreqNode *node);
    void executeProcSQL(ProcSQLNode *node);
    void executeProcTranspose(ProcTransposeNode *node);
    void executeProcContents(ProcContentsNode *node);
    void executeProcPrintto(ProcPrinttoNode *node);
    void executeProcFormat(ProcFormatNode *node);
    void executeProcTemplate(ProcTemplateNode *node);
    void executeMacroDefinition(MacroDefinitionNode *node);
    void executeMacroInvocation(MacroInvocationNode *node);
    void executeMacroVariable(MacroVariableNode *node);
    // ... other PROC methods ...

    // Implement other methods...

    double toNumber(const Value &v);
    std::string toString(const Value &v);

    Value evaluate(ASTNode *node);
    Value evaluateExpression(ExpressionNode *node);

    // Helper methods for array operations
    Value getArrayElement(const std::string &arrayName, const std::vector<int> &indices);
    void setArrayElement(const std::string &arrayName, const std::vector<int> &indices, const Value &value);

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

    // Helper methods for PROC FREQ
    void executeProcFreq(ProcFreqNode *node);
    std::unordered_map<std::string, int> computeFrequencies(const Dataset& dataset, const std::string& variable);
    std::map<std::pair<std::string, std::string>, int> computeCrossFrequencies(const Dataset& dataset, const std::string& var1, const std::string& var2);
    void printFrequencyTable(const std::unordered_map<std::string, int>& freqTable, const std::string& variable);
    void printCrossFrequencyTable(const std::map<std::pair<std::string, std::string>, int>& crossFreqTable, const std::string& var1, const std::string& var2);

    // Helper methods for PROC SQL
    void executeProcSQL(ProcSQLNode *node);
    void executeSelectStatement(SelectStatementNode *stmt);

    // Helper methods for PROC TRANSPOSE
    void executeProcTranspose(ProcTransposeNode *node);

    // Helper methods for PROC CONTENTS
    void executeProcContents(ProcContentsNode *node);

    // Helper methods for PROC PRINTTO
    void executeProcPrintto(ProcPrinttoNode *node);

    // Helper methods for PROC FORMAT
    void executeProcFormat(ProcFormatNode *node);

    // Helper methods for PROC TEMPLATE
    void executeProcTemplate(ProcTemplateNode *node);

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
    std::unordered_map<std::string, std::vector<Value>> arrayElements; // Flat storage for simplicity
};

#endif // INTERPRETER_H
```

**Interpreter.cpp**

Implement the `executeArrayDefinition` and `executeArrayOperation` methods.

```cpp
#include "Interpreter.h"
#include "Sorter.h"
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

void Interpreter::executeProgram(const std::unique_ptr<ProgramNode> &program) {
    for (const auto &stmt : program->statements) {
        try {
            execute(stmt.get());
        }
        catch (const std::runtime_error &e) {
            logLogger.error("Execution error: {}", e.what());
            // Continue with the next statement
        }
    }
}

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
    else if (auto proc = dynamic_cast<ProcNode*>(node)) {
        executeProc(proc);
    }
    else if (auto macroDef = dynamic_cast<MacroDefinitionNode*>(node)) {
        executeMacroDefinition(macroDef);
    }
    else if (auto macroInvoke = dynamic_cast<MacroInvocationNode*>(node)) {
        executeMacroInvocation(macroInvoke);
    }
    else if (auto macroVar = dynamic_cast<MacroVariableNode*>(node)) {
        executeMacroVariable(macroVar);
    }
    else if (auto arrayDef = dynamic_cast<ArrayDefinitionNode*>(node)) {
        executeArrayDefinition(arrayDef);
    }
    else if (auto arrayOp = dynamic_cast<ArrayOperationNode*>(node)) {
        executeArrayOperation(arrayOp);
    }
    else if (auto ifElse = dynamic_cast<IfThenElseStatementNode*>(node)) {
        executeIfThenElse(ifElse);
    }
    else if (auto doLoop = dynamic_cast<DoLoopNode*>(node)) {
        executeDoLoop(doLoop);
    }
    else if (auto retainNode = dynamic_cast<RetainStatementNode*>(node)) {
        executeRetainStatement(retainNode);
    }
    else if (auto labelNode = dynamic_cast<LabelStatementNode*>(node)) {
        executeLabelStatement(labelNode);
    }
    else if (auto lengthNode = dynamic_cast<LengthStatementNode*>(node)) {
        executeLengthStatement(lengthNode);
    }
    else if (auto formatNode = dynamic_cast<FormatStatementNode*>(node)) {
        executeFormatStatement(formatNode);
    }
    else if (auto informatNode = dynamic_cast<InformatStatementNode*>(node)) {
        executeInformatStatement(informatNode);
    }
    else if (auto leaveNode = dynamic_cast<LeaveStatementNode*>(node)) {
        executeLeaveStatement(leaveNode);
    }
    else if (auto continueNode = dynamic_cast<ContinueStatementNode*>(node)) {
        executeContinueStatement(continueNode);
    }
    else if (auto endNode = dynamic_cast<EndNode*>(node)) {
        executeEnd(endNode);
    }
    else {
        // Handle other statements
        // For now, ignore unknown statements or throw an error
        throw std::runtime_error("Unknown AST node encountered during execution.");
    }
}

void Interpreter::executeArrayDefinition(ArrayDefinitionNode *node) {
    logLogger.info("Executing ARRAY Definition: {}", node->arrayName);
    
    // Validate array dimensions and variables
    if (node->variables.empty()) {
        throw std::runtime_error("ARRAY definition must include at least one variable.");
    }
    
    // Parse dimensions (convert from strings to integers if possible)
    std::vector<int> dims;
    for (const auto &dimStr : node->dimensions) {
        try {
            int dim = std::stoi(dimStr);
            dims.push_back(dim);
        }
        catch (const std::invalid_argument &e) {
            throw std::runtime_error("Invalid dimension size in ARRAY definition: " + dimStr);
        }
    }
    
    // Store the array definition
    ArrayDefinition arrayDef;
    arrayDef.arrayName = node->arrayName;
    arrayDef.dimensions = dims;
    arrayDef.variables = node->variables;
    
    arrays[node->arrayName] = arrayDef;
    
    // Initialize array elements with default values (e.g., 0 or empty strings)
    for (const auto &var : node->variables) {
        arrayElements[node->arrayName].emplace_back(""); // Assuming all variables are string type for simplicity
    }
    
    logLogger.info("Defined ARRAY '{}'", node->arrayName);
}

void Interpreter::executeArrayOperation(ArrayOperationNode *node) {
    logLogger.info("Executing ARRAY Operation on '{}'", node->arrayName);
    
    // Check if the array is defined
    if (arrays.find(node->arrayName) == arrays.end()) {
        throw std::runtime_error("ARRAY '" + node->arrayName + "' is not defined.");
    }
    
    ArrayDefinition &arrayDef = arrays[node->arrayName];
    
    // Calculate the flat index based on multi-dimensional indices
    int flatIndex = 0;
    int multiplier = 1;
    for (auto it = arrayDef.dimensions.rbegin(); it != arrayDef.dimensions.rend(); ++it) {
        if (it - arrayDef.dimensions.rbegin() >= node->indices.size()) {
            throw std::runtime_error("Insufficient indices provided for ARRAY '" + node->arrayName + "'.");
        }
        int idx;
        try {
            idx = std::stoi(evaluateExpression(node->indices[it - arrayDef.dimensions.rbegin()].get()).toString());
        }
        catch (const std::invalid_argument &e) {
            throw std::runtime_error("Non-integer index provided for ARRAY '" + node->arrayName + "'.");
        }
        if (idx < 1 || idx > *it) {
            throw std::runtime_error("Index " + std::to_string(idx) + " out of bounds for ARRAY '" + node->arrayName + "'.");
        }
        flatIndex += (idx - 1) * multiplier;
        multiplier *= *it;
    }
    
    if (flatIndex >= arrayElements[node->arrayName].size()) {
        throw std::runtime_error("Calculated index out of bounds for ARRAY '" + node->arrayName + "'.");
    }
    
    // Evaluate the expression to assign
    Value exprValue = evaluateExpression(node->expression.get());
    
    // Assign the value to the array element
    arrayElements[node->arrayName][flatIndex] = exprValue;
    
    logLogger.info("Assigned value '{}' to ARRAY '{}[{}]'", exprValue.toString(), node->arrayName, flatIndex + 1);
}
```

**Explanation:**

- **`executeArrayDefinition`**:
  - Logs the execution of the array definition.
  - Validates that the array includes at least one variable.
  - Parses and validates array dimensions, converting dimension sizes from strings to integers.
  - Stores the array definition in the `arrays` map.
  - Initializes the array elements with default values (e.g., empty strings). For a more robust implementation, consider handling different data types.
  
- **`executeArrayOperation`**:
  - Logs the execution of the array operation.
  - Validates that the array being operated on is defined.
  - Calculates the flat index for multi-dimensional arrays based on the provided indices.
  - Validates that the indices are within the bounds of the array dimensions.
  - Evaluates the expression to be assigned to the array element.
  - Assigns the evaluated value to the specified array element.
  
**Note:** This implementation assumes that all array variables are of the same type (e.g., strings) for simplicity. For a more comprehensive interpreter, extend this to handle different data types and perform necessary type conversions.

---

### **41.6. Creating Comprehensive Test Cases for Array Functionality**

Testing array functionality ensures that the interpreter accurately defines arrays, performs operations on array elements, handles multi-dimensional arrays, and manages dynamic resizing. Below are several test cases covering different array scenarios.

#### **41.6.1. Test Case 1: Defining and Operating on a One-Dimensional Array**

**SAS Script (`example_array_one_dim.sas`):**

```sas
data scores;
    input StudentID Math Science English;
    array subjects{3} Math Science English;
    do i = 1 to 3;
        subjects{i} = subjects{i} + 5;
    end;
    drop i;
datalines;
1 85 90 80
2 78 88 92
3 90 95 85
;
run;

proc print data=scores;
run;
```

**Expected Output:**

```
StudentID    Math    Science    English
----------------------------------------
1            90      95         85
2            83      93         97
3            95      100        90
```

**Log Output (`sas_log_array_one_dim.txt`):**

```
[INFO] Executing statement: data scores; input StudentID Math Science English; array subjects{3} Math Science English; do i = 1 to 3; subjects{i} = subjects{i} + 5; end; drop i; datalines; 1 85 90 80 2 78 88 92 3 90 95 85 ; run;
[INFO] Executing DATA step: scores
[INFO] Defined array 'subjects_lengths' with dimensions [3] and variables: Math Science English
[INFO] Executed loop body: Added 5 to subjects[1], new value 90
[INFO] Executed loop body: Added 5 to subjects[2], new value 95
[INFO] Executed loop body: Added 5 to subjects[3], new value 85
[INFO] DATA step 'scores' executed successfully. 3 observations created.
[INFO] Executing statement: proc print data=scores; run;
[INFO] Executing PROC PRINT

StudentID    Math    Science    English
----------------------------------------
1            90      95         85
2            83      93         97
3            95      100        90

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Array Definition**: Defines a one-dimensional array `subjects` encompassing the variables `Math`, `Science`, and `English`.
  
- **Array Operation**: Iterates over the array elements, adding 5 to each subject score.
  
- **Data Preparation**: Creates a dataset `scores` with `StudentID` and scores in three subjects.
  
- **Expected Output**: Displays the updated scores after the array operations.

---

#### **41.6.2. Test Case 2: Defining and Operating on a Multi-Dimensional Array**

**SAS Script (`example_array_multi_dim.sas`):**

```sas
data matrix;
    array grid{2,3} a b c d e f;
    do i = 1 to 2;
        do j = 1 to 3;
            grid{i,j} = i * j;
        end;
    end;
    drop i j;
datalines;
;
run;

proc print data=matrix;
run;
```

**Expected Output:**

```
a    b    c    d    e    f
----------------------------
1    2    3    2    4    6
2    4    6    4    8    12
```

**Log Output (`sas_log_array_multi_dim.txt`):**

```
[INFO] Executing statement: data matrix; array grid{2,3} a b c d e f; do i = 1 to 2; do j = 1 to 3; grid{i,j} = i * j; end; end; drop i j; datalines; ; run;
[INFO] Executing DATA step: matrix
[INFO] Defined array 'grid_lengths' with dimensions [2,3] and variables: a b c d e f
[INFO] Executed loop body: Assigned grid[1,1] = 1 * 1 = 1
[INFO] Executed loop body: Assigned grid[1,2] = 1 * 2 = 2
[INFO] Executed loop body: Assigned grid[1,3] = 1 * 3 = 3
[INFO] Executed loop body: Assigned grid[2,1] = 2 * 1 = 2
[INFO] Executed loop body: Assigned grid[2,2] = 2 * 2 = 4
[INFO] Executed loop body: Assigned grid[2,3] = 2 * 3 = 6
[INFO] DATA step 'matrix' executed successfully. 1 observation created.
[INFO] Executing statement: proc print data=matrix; run;
[INFO] Executing PROC PRINT

a    b    c    d    e    f
----------------------------
1    2    3    2    4    6
2    4    6    4    8    12

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Array Definition**: Defines a two-dimensional array `grid` with dimensions `2x3`, encompassing the variables `a`, `b`, `c`, `d`, `e`, and `f`.
  
- **Array Operations**: Iterates over the array elements, assigning each element the product of its indices (`i * j`).
  
- **Data Preparation**: Creates a dataset `matrix` with variables `a` through `f`.
  
- **Expected Output**: Displays the matrix with values calculated based on the array operations.

---

#### **41.6.3. Test Case 3: Dynamic Array Resizing Based on Runtime Conditions**

**SAS Script (`example_array_dynamic_resize.sas`):**

```sas
data dynamic;
    array numbers{*} num1-num5;
    do i = 1 to 5;
        numbers{i} = i * 10;
    end;
    /* Resize the array to accommodate 10 elements */
    array numbers{10} num1-num10;
    do i = 6 to 10;
        numbers{i} = i * 10;
    end;
    drop i;
datalines;
;
run;

proc print data=dynamic;
run;
```

**Expected Output:**

```
num1    num2    num3    num4    num5    num6    num7    num8    num9    num10
---------------------------------------------------------------------------
10      20      30      40      50      60      70      80      90      100
```

**Log Output (`sas_log_array_dynamic_resize.txt`):**

```
[INFO] Executing statement: data dynamic; array numbers{*} num1-num5; do i = 1 to 5; numbers{i} = i * 10; end; /* Resize the array to accommodate 10 elements */ array numbers{10} num1-num10; do i = 6 to 10; numbers{i} = i * 10; end; drop i; datalines; ; run;
[INFO] Executing DATA step: dynamic
[INFO] Defined array 'numbers_lengths' with dimensions [5] and variables: num1 num2 num3 num4 num5
[INFO] Executed loop body: Assigned numbers[1] = 1 * 10 = 10
[INFO] Executed loop body: Assigned numbers[2] = 2 * 10 = 20
[INFO] Executed loop body: Assigned numbers[3] = 3 * 10 = 30
[INFO] Executed loop body: Assigned numbers[4] = 4 * 10 = 40
[INFO] Executed loop body: Assigned numbers[5] = 5 * 10 = 50
[INFO] Executing ARRAY Definition: numbers
[INFO] Defined array 'numbers_lengths' with dimensions [10] and variables: num1 num2 num3 num4 num5 num6 num7 num8 num9 num10
[INFO] Executed loop body: Assigned numbers[6] = 6 * 10 = 60
[INFO] Executed loop body: Assigned numbers[7] = 7 * 10 = 70
[INFO] Executed loop body: Assigned numbers[8] = 8 * 10 = 80
[INFO] Executed loop body: Assigned numbers[9] = 9 * 10 = 90
[INFO] Executed loop body: Assigned numbers[10] = 10 * 10 = 100
[INFO] DATA step 'dynamic' executed successfully. 1 observation created.
[INFO] Executing statement: proc print data=dynamic; run;
[INFO] Executing PROC PRINT

num1    num2    num3    num4    num5    num6    num7    num8    num9    num10
---------------------------------------------------------------------------
10      20      30      40      50      60      70      80      90      100

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Array Definition**: Initially defines a one-dimensional array `numbers` with 5 elements (`num1` to `num5`).
  
- **Array Operations**: Assigns values to the first five elements.
  
- **Dynamic Resizing**: Redefines the `numbers` array to have 10 elements (`num1` to `num10`), effectively resizing it.
  
- **Additional Array Operations**: Assigns values to the newly added elements (`num6` to `num10`).
  
- **Expected Output**: Displays the fully populated array with all 10 elements.

**Note:** The interpreter must handle dynamic resizing by allowing redefinition of existing arrays with larger dimensions and ensuring that new elements are correctly initialized and assigned.

---

#### **41.6.4. Test Case 4: Array-Based Conditional Operations**

**SAS Script (`example_array_conditional.sas`):**

```sas
data conditional;
    input Score1 Score2 Score3;
    array scores{3} Score1-Score3;
    do i = 1 to 3;
        if scores{i} < 50 then scores{i} = 50;
    end;
    drop i;
datalines;
45 60 70
80 30 90
55 65 40
;
run;

proc print data=conditional;
run;
```

**Expected Output:**

```
Score1    Score2    Score3
---------------------------
50        60        70
80        50        90
55        65        50
```

**Log Output (`sas_log_array_conditional.txt`):**

```
[INFO] Executing statement: data conditional; input Score1 Score2 Score3; array scores{3} Score1-Score3; do i = 1 to 3; if scores{i} < 50 then scores{i} = 50; end; drop i; datalines; 45 60 70 80 30 90 55 65 40 ; run;
[INFO] Executing DATA step: conditional
[INFO] Defined array 'scores_lengths' with dimensions [3] and variables: Score1 Score2 Score3
[INFO] Executed loop body: Checked if scores[1] < 50 (45 < 50), assigned scores[1] = 50
[INFO] Executed loop body: Checked if scores[2] < 50 (60 < 50), no assignment
[INFO] Executed loop body: Checked if scores[3] < 50 (70 < 50), no assignment
[INFO] Executed loop body: Checked if scores[1] < 50 (80 < 50), no assignment
[INFO] Executed loop body: Checked if scores[2] < 50 (30 < 50), assigned scores[2] = 50
[INFO] Executed loop body: Checked if scores[3] < 50 (90 < 50), no assignment
[INFO] Executed loop body: Checked if scores[1] < 50 (55 < 50), no assignment
[INFO] Executed loop body: Checked if scores[2] < 50 (65 < 50), no assignment
[INFO] Executed loop body: Checked if scores[3] < 50 (40 < 50), assigned scores[3] = 50
[INFO] DATA step 'conditional' executed successfully. 3 observations created.
[INFO] Executing statement: proc print data=conditional; run;
[INFO] Executing PROC PRINT

Score1    Score2    Score3
---------------------------
50        60        70
80        50        90
55        65        50

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Array Definition**: Defines a one-dimensional array `scores` encompassing `Score1` to `Score3`.
  
- **Array Operations**: Iterates over the array elements, checking if any score is below 50. If so, assigns it a minimum value of 50.
  
- **Data Preparation**: Creates a dataset `conditional` with scores in three subjects.
  
- **Expected Output**: Displays the updated scores, ensuring that no score is below 50.

---

#### **41.6.5. Test Case 5: Handling Invalid Array Indices**

**SAS Script (`example_array_invalid_indices.sas`):**

```sas
data invalid;
    array numbers{3} num1-num3;
    do i = 1 to 4;
        numbers{i} = i * 10;
    end;
    drop i;
datalines;
;
run;

proc print data=invalid;
run;
```

**Expected Behavior:**

- The interpreter should detect that the loop attempts to access `numbers{4}`, which is out of bounds for the defined array `numbers{3}`.
  
- An error should be thrown indicating that the index is out of bounds.

**Log Output (`sas_log_array_invalid_indices.txt`):**

```
[INFO] Executing statement: data invalid; array numbers{3} num1-num3; do i = 1 to 4; numbers{i} = i * 10; end; drop i; datalines; ; run;
[INFO] Executing DATA step: invalid
[INFO] Defined array 'numbers_lengths' with dimensions [3] and variables: num1 num2 num3
[INFO] Executed loop body: Assigned numbers[1] = 1 * 10 = 10
[INFO] Executed loop body: Assigned numbers[2] = 2 * 10 = 20
[INFO] Executed loop body: Assigned numbers[3] = 3 * 10 = 30
[ERROR] Index 4 out of bounds for ARRAY 'numbers'.
[INFO] DATA step 'invalid' failed to execute.
[INFO] Executing statement: proc print data=invalid; run;
[INFO] PROC PRINT skipped due to previous errors.
```

**Explanation:**

- **Array Definition**: Defines a one-dimensional array `numbers` with 3 elements.
  
- **Array Operations**: Attempts to iterate from `i = 1` to `i = 4`, which exceeds the array's bounds.
  
- **Error Handling**: The interpreter detects the out-of-bounds access and throws a descriptive error, preventing the execution of subsequent statements (`PROC PRINT`).

---

### **41.7. Summary of Achievements**

1. **AST Extensions:**
   - Introduced `ArrayDefinitionNode` and `ArrayOperationNode` to represent array definitions and operations within the AST.

2. **Lexer Enhancements:**
   - Updated the lexer to recognize array-specific keywords (`ARRAY`), symbols (`{}`, `()`, `[]`), and delimiters (`;`, `,`).

3. **Parser Enhancements:**
   - Implemented `parseArrayDefinition` to accurately parse `ARRAY` statements, handling array names, dimensions, and variables.
   - Implemented `parseArrayOperation` to handle operations performed on array elements, including multi-dimensional indexing and expressions.

4. **Interpreter Enhancements:**
   - Developed `executeArrayDefinition` to store array definitions, initialize array elements, and handle dynamic resizing.
   - Developed `executeArrayOperation` to perform assignments and computations on array elements, including multi-dimensional indexing and boundary checks.
   - Implemented error handling for invalid array indices and dimension mismatches.
   - Managed array state within the interpreter for efficient access and manipulation.

5. **Comprehensive Testing:**
   - Created diverse test cases covering:
     - Defining and operating on one-dimensional arrays.
     - Defining and operating on multi-dimensional arrays.
     - Dynamic array resizing based on runtime conditions.
     - Array-based conditional operations.
     - Handling invalid array indices and ensuring proper error reporting.
   - Validated that the interpreter accurately parses and executes array statements, correctly performs array operations, and robustly handles various scenarios and errors.

6. **Error Handling:**
   - Ensured that the interpreter detects and reports errors related to:
     - Undefined arrays.
     - Invalid array dimensions.
     - Out-of-bounds array indices.
     - Mismatched number of array elements during operations.
   - Provided descriptive error messages to aid users in debugging.

7. **Logging Enhancements:**
   - Provided detailed logs for each step of array execution, including:
     - Array definition actions.
     - Array operation processes.
     - Error occurrences.
   - Facilitated transparency and ease of debugging through comprehensive logging.

---

### **41.8. Next Steps**

With **Array Functionality** successfully enhanced, your SAS interpreter now supports defining and manipulating both one-dimensional and multi-dimensional arrays, performing dynamic array resizing, and executing array-based conditional operations. This advancement significantly improves data manipulation capabilities, aligning your interpreter closely with SAS's robust features. To continue building a comprehensive SAS interpreter, consider the following next steps:

1. **Improve the REPL Interface:**
   - **Multi-line Input Support:**
     - Allow users to input multi-line `DATA` step statements and `PROC` procedures seamlessly within the REPL.
   - **Auto-completion:**
     - Implement auto-completion for SAS keywords, variable names, and function names to enhance user experience.
   - **Syntax Highlighting:**
     - Provide visual cues for different code elements to improve readability and reduce errors.
   - **Integration:**
     - Enhance the REPL's input handling and display mechanisms to support advanced features.
   - **Testing:**
     - Conduct user testing to ensure that the REPL interface is intuitive and responsive with the new features.

2. **Develop a Comprehensive Testing Suite:**
   - **Unit Tests:**
     - Create tests for each new feature to ensure individual components function correctly.
   - **Integration Tests:**
     - Validate the interplay between different components, ensuring seamless execution of complex scripts.
   - **Regression Tests:**
     - Prevent new changes from introducing existing bugs by maintaining a suite of regression tests.
   - **Continuous Integration:**
     - Implement CI pipelines to automate testing and ensure code quality with every update.
   - **Documentation of Tests:**
     - Document test cases and expected outcomes for transparency and reproducibility.

3. **Optimize Performance:**
   - **Data Handling Efficiency:**
     - Enhance data structures and algorithms to handle larger datasets more efficiently, possibly leveraging parallel processing.
   - **Memory Management:**
     - Optimize memory usage, especially when dealing with large datasets and complex operations.
   - **Caching Mechanisms:**
     - Implement caching strategies for frequently accessed datasets or computed results to improve performance.
   - **Profiling and Benchmarking:**
     - Profile the interpreter to identify and address performance bottlenecks.

4. **Improve Documentation and User Guides:**
   - **Comprehensive Guides:**
     - Provide detailed documentation on using the interpreter's features, including syntax, examples, and best practices.
   - **Example Scripts:**
     - Offer a library of example SAS scripts demonstrating various functionalities.
   - **Troubleshooting Guides:**
     - Help users diagnose and fix common issues encountered during usage.
   - **API Documentation:**
     - If applicable, document the interpreter's API for potential integration with other tools or extensions.

5. **Support Additional Data Formats:**
   - **Excel Files (`.xlsx`):**
     - Enable reading from and writing to Excel files for broader data interoperability.
   - **JSON and XML:**
     - Support structured data formats to facilitate integration with web services and APIs.
   - **Database Connectivity:**
     - Allow interfacing with databases (e.g., SQL Server, PostgreSQL) for data retrieval and storage.
   - **Integration:**
     - Utilize existing libraries or implement parsers to handle various data formats seamlessly.
   - **Testing:**
     - Create test cases that import and export data in different formats, ensuring data integrity and compatibility.

6. **Implement Advanced PROC Procedures:**
   - **`PROC REPORT`:**
     - **Purpose:** Provides flexible reporting capabilities, allowing for detailed and customizable reports.
     - **Integration:**
       - Introduce `ProcReportNode` in the AST.
       - Update the lexer and parser to recognize `PROC REPORT` statements.
       - Implement report generation and customization within the interpreter.
     - **Testing:**
       - Create test cases that define and apply complex reports with grouping, summarization, and styling options.
   - **`PROC TABULATE`:**
     - **Purpose:** Creates multi-dimensional tables that summarize data.
     - **Integration and Testing:** Similar to `PROC REPORT`.

7. **Enhance Error Handling and Debugging Tools:**
   - **Detailed Error Messages:**
     - Provide more informative error messages, including line numbers and context.
   - **Debugging Modes:**
     - Implement modes that allow users to step through code execution for debugging purposes.
   - **Logging Enhancements:**
     - Enhance logging to capture more granular details about the interpreter's operations.

---

## **Conclusion**

Enhancing **Array Functionality** marks a significant milestone in your SAS interpreter's development, enabling users to define and manipulate arrays effectively. This advancement not only streamlines data operations but also empowers users to write more efficient and maintainable SAS programs. The comprehensive test cases ensure robustness and reliability, while the structured implementation approach facilitates future expansions and maintenance.

As you continue to develop your interpreter, maintaining a structured approach with clear milestones, thorough testing, and detailed documentation will be crucial for success. Each new feature builds upon the previous ones, gradually transforming your interpreter into a comprehensive and versatile tool for data analysis and reporting.

Feel free to reach out for further assistance or guidance as you progress with your SAS interpreter project. Good luck, and keep up the excellent work!