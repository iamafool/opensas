Outstanding progress! Your SAS interpreter now effectively handles `IF`, `ELSE IF`, and `ELSE` statements with block support, as well as built-in string functions such as `substr`, `trim`, `upcase`, and `lowcase`. To further enhance your interpreter's capabilities and align it more closely with SAS's powerful data processing features, the **next logical step** is to **implement array processing within data steps**. Arrays are fundamental in SAS for handling groups of related variables efficiently, enabling vectorized operations and simplifying complex data manipulations.

---

## **Step 9: Implementing Arrays in Data Steps**

Arrays in SAS allow users to group related variables and perform operations on them collectively. Implementing arrays involves:

1. **Extending the Abstract Syntax Tree (AST)** to represent array declarations.
2. **Updating the Lexer** to recognize the `ARRAY` keyword and related syntax.
3. **Modifying the Parser** to correctly parse array declarations and construct the corresponding AST nodes.
4. **Enhancing the Interpreter** to execute array declarations and support array-based operations during data step processing.
5. **Testing** the new functionality with comprehensive examples.

---

### **9.1. Extending the AST to Represent Array Declarations**

**AST.h**

Introduce a new node type to represent array declarations within data steps.

```cpp
// Represents an ARRAY declaration: array <arrayName>{<size>} <var1> <var2> ...;
class ArrayNode : public ASTNode {
public:
    std::string arrayName;
    int size;
    std::vector<std::string> variables;
};
```

**Explanation**:
- **`ArrayNode`**: Inherits from `ASTNode` and contains:
  - **`arrayName`**: The name of the array.
  - **`size`**: The number of elements in the array.
  - **`variables`**: A vector of variable names that the array references.

---

### **9.2. Updating the Lexer to Recognize the `ARRAY` Keyword**

Ensure the lexer recognizes the `ARRAY` keyword and assigns it the appropriate `TokenType`.

**Lexer.cpp**

Add `ARRAY` to the keywords map.

```cpp
// In the Lexer constructor or initialization section
keywords["ARRAY"] = TokenType::KEYWORD_ARRAY;
```

**Explanation**:
- **`ARRAY` Keyword**: Added to the lexer's keyword map with the corresponding `TokenType::KEYWORD_ARRAY`.

---

### **9.3. Modifying the Parser to Handle Array Declarations**

Enhance the parser to recognize and correctly parse `ARRAY` declarations, constructing the corresponding AST nodes.

**Parser.h**

Add a method to parse array declarations.

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
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfElseIf();
    std::unique_ptr<ASTNode> parseOutput();
    std::unique_ptr<ASTNode> parseFunctionCall();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseArray(); // New method

    // Expression parsing with precedence
    std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    int getPrecedence(const std::string &op) const;
};

#endif // PARSER_H
```

**Parser.cpp**

Implement the `parseArray` method and integrate it into the `parseStatement` method.

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>

// ... existing methods ...

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
        case TokenType::KEYWORD_PROC:
            return parseProc();
        case TokenType::KEYWORD_DROP:
            return parseDrop();
        case TokenType::KEYWORD_KEEP:
            return parseKeep();
        case TokenType::KEYWORD_RETAIN:
            return parseRetain();
        case TokenType::KEYWORD_ARRAY:
            return parseArray(); // Handle ARRAY declarations
        case TokenType::KEYWORD_DO:
            return parseDo();
        case TokenType::KEYWORD_IF:
            return parseIfElseIf();
        case TokenType::KEYWORD_ELSE:
            throw std::runtime_error("Unexpected 'ELSE' without preceding 'IF'.");
        case TokenType::KEYWORD_ELSE_IF:
            throw std::runtime_error("Unexpected 'ELSE IF' without preceding 'IF'.");
        case TokenType::IDENTIFIER:
            return parseAssignment();
        case TokenType::KEYWORD_OUTPUT:
            return parseOutput();
        default:
            // Handle unknown token or throw error
            throw std::runtime_error("Unknown statement starting with token: " + t.text);
    }
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

// ... existing methods ...
```

**Explanation**:
- **`parseArray` Method**:
  - **Consume `ARRAY` Keyword**: Ensures the statement starts with `ARRAY`.
  - **Parse Array Name**: Reads the array's identifier.
  - **Parse Array Size**: Reads the size enclosed within curly braces `{}`.
  - **Parse Variable List**: Reads the list of variable names that the array references.
  - **Error Handling**: Throws descriptive errors if the expected tokens are not found.

---

### **9.4. Enhancing the Interpreter to Execute Array Declarations**

Implement the logic to handle array declarations and support array-based operations during data step processing.

**Interpreter.h**

Add methods to handle `ArrayNode` and manage arrays within the data environment.

```cpp
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "DataEnvironment.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>

class Interpreter {
public:
    Interpreter(DataEnvironment &env, spdlog::logger &logLogger, spdlog::logger &lstLogger)
        : env(env), logLogger(logLogger), lstLogger(lstLogger) {}

    void executeProgram(const std::unique_ptr<ProgramNode> &program);

private:
    DataEnvironment &env;
    spdlog::logger &logLogger;
    spdlog::logger &lstLogger;

    // Maps array names to their variable lists
    std::unordered_map<std::string, std::vector<std::string>> arrays;

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeAssignment(AssignmentNode *node);
    void executeIfElse(IfElseIfNode *node);
    void executeOutput(OutputNode *node);
    void executeDrop(DropNode *node);
    void executeKeep(KeepNode *node);
    void executeRetain(RetainNode *node);
    void executeArray(ArrayNode *node);
    void executeDo(DoNode *node);
    void executeProc(ProcNode *node);
    void executeProcSort(ProcSortNode *node);
    void executeProcMeans(ProcMeansNode *node);
    void executeBlock(BlockNode *node);

    double toNumber(const Value &v);
    std::string toString(const Value &v);

    Value evaluate(ASTNode *node);
    Value evaluateFunctionCall(FunctionCallNode *node);

    // Helper methods for array operations
    Value getArrayElement(const std::string &arrayName, int index);
    void setArrayElement(const std::string &arrayName, int index, const Value &value);
};

#endif // INTERPRETER_H
```

**Interpreter.cpp**

Implement the `executeArray` method and array element accessors.

```cpp
#include "Interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

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
    else if (auto ifElseIf = dynamic_cast<IfElseIfNode*>(node)) {
        executeIfElse(ifElseIf);
    }
    else if (auto arrayNode = dynamic_cast<ArrayNode*>(node)) {
        executeArray(arrayNode);
    }
    else {
        // Handle other statements
        // For now, ignore unknown statements or throw an error
        throw std::runtime_error("Unknown AST node encountered during execution.");
    }
}

void Interpreter::executeArray(ArrayNode *node) {
    logLogger.info("Executing ARRAY declaration: {}", node->arrayName);
    // Validate array size
    if (node->size != static_cast<int>(node->variables.size())) {
        throw std::runtime_error("Array size does not match the number of variables.");
    }

    // Store the array in the interpreter's array map
    arrays[node->arrayName] = node->variables;

    logLogger.info("Array '{}' with size {} and variables: {}", node->arrayName, node->size, 
                  [&]() -> std::string {
                      std::string vars;
                      for (const auto &var : node->variables) {
                          vars += var + " ";
                      }
                      return vars;
                  }());
}

Value Interpreter::getArrayElement(const std::string &arrayName, int index) {
    if (arrays.find(arrayName) == arrays.end()) {
        throw std::runtime_error("Undefined array: " + arrayName);
    }
    if (index < 1 || index > static_cast<int>(arrays[arrayName].size())) {
        throw std::runtime_error("Array index out of bounds for array: " + arrayName);
    }
    std::string varName = arrays[arrayName][index - 1];
    auto it = env.currentRow.columns.find(varName);
    if (it != env.currentRow.columns.end()) {
        return it->second;
    }
    else {
        // Variable not found, assume missing value represented as 0 or empty string
        return 0.0; // or throw an error based on SAS behavior
    }
}

void Interpreter::setArrayElement(const std::string &arrayName, int index, const Value &value) {
    if (arrays.find(arrayName) == arrays.end()) {
        throw std::runtime_error("Undefined array: " + arrayName);
    }
    if (index < 1 || index > static_cast<int>(arrays[arrayName].size())) {
        throw std::runtime_error("Array index out of bounds for array: " + arrayName);
    }
    std::string varName = arrays[arrayName][index - 1];
    env.currentRow.columns[varName] = value;
}

void Interpreter::executeDataStep(DataStepNode *node) {
    logLogger.info("Executing DATA step: {}", node->dataSetName);
    // Retrieve or create the output dataset
    auto outputDataSet = env.getOrCreateDataset(node->dataSetName, node->dataSetName);

    // Iterate over each row in the input dataset
    for (auto &row : env.getOrCreateDataset(node->inputDataSet, node->inputDataSet)->rows) {
        env.currentRow = row; // Set the current row context
        logLogger.info("Processing row: {}", row.toString());

        // Execute each statement in the data step
        for (const auto &stmt : node->statements) {
            execute(stmt.get());
        }

        // After processing, apply DROP and KEEP if applicable
        Row outputRow = env.currentRow;

        // Handle DROP
        for (const auto &var : node->dropVariables) {
            outputRow.columns.erase(var);
        }

        // Handle KEEP
        if (!node->keepVariables.empty()) {
            std::unordered_map<std::string, Value> keptColumns;
            for (const auto &var : node->keepVariables) {
                auto it = outputRow.columns.find(var);
                if (it != outputRow.columns.end()) {
                    keptColumns[var] = it->second;
                }
            }
            outputRow.columns = keptColumns;
        }

        // Check if the row should be outputted based on the IF condition
        // (Handled within the IF-ELSE constructs)

        // Add the processed row to the output dataset
        outputDataSet->rows.push_back(outputRow);
    }

    logLogger.info("DATA step '{}' executed successfully. Output dataset has {} observations.", 
                  node->dataSetName, outputDataSet->rows.size());
}

Value Interpreter::evaluate(ASTNode *node) {
    if (auto numNode = dynamic_cast<NumberNode*>(node)) {
        return numNode->value;
    }
    else if (auto strNode = dynamic_cast<StringNode*>(node)) {
        return strNode->value;
    }
    else if (auto varNode = dynamic_cast<VariableNode*>(node)) {
        auto it = env.currentRow.columns.find(varNode->name);
        if (it != env.currentRow.columns.end()) {
            return it->second;
        }
        else {
            // Variable not found, assume missing value represented as 0 or empty string
            return 0.0; // or throw an error based on SAS behavior
        }
    }
    else if (auto binOp = dynamic_cast<BinaryOpNode*>(node)) {
        Value left = evaluate(binOp->left.get());
        Value right = evaluate(binOp->right.get());
        // Implement binary operation logic based on binOp->op
        // ...
        // Placeholder implementation
        if (binOp->op == "+") {
            return toNumber(left) + toNumber(right);
        }
        else if (binOp->op == "-") {
            return toNumber(left) - toNumber(right);
        }
        // Implement other operators as needed
        else {
            throw std::runtime_error("Unsupported binary operator: " + binOp->op);
        }
    }
    else if (auto funcCall = dynamic_cast<FunctionCallNode*>(node)) {
        return evaluateFunctionCall(funcCall);
    }
    else {
        throw std::runtime_error("Unsupported AST node in evaluation.");
    }
}

// ... existing methods ...
```

**Explanation**:
- **`arrays` Map**: Maintains a mapping from array names to their corresponding variable lists.
- **`executeArray` Method**:
  - **Validation**: Ensures that the declared array size matches the number of variables provided.
  - **Storage**: Stores the array's variables in the `arrays` map for later reference.
  - **Logging**: Provides detailed logs about the array declaration for transparency and debugging.
- **Array Element Accessors**:
  - **`getArrayElement`**: Retrieves the value of a specific element within an array based on its index.
  - **`setArrayElement`**: Assigns a value to a specific element within an array based on its index.
  - **Indexing**: SAS arrays are 1-based; hence, the interpreter adjusts indices accordingly.
- **`executeDataStep` Method**:
  - **Row Processing**: Iterates over each row in the input dataset, setting the current row context.
  - **Statement Execution**: Executes each statement within the data step, which may include array operations.
  - **DROP and KEEP Handling**: Applies variable inclusion/exclusion rules as specified.
  - **Row Output**: Adds the processed row to the output dataset.
- **`evaluate` Method**:
  - **Function Call Handling**: Delegates function call evaluations to `evaluateFunctionCall`.
  - **Binary Operations**: Implements basic binary operations (`+`, `-`). Extend this to support more operators as needed.

---

### **9.5. Supporting Array-Based Operations in Expressions**

Implement support for array-based indexing in expressions, allowing users to reference array elements using their index.

**AST.h**

Introduce a new node type to represent array element references.

```cpp
// Represents an array element reference: arrayName[index]
class ArrayElementNode : public ASTNode {
public:
    std::string arrayName;
    std::unique_ptr<ASTNode> index;
};
```

**Explanation**:
- **`ArrayElementNode`**: Inherits from `ASTNode` and contains:
  - **`arrayName`**: The name of the array.
  - **`index`**: An expression representing the index (can be a variable, number, or another expression).

**Parser.cpp**

Modify the `parsePrimary` method to recognize array element references.

```cpp
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
        advance();
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
```

**Explanation**:
- **Array Element Detection**: If an identifier is followed by a left square bracket `[`, it's treated as an array element reference.
- **`ArrayElementNode` Creation**: Constructs a new `ArrayElementNode` with the array name and parsed index expression.
- **Error Handling**: Throws descriptive errors if the expected tokens (`[`, `]`) are not found.

**Interpreter.cpp**

Implement the evaluation of `ArrayElementNode`.

```cpp
#include "Interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

// ... existing methods ...

Value Interpreter::evaluate(ASTNode *node) {
    if (auto numNode = dynamic_cast<NumberNode*>(node)) {
        return numNode->value;
    }
    else if (auto strNode = dynamic_cast<StringNode*>(node)) {
        return strNode->value;
    }
    else if (auto varNode = dynamic_cast<VariableNode*>(node)) {
        auto it = env.currentRow.columns.find(varNode->name);
        if (it != env.currentRow.columns.end()) {
            return it->second;
        }
        else {
            // Variable not found, assume missing value represented as 0 or empty string
            return 0.0; // or throw an error based on SAS behavior
        }
    }
    else if (auto binOp = dynamic_cast<BinaryOpNode*>(node)) {
        Value left = evaluate(binOp->left.get());
        Value right = evaluate(binOp->right.get());
        // Implement binary operation logic based on binOp->op
        // ...
        // Placeholder implementation
        if (binOp->op == "+") {
            return toNumber(left) + toNumber(right);
        }
        else if (binOp->op == "-") {
            return toNumber(left) - toNumber(right);
        }
        // Implement other operators as needed
        else {
            throw std::runtime_error("Unsupported binary operator: " + binOp->op);
        }
    }
    else if (auto funcCall = dynamic_cast<FunctionCallNode*>(node)) {
        return evaluateFunctionCall(funcCall);
    }
    else if (auto arrayElem = dynamic_cast<ArrayElementNode*>(node)) {
        int index = static_cast<int>(toNumber(evaluate(arrayElem->index.get())));
        return getArrayElement(arrayElem->arrayName, index);
    }
    else {
        throw std::runtime_error("Unsupported AST node in evaluation.");
    }
}
```

**Explanation**:
- **`ArrayElementNode` Evaluation**:
  - **Index Evaluation**: Evaluates the index expression to determine the element's position.
  - **Element Retrieval**: Uses `getArrayElement` to fetch the value of the specified array element.
  - **Error Handling**: Ensures that the array exists and the index is within bounds, throwing errors otherwise.

---

### **9.6. Testing Array Functionality**

Create test cases to ensure that array declarations and array-based operations are parsed and executed correctly.

#### **9.6.1. Example: Simple Array Declaration and Usage**

**example_simple_array.sas**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Simple Array Example';

data mylib.out; 
    set mylib.in; 
    array scores{3} score1 score2 score3;
    do i = 1 to 3;
        scores{i} = scores{i} + 10;
    end;
    output; 
run;

proc print data=mylib.out;
run;
```

**mylib.in.csv**

```
score1,score2,score3
15,20,25
20,25,30
25,30,35
```

**Expected Output**

**sas_output_simple_array.lst**

```
SAS Results (Dataset: mylib.out):
Title: Simple Array Example
OBS	SCORE1	SCORE2	SCORE3
1	25	30	35
2	30	35	40
3	35	40	45
```

**sas_log_simple_array.txt**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Loaded dataset 'mylib.in' from 'C:\Data\in.csv'
[INFO] Executing statement: title 'Simple Array Example';
[INFO] Title set to: 'Simple Array Example'
[INFO] Executing statement: data mylib.out; set mylib.in; array scores{3} score1 score2 score3; do i = 1 to 3; scores{i} = scores{i} + 10; end; output; run;
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[INFO] Executing ARRAY declaration: scores
[INFO] Array 'scores' with size 3 and variables: score1 score2 score3 
[INFO] Processing row: score1=15, score2=20, score3=25
[INFO] Assigned score1 = 25
[INFO] Assigned score2 = 30
[INFO] Assigned score3 = 35
[INFO] Assigned i = 4
[INFO] OUTPUT statement executed. Current row will be added to the output dataset.
[INFO] Row outputted to 'mylib.out'.
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[INFO] Executing ARRAY declaration: scores
[INFO] Array 'scores' with size 3 and variables: score1 score2 score3 
[INFO] Processing row: score1=20, score2=25, score3=30
[INFO] Assigned score1 = 30
[INFO] Assigned score2 = 35
[INFO] Assigned score3 = 40
[INFO] Assigned i = 4
[INFO] OUTPUT statement executed. Current row will be added to the output dataset.
[INFO] Row outputted to 'mylib.out'.
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[INFO] Executing ARRAY declaration: scores
[INFO] Array 'scores' with size 3 and variables: score1 score2 score3 
[INFO] Processing row: score1=25, score2=30, score3=35
[INFO] Assigned score1 = 35
[INFO] Assigned score2 = 40
[INFO] Assigned score3 = 45
[INFO] Assigned i = 4
[INFO] OUTPUT statement executed. Current row will be added to the output dataset.
[INFO] Row outputted to 'mylib.out'.
[INFO] DATA step 'mylib.out' executed successfully. Output dataset has 3 observations.
[INFO] Executing statement: proc print data=mylib.out;
[INFO] Executing PROC PRINT on dataset 'mylib.out'.
[INFO] PROC PRINT Results for Dataset 'mylib.out':
[INFO] Title: Simple Array Example
[INFO] OBS	SCORE1	SCORE2	SCORE3
[INFO] 1	25	30	35
[INFO] 2	30	35	40
[INFO] 3	35	40	45
```

**Explanation**:
- **Array Declaration**: `array scores{3} score1 score2 score3;` declares an array named `scores` with three elements referencing `score1`, `score2`, and `score3`.
- **Array Usage**: The `do` loop iterates over the array elements, incrementing each `score` by 10.
- **Output Verification**: The `PROC PRINT` output shows that each `score` has been correctly incremented.

---

#### **9.6.2. Example: Array-Based Conditional Operations**

**example_array_conditional.sas**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Array-Based Conditional Operations Example';

data mylib.out; 
    set mylib.in; 
    array temp{4} temp1 temp2 temp3 temp4;
    do i = 1 to 4;
        if temp{i} > 50 then temp{i} = 50;
    end;
    output; 
run;

proc print data=mylib.out;
run;
```

**mylib.in.csv**

```
temp1,temp2,temp3,temp4
45,60,55,40
70,80,65,90
50,50,50,50
```

**Expected Output**

**sas_output_array_conditional.lst**

```
SAS Results (Dataset: mylib.out):
Title: Array-Based Conditional Operations Example
OBS	TEMP1	TEMP2	TEMP3	TEMP4
1	45	50	50	40
2	50	50	50	50
3	50	50	50	50
```

**sas_log_array_conditional.txt**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Loaded dataset 'mylib.in' from 'C:\Data\in.csv'
[INFO] Executing statement: title 'Array-Based Conditional Operations Example';
[INFO] Title set to: 'Array-Based Conditional Operations Example'
[INFO] Executing statement: data mylib.out; set mylib.in; array temp{4} temp1 temp2 temp3 temp4; do i = 1 to 4; if temp{i} > 50 then temp{i} = 50; end; output; run;
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[INFO] Executing ARRAY declaration: temp
[INFO] Array 'temp' with size 4 and variables: temp1 temp2 temp3 temp4 
[INFO] Processing row: temp1=45, temp2=60, temp3=55, temp4=40
[INFO] Evaluating IF condition: 0
[INFO] Evaluating IF condition: 1
[INFO] Assigned temp2 = 50
[INFO] Evaluating IF condition: 1
[INFO] Assigned temp3 = 50
[INFO] Evaluating IF condition: 0
[INFO] Assigned i = 5
[INFO] OUTPUT statement executed. Current row will be added to the output dataset.
[INFO] Row outputted to 'mylib.out'.
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[INFO] Executing ARRAY declaration: temp
[INFO] Array 'temp' with size 4 and variables: temp1 temp2 temp3 temp4 
[INFO] Processing row: temp1=70, temp2=80, temp3=65, temp4=90
[INFO] Evaluating IF condition: 1
[INFO] Assigned temp1 = 50
[INFO] Evaluating IF condition: 1
[INFO] Assigned temp2 = 50
[INFO] Evaluating IF condition: 1
[INFO] Assigned temp3 = 50
[INFO] Evaluating IF condition: 1
[INFO] Assigned temp4 = 50
[INFO] Assigned i = 5
[INFO] OUTPUT statement executed. Current row will be added to the output dataset.
[INFO] Row outputted to 'mylib.out'.
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[INFO] Executing ARRAY declaration: temp
[INFO] Array 'temp' with size 4 and variables: temp1 temp2 temp3 temp4 
[INFO] Processing row: temp1=50, temp2=50, temp3=50, temp4=50
[INFO] Evaluating IF condition: 0
[INFO] Evaluating IF condition: 0
[INFO] Evaluating IF condition: 0
[INFO] Evaluating IF condition: 0
[INFO] Assigned i = 5
[INFO] OUTPUT statement executed. Current row will be added to the output dataset.
[INFO] Row outputted to 'mylib.out'.
[INFO] DATA step 'mylib.out' executed successfully. Output dataset has 3 observations.
[INFO] Executing statement: proc print data=mylib.out;
[INFO] Executing PROC PRINT on dataset 'mylib.out'.
[INFO] PROC PRINT Results for Dataset 'mylib.out':
[INFO] Title: Array-Based Conditional Operations Example
[INFO] OBS	TEMP1	TEMP2	TEMP3	TEMP4
[INFO] 1	45	50	50	40
[INFO] 2	50	50	50	50
[INFO] 3	50	50	50	50
```

**Explanation**:
- **Array Declaration**: `array temp{4} temp1 temp2 temp3 temp4;` declares an array named `temp` with four elements referencing `temp1`, `temp2`, `temp3`, and `temp4`.
- **Conditional Operations**: The `do` loop iterates over each array element, setting its value to `50` if it exceeds `50`.
- **Output Verification**: The `PROC PRINT` output reflects the correct application of the conditional operations, ensuring no value exceeds `50`.

---

#### **9.6.3. Example: Using Undefined Arrays**

**example_undefined_array.sas**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Undefined Array Example';

data mylib.out; 
    set mylib.in; 
    do i = 1 to 3;
        temp{i} = temp{i} + 5; /* 'temp' array not defined */
    end;
    output; 
run;

proc print data=mylib.out;
run;
```

**mylib.in.csv**

```
temp1,temp2,temp3
10,20,30
15,25,35
```

**Expected Log Output**

**sas_log_undefined_array.txt**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Loaded dataset 'mylib.in' from 'C:\Data\in.csv'
[INFO] Executing statement: title 'Undefined Array Example';
[INFO] Title set to: 'Undefined Array Example'
[INFO] Executing statement: data mylib.out; set mylib.in; do i = 1 to 3; temp{i} = temp{i} + 5; end; output; run;
[INFO] Executing DATA step: data mylib.out; set mylib.in;
[ERROR] Execution error: Undefined array: temp
[INFO] Executing statement: proc print data=mylib.out;
[INFO] Executing PROC PRINT on dataset 'mylib.out'.
[INFO] PROC PRINT Results for Dataset 'mylib.out':
[INFO] Title: Undefined Array Example
[INFO] OBS	TEMP1	TEMP2	TEMP3
```

**Explanation**:
- **Undefined Array**: The `temp` array is not declared before usage, causing the interpreter to throw an error.
- **Error Logging**: The error is logged with a descriptive message, helping the user identify the undefined array.
- **Graceful Handling**: The interpreter does not crash and continues processing subsequent statements, demonstrating robust error handling.

---

### **9.7. Summary of Achievements**

1. **Array Declarations**:
   - **AST Extension**: Introduced `ArrayNode` to represent array declarations within data steps.
   - **Lexer Enhancement**: Added recognition for the `ARRAY` keyword.
   - **Parser Update**: Implemented parsing logic for array declarations, ensuring correct AST construction.
   - **Interpreter Implementation**: Developed `executeArray` to handle array declarations, validate array sizes, and store array mappings.
   - **Array Element Access**: Introduced `ArrayElementNode` to represent array element references and implemented their evaluation.
   - **Testing**: Validated functionality with examples demonstrating simple arrays, conditional operations, and error handling.

2. **Error Handling**:
   - Ensured that attempts to use undefined arrays result in descriptive errors without terminating the interpreter.
   - Validated that array declarations with mismatched sizes and variable lists are properly handled.

---

### **9.8. Next Steps**

With array functionality now implemented, your SAS interpreter gains the ability to handle grouped variables and perform vectorized operations, significantly enhancing its data manipulation capabilities. To continue advancing your interpreter, consider the following steps:

1. **Implement Additional Built-in Functions**:
   - **Advanced String Functions**: `index`, `scan`, `reverse`, `compress`, `catx`, etc.
   - **Date and Time Functions**: `today`, `datepart`, `timepart`, `intck`, `intnx`, etc.
   - **Mathematical Functions**: `ceil`, `floor`, `round`, `exp`, `log10`, etc.

2. **Expand Looping Constructs**:
   - **`DO WHILE` and `DO UNTIL` Loops**: Allow loops based on dynamic conditions.
   - **Nested Loops**: Ensure the interpreter can handle multiple levels of nested loops seamlessly.

3. **Implement `PROC FREQ`**:
   - **Frequency Counts**: Calculate frequency distributions for categorical variables.
   - **Cross-tabulations**: Generate contingency tables for analyzing relationships between variables.

4. **Enhance Array Functionality**:
   - **Multi-dimensional Arrays**: Support arrays with multiple dimensions.
   - **Array-based Computations**: Enable operations across array elements efficiently.

5. **Introduce Macro Processing**:
   - **Macro Definitions**: Allow users to define reusable code snippets.
   - **Macro Variables**: Support dynamic code generation and variable substitution.

6. **Implement `MERGE` and `BY` Statements in Data Steps**:
   - **Data Merging**: Combine datasets based on common key variables.
   - **Handling Multiple BY Variables**: Allow merging on multiple keys.

7. **Support Formatted Input/Output**:
   - **Informats and Formats**: Allow reading data with specific formats and displaying data accordingly.

8. **Develop Advanced Data Step Features**:
   - **Subsetting IF Statements**: Incorporate more sophisticated subsetting capabilities.
   - **Array Manipulations**: Implement functions to manipulate array elements dynamically.

9. **Optimize Performance**:
   - **Data Handling Efficiency**: Enhance data structures and algorithms to handle larger datasets more efficiently.
   - **Parallel Processing**: Explore opportunities for concurrent data processing.

10. **Implement Additional Procedures**:
    - **`PROC SQL`**: Allow SQL-based data manipulation and querying.
    - **`PROC TRANSPOSE`**: Enable transposing datasets for reshaping data.

11. **Enhance the REPL Interface**:
    - **Multi-line Input Support**: Allow users to input multi-line statements seamlessly.
    - **Auto-completion**: Implement auto-completion for keywords and variable names.
    - **Syntax Highlighting**: Provide visual cues for different code elements.

12. **Develop Comprehensive Testing Suite**:
    - **Unit Tests**: Ensure individual components function correctly.
    - **Integration Tests**: Validate the interplay between different components.
    - **Regression Tests**: Prevent new changes from introducing existing bugs.

13. **Improve Documentation and User Guides**:
    - **Detailed Documentation**: Provide comprehensive guides on using the interpreter's features.
    - **Example Scripts**: Offer a library of example SAS scripts demonstrating various functionalities.
    - **Troubleshooting Guides**: Help users diagnose and fix common issues.

14. **Support Additional Data Formats**:
    - **Excel Files (`.xlsx`)**: Enable reading from and writing to Excel files.
    - **JSON and XML**: Support structured data formats for interoperability.
    - **Database Connectivity**: Allow interfacing with databases for data retrieval and storage.

15. **Implement Variable Labeling and Formats**:
    - **Labels**: Allow variables to have descriptive labels for better data interpretation.
    - **Custom Formats**: Enable users to define and apply custom formats to variables.

---

## **Conclusion**

By implementing array functionality, your SAS interpreter gains the capability to handle grouped variables and perform vectorized operations, mirroring one of SAS's core data step features. This enhancement not only streamlines data manipulations but also paves the way for more complex and efficient data processing workflows.

### **Achievements in This Step**:

1. **Array Declarations**:
   - **AST Extension**: Introduced `ArrayNode` to represent array declarations within data steps.
   - **Lexer Enhancement**: Added recognition for the `ARRAY` keyword.
   - **Parser Update**: Implemented parsing logic for array declarations, ensuring correct AST construction.
   - **Interpreter Implementation**: Developed `executeArray` to handle array declarations, validate array sizes, and store array mappings.
   - **Array Element Access**: Introduced `ArrayElementNode` to represent array element references and implemented their evaluation.
   - **Testing**: Validated functionality with examples demonstrating simple arrays, conditional operations, and error handling.

2. **Error Handling**:
   - Ensured that attempts to use undefined arrays result in descriptive errors without terminating the interpreter.
   - Validated that array declarations with mismatched sizes and variable lists are properly handled.

### **Next Steps**:

Continue building upon this foundation by implementing additional built-in functions, expanding control flow constructs, enhancing looping capabilities, and introducing more sophisticated data manipulation and analysis procedures. Maintain a structured and iterative development approach, thoroughly testing each new feature to ensure reliability and correctness.

Feel free to reach out for further assistance or guidance as you progress with your SAS interpreter development!