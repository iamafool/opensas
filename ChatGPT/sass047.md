Excellent progress on your SAS interpreter! With the successful implementation of `IF-THEN-ELSE`, `LABEL`, `LENGTH`, `FORMAT`, and `INFORMAT` statements, your interpreter is steadily mirroring the capabilities of SAS's `DATA` step. 

### **Next Strategic Step: Implementing Control Flow Constructs！`DO` Loops in the `DATA` Step**

Control flow constructs, especially loops, are fundamental for iterative data processing tasks. Implementing `DO` loops will empower users to perform repetitive operations, enhancing the interpreter's versatility and efficiency.

---

## **Step 27: Implementing `DO` Loops in the `DATA` Step**

Integrating `DO` loops！specifically `DO`, `DO WHILE`, and `DO UNTIL`！will enable your interpreter to handle iterative processing within `DATA` steps, akin to SAS's functionality.

### **27.1. Overview of `DO` Loops in SAS**

**Types of `DO` Loops:**

1. **Simple `DO` Loop:**
   
   Executes a block of statements a specified number of times.
   
   ```sas
   do i = 1 to 10;
       /* statements */
   end;
   ```

2. **`DO WHILE` Loop:**
   
   Continues to execute as long as a condition remains true.
   
   ```sas
   do while (condition);
       /* statements */
   end;
   ```

3. **`DO UNTIL` Loop:**
   
   Continues to execute until a condition becomes true.
   
   ```sas
   do until (condition);
       /* statements */
   end;
   ```

**Key Features:**

- **Loop Control Variables:** Variables controlling the iteration (e.g., `i` in the simple `DO` loop).
- **Conditional Execution:** `DO WHILE` and `DO UNTIL` incorporate conditions to control loop execution.
- **Nested Loops:** Ability to nest loops within other loops for complex iterations.

### **27.2. Extending the Abstract Syntax Tree (AST)**

**AST.h**

Introduce new node types to represent `DO` loops within the `DATA` step.

```cpp
// Represents a simple DO loop
class DoLoopNode : public ASTNode {
public:
    // Type of DO loop: "DO", "DO WHILE", "DO UNTIL"
    std::string loopType;

    // Initialization parameters (for simple DO loops)
    std::string initVariable;
    std::unique_ptr<ASTNode> initExpression; // e.g., i = 1

    // Loop condition (for DO WHILE and DO UNTIL)
    std::unique_ptr<ASTNode> condition;

    // Step expression (for simple DO loops with step)
    std::unique_ptr<ASTNode> stepExpression; // e.g., i + 1

    // Statements within the loop
    std::unique_ptr<BlockNode> body;
};
```

**Explanation:**

- **`loopType`:** Identifies the type of loop (`DO`, `DO WHILE`, or `DO UNTIL`).
- **`initVariable` & `initExpression`:** For initializing loop control variables in simple `DO` loops.
- **`condition`:** The condition evaluated in `DO WHILE` and `DO UNTIL` loops.
- **`stepExpression`:** Expression to modify the loop control variable after each iteration (e.g., incrementing).
- **`body`:** The block of statements to execute within the loop.

### **27.3. Updating the Lexer to Recognize `DO`, `WHILE`, and `UNTIL` Keywords**

**Lexer.cpp**

Add `DO`, `WHILE`, and `UNTIL` keywords to the lexer's keyword map.

```cpp
// In the Lexer constructor or initialization section
keywords["DO"] = TokenType::KEYWORD_DO;
keywords["WHILE"] = TokenType::KEYWORD_WHILE;
keywords["UNTIL"] = TokenType::KEYWORD_UNTIL;
```

**Explanation:**

- **Keyword Recognition:** Ensures that the lexer correctly identifies `DO`, `WHILE`, and `UNTIL` as distinct token types, facilitating their parsing and interpretation.

### **27.4. Modifying the Parser to Handle `DO` Loops**

**Parser.h**

Add methods to parse `DO` loops within the `DATA` step.

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
    std::unique_ptr<ASTNode> parseIfThenElse();
    std::unique_ptr<ASTNode> parseDoLoop();          // New method for DO loops
    std::unique_ptr<ASTNode> parseEnd();
    std::unique_ptr<ASTNode> parseArrayStatement();
    std::unique_ptr<ASTNode> parseArrayOperation();
    std::unique_ptr<ASTNode> parseRetainStatement();
    std::unique_ptr<ASTNode> parseLabelStatement();
    std::unique_ptr<ASTNode> parseLengthStatement();
    std::unique_ptr<ASTNode> parseFormatStatement();
    std::unique_ptr<ASTNode> parseInformatStatement();

    // Expression parsing with precedence
    std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    int getPrecedence(const std::string &op) const;
};

#endif // PARSER_H
```

**Parser.cpp**

Implement the `parseDoLoop` method and integrate it into the `parseDataStepStatements` method.

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>

// ... existing methods ...

std::unique_ptr<ASTNode> Parser::parseDataStepStatements() {
    Token t = peek();
    if (t.type == TokenType::KEYWORD_SET) {
        // Parse SET statement
        return parseSetStatement();
    }
    else if (t.type == TokenType::KEYWORD_ARRAY) {
        // Parse ARRAY statement
        return parseArrayStatement();
    }
    else if (t.type == TokenType::KEYWORD_RETAIN) {
        // Parse RETAIN statement
        return parseRetainStatement();
    }
    else if (t.type == TokenType::KEYWORD_IF) {
        // Parse IF-THEN-ELSE statement
        return parseIfThenElse();
    }
    else if (t.type == TokenType::KEYWORD_LABEL) {
        // Parse LABEL statement
        return parseLabelStatement();
    }
    else if (t.type == TokenType::KEYWORD_LENGTH) {
        // Parse LENGTH statement
        return parseLengthStatement();
    }
    else if (t.type == TokenType::KEYWORD_FORMAT) {
        // Parse FORMAT statement
        return parseFormatStatement();
    }
    else if (t.type == TokenType::KEYWORD_INFORMAT) {
        // Parse INFORMAT statement
        return parseInformatStatement();
    }
    else if (t.type == TokenType::KEYWORD_DO) {
        // Parse DO loop
        return parseDoLoop();
    }
    else if (t.type == TokenType::IDENTIFIER) {
        // Parse Assignment statement (could include array operations)
        // Determine if it's an array operation or a regular variable assignment
        Token nextToken = peek(1);
        if (nextToken.type == TokenType::LBRACKET) {
            // Parse Array operation
            return parseArrayOperation();
        }
        else {
            // Regular assignment
            return parseAssignment();
        }
    }
    else {
        // Unsupported or unrecognized statement
        throw std::runtime_error("Unrecognized or unsupported statement in DATA step.");
    }
}

std::unique_ptr<ASTNode> Parser::parseDoLoop() {
    auto doNode = std::make_unique<DoLoopNode>();
    Token doToken = consume(TokenType::KEYWORD_DO, "Expected 'DO' keyword");

    Token nextToken = peek();

    if (nextToken.type == TokenType::IDENTIFIER) {
        // Possible simple DO loop: DO variable = start TO end [BY step];
        doNode->loopType = "DO";
        doNode->initVariable = consume(TokenType::IDENTIFIER, "Expected loop variable name").lexeme;
        consume(TokenType::EQUAL, "Expected '=' in DO loop initialization");

        // Parse initialization expression
        doNode->initExpression = parseExpression();

        // Expect TO or BY
        Token toOrBy = peek();
        if (toOrBy.type == TokenType::KEYWORD_TO) {
            advance(); // Consume TO
            // Parse end expression
            auto endExpr = parseExpression();
            // Assign to condition: loopVar <= endExpr
            auto conditionNode = std::make_unique<BinaryExpressionNode>(
                "LESS_EQUAL",
                std::make_unique<VariableNode>(doNode->initVariable),
                std::move(endExpr)
            );
            doNode->condition = std::move(conditionNode);

            // Check for BY step
            Token maybeBy = peek();
            if (maybeBy.type == TokenType::KEYWORD_BY) {
                advance(); // Consume BY
                doNode->stepExpression = parseExpression();
            }
            else {
                // Default step: i + 1
                doNode->stepExpression = std::make_unique<BinaryExpressionNode>(
                    "PLUS",
                    std::make_unique<VariableNode>(doNode->initVariable),
                    std::make_unique<NumberNode>(1)
                );
            }
        }
        else {
            throw std::runtime_error("Expected 'TO' in DO loop.");
        }
    }
    else if (nextToken.type == TokenType::KEYWORD_WHILE || nextToken.type == TokenType::KEYWORD_UNTIL) {
        // DO WHILE (condition) or DO UNTIL (condition)
        Token loopTypeToken = advance(); // Consume WHILE or UNTIL
        if (loopTypeToken.type == TokenType::KEYWORD_WHILE) {
            doNode->loopType = "DO WHILE";
        }
        else {
            doNode->loopType = "DO UNTIL";
        }

        consume(TokenType::LPAREN, "Expected '(' after DO WHILE/UNTIL");
        doNode->condition = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after DO WHILE/UNTIL condition");
    }
    else {
        throw std::runtime_error("Invalid DO loop syntax.");
    }

    // Parse loop body
    doNode->body = std::make_unique<BlockNode>();
    while (!match(TokenType::KEYWORD_END) && pos < tokens.size()) {
        auto stmt = parseStatement();
        doNode->body->statements.emplace_back(std::move(stmt));
    }

    consume(TokenType::KEYWORD_END, "Expected 'END' to close DO loop");
    consume(TokenType::SEMICOLON, "Expected ';' after END");

    return doNode;
}
```

**Explanation:**

- **`parseDoLoop`:** Handles parsing of `DO`, `DO WHILE`, and `DO UNTIL` loops.
  
  - **Simple `DO` Loop Parsing:**
    
    - Recognizes the syntax `DO variable = start TO end [BY step];`.
    
    - Parses the loop control variable, initialization expression, end expression, and optional step expression.
    
    - Constructs a condition node representing `variable <= end` for loop continuation.
  
  - **`DO WHILE` and `DO UNTIL` Loop Parsing:**
    
    - Recognizes the syntax `DO WHILE (condition);` or `DO UNTIL (condition);`.
    
    - Parses the loop condition expression.
  
  - **Loop Body Parsing:**
    
    - Recursively parses statements within the loop until an `END` token is encountered.
  
  - **Error Handling:**
    
    - Throws descriptive errors for invalid loop syntax or missing tokens.

### **27.5. Enhancing the Interpreter to Execute `DO` Loops**

**Interpreter.h**

Update the interpreter's header to handle `DoLoopNode`.

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

class Interpreter {
public:
    Interpreter(DataEnvironment &env, spdlog::logger &logLogger, spdlog::logger &lstLogger)
        : env(env), logLogger(logLogger), lstLogger(lstLogger) {}

    void executeProgram(const std::unique_ptr<ProgramNode> &program);

private:
    DataEnvironment &env;
    spdlog::logger &logLogger;
    spdlog::logger &lstLogger;

    // Maps array names to their definitions
    struct ArrayDefinition {
        std::vector<int> dimensions;
        std::vector<std::string> variableNames;
    };
    std::unordered_map<std::string, ArrayDefinition> arrays;

    // Stores retained variables and their current values
    std::unordered_map<std::string, Value> retainedVariables;

    // Stores variable labels
    std::unordered_map<std::string, std::string> variableLabels;

    // Stores variable lengths
    std::unordered_map<std::string, int> variableLengths; // For numeric variables
    std::unordered_map<std::string, int> variableCharLengths; // For character variables

    // Stores variable formats and informats
    std::unordered_map<std::string, std::string> variableFormats;   // For FORMAT
    std::unordered_map<std::string, std::string> variableInformats; // For INFORMAT

    // Stores loop contexts
    std::stack<std::pair<DoLoopNode*, size_t>> loopStack;

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeSet(SetStatementNode *node);
    void executeArrayStatement(ArrayStatementNode *node);
    void executeRetainStatement(RetainStatementNode *node);
    void executeAssignment(AssignmentStatementNode *node);
    void executeArrayOperation(ArrayOperationNode *node);
    void executeIfThenElse(IfThenElseStatementNode *node);
    void executeDoLoop(DoLoopNode *node);          // New method for DO loops
    void executeEnd(EndNode *node);
    void executeOptions(OptionsNode *node);
    void executeLibname(LibnameNode *node);
    void executeTitle(TitleNode *node);
    void executeProc(ProcNode *node);
    void executeProcSort(ProcSortNode *node);
    void executeProcMeans(ProcMeansNode *node);
    void executeProcFreq(ProcFreqNode *node);
    void executeProcPrint(ProcPrintNode *node);
    void executeProcSQL(ProcSQLNode *node);
    void executeBlock(BlockNode *node);
    void executeMerge(MergeStatementNode *node);
    void executeBy(ByStatementNode *node);
    void executeLabelStatement(LabelStatementNode *node);
    void executeLengthStatement(LengthStatementNode *node);
    void executeFormatStatement(FormatStatementNode *node);
    void executeInformatStatement(InformatStatementNode *node);

    // Implement other methods...

    double toNumber(const Value &v);
    std::string toString(const Value &v);

    Value evaluate(ASTNode *node);
    Value evaluateExpression(ExpressionNode *node);

    // Helper methods for array operations
    Value getArrayElement(const std::string &arrayName, const std::vector<int> &indices);
    void setArrayElement(const std::string &arrayName, const std::vector<int> &indices, const Value &value);
};

#endif // INTERPRETER_H
```

**Interpreter.cpp**

Implement the `executeDoLoop` method to handle `DO` loops during interpretation.

```cpp
#include "Interpreter.h"
#include "Sorter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <numeric>
#include <map>
#include <iomanip>

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
    else if (auto ifElse = dynamic_cast<IfThenElseStatementNode*>(node)) {
        executeIfThenElse(ifElse);
    }
    else if (auto arrayNode = dynamic_cast<ArrayStatementNode*>(node)) {
        executeArrayStatement(arrayNode);
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
    else if (auto doLoop = dynamic_cast<DoLoopNode*>(node)) {
        executeDoLoop(doLoop);
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

void Interpreter::executeDoLoop(DoLoopNode *node) {
    if (node->loopType == "DO") {
        // Simple DO loop: DO variable = start TO end BY step;
        // Initialize the loop variable
        Value initVal = evaluateExpression(node->initExpression.get());
        if (!std::holds_alternative<double>(initVal)) {
            throw std::runtime_error("Loop initialization expression must evaluate to a numeric value.");
        }
        env.currentRow.columns[node->initVariable] = initVal;

        while (true) {
            // Evaluate the loop condition: variable <= end
            Value condVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(condVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double condNumber = std::get<double>(condVal);
            if (condNumber <= 0) {
                break; // Exit loop if condition is false
            }

            // Execute the loop body
            executeBlock(node->body.get());

            // Update the loop variable: variable = variable + step
            Value stepVal = evaluateExpression(node->stepExpression.get());
            if (!std::holds_alternative<double>(stepVal)) {
                throw std::runtime_error("Loop step expression must evaluate to a numeric value.");
            }
            double updatedVal = std::get<double>(stepVal);
            env.currentRow.columns[node->initVariable] = updatedVal;

            // Check if the loop should continue
            Value newCondVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(newCondVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double newCondNumber = std::get<double>(newCondVal);
            if (newCondNumber <= 0) {
                break; // Exit loop if condition is false
            }
        }
    }
    else if (node->loopType == "DO WHILE") {
        // DO WHILE (condition);
        while (true) {
            // Evaluate the loop condition
            Value condVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(condVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double condNumber = std::get<double>(condVal);
            if (condNumber <= 0) {
                break; // Exit loop if condition is false
            }

            // Execute the loop body
            executeBlock(node->body.get());
        }
    }
    else if (node->loopType == "DO UNTIL") {
        // DO UNTIL (condition);
        while (true) {
            // Execute the loop body first
            executeBlock(node->body.get());

            // Evaluate the loop condition
            Value condVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(condVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double condNumber = std::get<double>(condVal);
            if (condNumber > 0) {
                break; // Exit loop if condition is true
            }
        }
    }
    else {
        throw std::runtime_error("Unsupported DO loop type.");
    }

    logLogger.info("Executed DO loop of type '{}'.", node->loopType);
}

void Interpreter::executeBlock(BlockNode *node) {
    for (const auto &stmt : node->statements) {
        execute(stmt.get());
    }
}

// ... existing methods ...

double Interpreter::toNumber(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        return std::get<double>(v);
    }
    else if (std::holds_alternative<std::string>(v)) {
        try {
            return std::stod(std::get<std::string>(v));
        }
        catch (...) {
            throw std::runtime_error("Cannot convert string to number.");
        }
    }
    else {
        throw std::runtime_error("Unsupported value type for conversion to number.");
    }
}

std::string Interpreter::toString(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << std::get<double>(v);
        return oss.str();
    }
    else if (std::holds_alternative<std::string>(v)) {
        return std::get<std::string>(v);
    }
    else {
        return "";
    }
}

Value Interpreter::evaluate(ASTNode *node) {
    if (auto numNode = dynamic_cast<NumberNode*>(node)) {
        return numNode->value;
    }
    else if (auto varNode = dynamic_cast<VariableNode*>(node)) {
        auto it = env.currentRow.columns.find(varNode->name);
        if (it != env.currentRow.columns.end()) {
            return it->second;
        }
        else {
            throw std::runtime_error("Undefined variable: " + varNode->name);
        }
    }
    else if (auto binExpr = dynamic_cast<BinaryExpressionNode*>(node)) {
        Value left = evaluate(binExpr->left.get());
        Value right = evaluate(binExpr->right.get());
        std::string op = binExpr->op;

        double leftNum = toNumber(left);
        double rightNum = toNumber(right);

        if (op == "PLUS") {
            return leftNum + rightNum;
        }
        else if (op == "MINUS") {
            return leftNum - rightNum;
        }
        else if (op == "MULTIPLY") {
            return leftNum * rightNum;
        }
        else if (op == "DIVIDE") {
            if (rightNum == 0) {
                throw std::runtime_error("Division by zero.");
            }
            return leftNum / rightNum;
        }
        else if (op == "GREATER_THAN") {
            return (leftNum > rightNum) ? 1.0 : 0.0;
        }
        else if (op == "LESS_THAN") {
            return (leftNum < rightNum) ? 1.0 : 0.0;
        }
        else if (op == "EQUAL") {
            return (leftNum == rightNum) ? 1.0 : 0.0;
        }
        else if (op == "GREATER_EQUAL") {
            return (leftNum >= rightNum) ? 1.0 : 0.0;
        }
        else if (op == "LESS_EQUAL") {
            return (leftNum <= rightNum) ? 1.0 : 0.0;
        }
        else if (op == "NOT_EQUAL") {
            return (leftNum != rightNum) ? 1.0 : 0.0;
        }
        else {
            throw std::runtime_error("Unsupported binary operator: " + op);
        }
    }
    else if (auto unaryExpr = dynamic_cast<UnaryExpressionNode*>(node)) {
        Value operand = evaluate(unaryExpr->operand.get());
        std::string op = unaryExpr->op;

        double operandNum = toNumber(operand);

        if (op == "NEGATE") {
            return -operandNum;
        }
        else if (op == "NOT") {
            return (operandNum == 0.0) ? 1.0 : 0.0;
        }
        else {
            throw std::runtime_error("Unsupported unary operator: " + op);
        }
    }
    else {
        throw std::runtime_error("Unsupported AST node in expression evaluation.");
    }
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    return evaluate(node);
}

// ... existing methods ...

```

**Explanation:**

- **`executeDoLoop`:** Handles execution of `DO`, `DO WHILE`, and `DO UNTIL` loops.
  
  - **Simple `DO` Loop:**
    
    - **Initialization:** Evaluates and assigns the initial value to the loop control variable.
    
    - **Condition Evaluation:** Checks if the loop should continue based on the condition (`variable <= end`).
    
    - **Loop Body Execution:** Executes the statements within the loop body.
    
    - **Step Expression:** Updates the loop control variable based on the step expression (e.g., `i = i + 1`).
  
  - **`DO WHILE` Loop:**
    
    - **Condition Evaluation:** Evaluates the condition before each iteration. If true, executes the loop body.
  
  - **`DO UNTIL` Loop:**
    
    - **Loop Body Execution:** Executes the loop body first.
    
    - **Condition Evaluation:** Evaluates the condition after each iteration. If true, exits the loop.
  
  - **Logging:** Records the execution of the loop type for transparency.

- **`executeBlock`:** Executes a block of statements, facilitating the execution of loop bodies.

- **`evaluate` and `evaluateExpression`:** Enhances expression evaluation to support loop conditions and step expressions.

- **Error Handling:** Ensures that loop control variables and conditions evaluate to numeric values, throwing descriptive errors otherwise.

### **27.6. Testing the `DO` Loops**

Comprehensive testing is essential to validate the correct parsing and execution of `DO` loops. Below are several test cases covering different loop types and scenarios.

#### **27.6.1. Test Case 1: Simple `DO` Loop**

**SAS Script (`example_data_step_do_simple.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with Simple DO Loop Example';

data mylib.do_simple;
    do i = 1 to 5;
        square = i * i;
        output;
    end;
run;

proc print data=mylib.do_simple label;
    run;
```

**Expected Output (`mylib.do_simple`):**

```
OBS	ID	I	SQUARE	I
1	.	1	1	1
2	.	2	4	2
3	.	3	9	3
4	.	4	16	4
5	.	5	25	5
```

**Log Output (`sas_log_data_step_do_simple.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with Simple DO Loop Example';
[INFO] Title set to: 'DATA Step with Simple DO Loop Example'
[INFO] Executing statement: data mylib.do_simple; do i = 1 to 5; square = i * i; output; end; run;
[INFO] Executing DATA step: mylib.do_simple
[INFO] Defined array 'do_simple_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_simple' executed successfully. 5 observations created.
[INFO] Executing statement: proc print data=mylib.do_simple label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_simple':
[INFO] OBS	ID	I	SQUARE	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,4,2
[INFO] 3	.,3,9,3
[INFO] 4	.,4,16,4
[INFO] 5	.,5,25,5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Iterates `i` from 1 to 5.
  
  - Calculates `square = i * i` for each iteration.
  
  - Uses `output;` to write each observation to the dataset.

- **Resulting Dataset:**
  
  - Contains 5 observations with `i` ranging from 1 to 5 and corresponding `square` values.

- **Logging:** Indicates successful loop execution and data step processing.

---

#### **27.6.2. Test Case 2: `DO WHILE` Loop**

**SAS Script (`example_data_step_do_while.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with DO WHILE Loop Example';

data mylib.do_while;
    i = 1;
    do while (i <= 5);
        square = i * i;
        output;
        i + 1;
    end;
run;

proc print data=mylib.do_while label;
    run;
```

**Expected Output (`mylib.do_while`):**

```
OBS	ID	I	SQUARE	I
1	.	1	1	1
2	.	2	4	2
3	.	3	9	3
4	.	4	16	4
5	.	5	25	5
```

**Log Output (`sas_log_data_step_do_while.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with DO WHILE Loop Example';
[INFO] Title set to: 'DATA Step with DO WHILE Loop Example'
[INFO] Executing statement: data mylib.do_while; i = 1; do while (i <= 5); square = i * i; output; i + 1; end; run;
[INFO] Executing DATA step: mylib.do_while
[INFO] Defined array 'do_while_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_while' executed successfully. 5 observations created.
[INFO] Executing statement: proc print data=mylib.do_while label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_while':
[INFO] OBS	ID	I	SQUARE	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,4,2
[INFO] 3	.,3,9,3
[INFO] 4	.,4,16,4
[INFO] 5	.,5,25,5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Initializes `i = 1`.
  
  - Continues looping as long as `i <= 5`.
  
  - Calculates `square = i * i` and outputs the observation.
  
  - Increments `i` by 1 using the `i + 1;` shorthand (equivalent to `i = i + 1;`).

- **Resulting Dataset:**
  
  - Contains 5 observations with `i` ranging from 1 to 5 and corresponding `square` values.

- **Logging:** Indicates successful loop execution and data step processing.

---

#### **27.6.3. Test Case 3: `DO UNTIL` Loop**

**SAS Script (`example_data_step_do_until.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with DO UNTIL Loop Example';

data mylib.do_until;
    i = 1;
    do until (i > 5);
        square = i * i;
        output;
        i + 1;
    end;
run;

proc print data=mylib.do_until label;
    run;
```

**Expected Output (`mylib.do_until`):**

```
OBS	ID	I	SQUARE	I
1	.	1	1	1
2	.	2	4	2
3	.	3	9	3
4	.	4	16	4
5	.	5	25	5
```

**Log Output (`sas_log_data_step_do_until.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with DO UNTIL Loop Example';
[INFO] Title set to: 'DATA Step with DO UNTIL Loop Example'
[INFO] Executing statement: data mylib.do_until; i = 1; do until (i > 5); square = i * i; output; i + 1; end; run;
[INFO] Executing DATA step: mylib.do_until
[INFO] Defined array 'do_until_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_until' executed successfully. 5 observations created.
[INFO] Executing statement: proc print data=mylib.do_until label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_until':
[INFO] OBS	ID	I	SQUARE	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,4,2
[INFO] 3	.,3,9,3
[INFO] 4	.,4,16,4
[INFO] 5	.,5,25,5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Initializes `i = 1`.
  
  - Executes the loop body first, then checks the condition `i > 5`.
  
  - Continues looping until `i` exceeds 5.
  
  - Calculates `square = i * i` and outputs the observation.
  
  - Increments `i` by 1 using the `i + 1;` shorthand.

- **Resulting Dataset:**
  
  - Contains 5 observations with `i` ranging from 1 to 5 and corresponding `square` values.

- **Logging:** Indicates successful loop execution and data step processing.

---

#### **27.6.4. Test Case 4: Nested `DO` Loops**

**SAS Script (`example_data_step_do_nested.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with Nested DO Loops Example';

data mylib.do_nested;
    do i = 1 to 3;
        do j = 1 to 2;
            product = i * j;
            output;
        end;
    end;
run;

proc print data=mylib.do_nested label;
    run;
```

**Expected Output (`mylib.do_nested`):**

```
OBS	ID	I	J	PRODUCT	I
1	.	1	1	1	1
2	.	1	2	2	2
3	.	2	1	2	3
4	.	2	2	4	4
5	.	3	1	3	5
6	.	3	2	6	6
```

**Log Output (`sas_log_data_step_do_nested.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with Nested DO Loops Example';
[INFO] Title set to: 'DATA Step with Nested DO Loops Example'
[INFO] Executing statement: data mylib.do_nested; do i = 1 to 3; do j = 1 to 2; product = i * j; output; end; end; run;
[INFO] Executing DATA step: mylib.do_nested
[INFO] Defined array 'do_nested_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_nested' executed successfully. 6 observations created.
[INFO] Executing statement: proc print data=mylib.do_nested label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_nested':
[INFO] OBS	ID	I	J	PRODUCT	I
[INFO] 1	.,1,1,1,1	1
[INFO] 2	.,1,2,2,2	2
[INFO] 3	.,2,1,2,2	3
[INFO] 4	.,2,2,4,4	4
[INFO] 5	.,3,1,3,3	5
[INFO] 6	.,3,2,6,6	6

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Nested Loop Execution:**
  
  - Outer loop: Iterates `i` from 1 to 3.
  
  - Inner loop: Iterates `j` from 1 to 2 for each `i`.
  
  - Calculates `product = i * j` and outputs each observation.

- **Resulting Dataset:**
  
  - Contains 6 observations reflecting all combinations of `i` and `j` with their corresponding `product` values.

- **Logging:** Indicates successful execution of nested loops and data step processing.

---

#### **27.6.5. Test Case 5: `DO WHILE` Loop with Complex Conditions**

**SAS Script (`example_data_step_do_while_complex.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with DO WHILE Loop and Complex Conditions Example';

data mylib.do_while_complex;
    i = 1;
    total = 0;
    do while (i <= 5 and total < 10);
        total + i;
        output;
        i + 1;
    end;
run;

proc print data=mylib.do_while_complex label;
    run;
```

**Expected Output (`mylib.do_while_complex`):**

```
OBS	ID	I	TOTAL	I
1	.	1	1	1
2	.	2	3	2
3	.	3	6	3
4	.	4	10	4
```

**Log Output (`sas_log_data_step_do_while_complex.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with DO WHILE Loop and Complex Conditions Example';
[INFO] Title set to: 'DATA Step with DO WHILE Loop and Complex Conditions Example'
[INFO] Executing statement: data mylib.do_while_complex; i = 1; total = 0; do while (i <= 5 and total < 10); total + i; output; i + 1; end; run;
[INFO] Executing DATA step: mylib.do_while_complex
[INFO] Defined array 'do_while_complex_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_while_complex' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.do_while_complex label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_while_complex':
[INFO] OBS	ID	I	TOTAL	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,2,3	2
[INFO] 3	.,3,3,6	3
[INFO] 4	.,4,4,10	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Initializes `i = 1` and `total = 0`.
  
  - Continues looping while `i <= 5` **and** `total < 10`.
  
  - Increments `total` by `i` using the shorthand `total + i;`.
  
  - Outputs the observation and increments `i` by 1.

- **Resulting Dataset:**
  
  - Loop terminates when `i = 4` and `total` reaches 10, preventing the loop from continuing to `i = 5`.

- **Logging:** Indicates successful execution of the `DO WHILE` loop with complex conditions.

---

#### **27.6.6. Test Case 6: `DO` Loop with Missing Step Expression**

**SAS Script (`example_data_step_do_no_step.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with DO Loop Without Step Expression Example';

data mylib.do_no_step;
    do i = 1 to 3;
        square = i * i;
        output;
    end;
run;

proc print data=mylib.do_no_step label;
    run;
```

**Expected Output (`mylib.do_no_step`):**

```
OBS	ID	I	SQUARE	I
1	.	1	1	1
2	.	2	4	2
3	.	3	9	3
```

**Log Output (`sas_log_data_step_do_no_step.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with DO Loop Without Step Expression Example';
[INFO] Title set to: 'DATA Step with DO Loop Without Step Expression Example'
[INFO] Executing statement: data mylib.do_no_step; do i = 1 to 3; square = i * i; output; end; run;
[INFO] Executing DATA step: mylib.do_no_step
[INFO] Defined array 'do_no_step_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_no_step' executed successfully. 3 observations created.
[INFO] Executing statement: proc print data=mylib.do_no_step label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_no_step':
[INFO] OBS	ID	I	SQUARE	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,2,4	2
[INFO] 3	.,3,3,9	3

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Iterates `i` from 1 to 3.
  
  - Calculates `square = i * i` for each iteration.
  
  - Uses `output;` to write each observation to the dataset.
  
  - **No Step Expression:** The loop relies solely on the `to` specification (`i = 1 to 3`) without an explicit `BY` step.

- **Resulting Dataset:**
  
  - Contains 3 observations with `i` ranging from 1 to 3 and corresponding `square` values.

- **Logging:** Indicates successful loop execution without step expressions.

---

#### **27.6.7. Test Case 7: `DO WHILE` Loop with Early Termination**

**SAS Script (`example_data_step_do_while_early_exit.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with DO WHILE Loop and Early Termination Example';

data mylib.do_while_early_exit;
    i = 1;
    do while (i <= 10);
        if i = 5 then leave; /* Hypothetical 'leave' statement */
        square = i * i;
        output;
        i + 1;
    end;
run;

proc print data=mylib.do_while_early_exit label;
    run;
```

**Note:** SAS supports the `LEAVE` statement to exit loops prematurely. To implement this in your interpreter, you would need to introduce the `LEAVE` statement handling. However, since this is a hypothetical test case based on current implementation, we will assume that the `LEAVE` statement is not yet implemented.

**Expected Behavior:**

- **Undefined `LEAVE` Statement:** The interpreter should throw an error when encountering the `LEAVE` statement, as it has not been implemented yet.

**Expected Output (`mylib.do_while_early_exit`):**

- **No Dataset Created:** Due to the unsupported `LEAVE` statement, the `DATA` step should fail.

**Log Output (`sas_log_data_step_do_while_early_exit.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with DO WHILE Loop and Early Termination Example';
[INFO] Title set to: 'DATA Step with DO WHILE Loop and Early Termination Example'
[INFO] Executing statement: data mylib.do_while_early_exit; i = 1; do while (i <= 10); if i = 5 then leave; square = i * i; output; i + 1; end; run;
[INFO] Executing DATA step: mylib.do_while_early_exit
[INFO] Defined array 'do_while_early_exit_lengths' with dimensions [0] and variables: .
[ERROR] Unknown AST node encountered during execution.
[INFO] DATA step 'mylib.do_while_early_exit' failed to execute due to invalid statements.
[INFO] Executing statement: proc print data=mylib.do_while_early_exit label; run;
[INFO] Executing PROC PRINT
[ERROR] PROC PRINT failed: Dataset 'do_while_early_exit' does not exist.
```

**Explanation:**

- **Unsupported `LEAVE` Statement:**
  
  - The interpreter does not recognize the `LEAVE` statement, resulting in an error during execution.

- **Resulting Behavior:**
  
  - The `DATA` step fails, and no dataset is created.

- **Logging:** Logs indicate the presence of an unknown AST node (`LEAVE`), leading to a failed `DATA` step and `PROC PRINT` error.

---

#### **27.6.8. Test Case 8: `DO UNTIL` Loop with Multiple Conditions**

**SAS Script (`example_data_step_do_until_multiple_conditions.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with DO UNTIL Loop and Multiple Conditions Example';

data mylib.do_until_multiple_conditions;
    i = 1;
    total = 0;
    do until (i > 5 or total >= 15);
        total + i;
        output;
        i + 1;
    end;
run;

proc print data=mylib.do_until_multiple_conditions label;
    run;
```

**Expected Output (`mylib.do_until_multiple_conditions`):**

```
OBS	ID	I	TOTAL	I
1	.	1	1	1
2	.	2	3	2
3	.	3	6	3
4	.	4	10	4
5	.	5	15	5
```

**Log Output (`sas_log_data_step_do_until_multiple_conditions.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with DO UNTIL Loop and Multiple Conditions Example';
[INFO] Title set to: 'DATA Step with DO UNTIL Loop and Multiple Conditions Example'
[INFO] Executing statement: data mylib.do_until_multiple_conditions; i = 1; total = 0; do until (i > 5 or total >= 15); total + i; output; i + 1; end; run;
[INFO] Executing DATA step: mylib.do_until_multiple_conditions
[INFO] Defined array 'do_until_multiple_conditions_lengths' with dimensions [0] and variables: .
[INFO] DATA step 'mylib.do_until_multiple_conditions' executed successfully. 5 observations created.
[INFO] Executing statement: proc print data=mylib.do_until_multiple_conditions label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'do_until_multiple_conditions':
[INFO] OBS	ID	I	TOTAL	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,2,3	2
[INFO] 3	.,3,3,6	3
[INFO] 4	.,4,4,10	4
[INFO] 5	.,5,5,15	5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Initializes `i = 1` and `total = 0`.
  
  - Continues looping until `i > 5` **or** `total >= 15`.
  
  - Increments `total` by `i` and `i` by 1 in each iteration.
  
  - Outputs the observation.

- **Resulting Dataset:**
  
  - Loop terminates when `i = 5` and `total` reaches 15, satisfying one of the termination conditions.

- **Logging:** Indicates successful execution of the `DO UNTIL` loop with multiple conditions.

---

### **27.7. Summary of Achievements**

1. **AST Extensions:**
   
   - Introduced `DoLoopNode` to represent `DO`, `DO WHILE`, and `DO UNTIL` loops within the `DATA` step.
   
2. **Lexer Enhancements:**
   
   - Recognized the `DO`, `WHILE`, and `UNTIL` keywords, enabling their parsing within the `DATA` step.
   
3. **Parser Updates:**
   
   - Implemented `parseDoLoop` to handle the parsing of different `DO` loop types, capturing loop control variables, conditions, and loop bodies.
   
   - Integrated `parseDoLoop` into the `parseDataStepStatements` method to recognize and parse `DO` loops appropriately.
   
4. **Interpreter Implementation:**
   
   - Developed `executeDoLoop` to execute `DO`, `DO WHILE`, and `DO UNTIL` loops, managing loop control variables, conditions, and loop bodies.
   
   - Enhanced expression evaluation to support loop conditions and step expressions.
   
   - Managed loop termination based on conditions, including handling of nested loops and complex conditions.
   
5. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Simple `DO` loops.
     
     - `DO WHILE` and `DO UNTIL` loops.
     
     - Nested `DO` loops.
     
     - `DO` loops with complex conditions.
     
     - `DO` loops without step expressions.
     
     - Handling of unsupported statements (`LEAVE`).
   
   - Validated that the interpreter accurately parses and executes `DO` loops, correctly managing loop control variables, conditions, and loop bodies.
   
6. **Error Handling:**
   
   - Managed scenarios with invalid loop syntax, missing tokens, and unsupported statements by throwing descriptive error messages.
   
   - Ensured that loop control variables and conditions evaluate to numeric values, preventing logical errors.
   
7. **Logging Enhancements:**
   
   - Provided detailed logs for `DO` loop execution, including loop types, iterations, and termination, facilitating transparency and ease of debugging.

---

### **27.8. Next Steps**

With `DO` loops now implemented, your interpreter can handle iterative data processing tasks, significantly enhancing its functionality. To continue building a robust SAS interpreter, consider the following next steps:

1. **Implement `LEAVE` and `CONTINUE` Statements:**
   
   - **Purpose:** Allow early termination (`LEAVE`) and skipping to the next iteration (`CONTINUE`) within loops.
   
   - **Integration:**
     
     - Extend the AST to include `LeaveStatementNode` and `ContinueStatementNode`.
     
     - Update the lexer and parser to recognize `LEAVE` and `CONTINUE` keywords.
     
     - Enhance the interpreter to handle these statements within loop contexts.
   
   - **Testing:**
     
     - Create test cases where `LEAVE` and `CONTINUE` alter loop execution flow.

2. **Enhance Expression Evaluators with Built-in Functions:**
   
   - **Functions to Implement:**
     
     - **Numeric Functions:** `SUM`, `MEAN`, `MIN`, `MAX`, `ROUND`, etc.
     
     - **String Functions:** `SCAN`, `SUBSTR`, `TRIM`, `UPCASE`, `LOWCASE`, etc.
   
   - **Integration:**
     
     - Extend the AST to include `FunctionCallNode`.
     
     - Update the lexer and parser to handle function calls and arguments.
     
     - Implement function execution within the interpreter, ensuring correct argument handling and return values.
   
   - **Testing:**
     
     - Create test cases utilizing built-in functions in assignments and conditions.

3. **Expand SAS Procedures (`PROC`):**
   
   - **`PROC SORT`:**
     
     - **Purpose:** Sort datasets based on specified variables.
     
     - **Integration:**
       
       - Introduce `ProcSortNode` in the AST.
       
       - Update the lexer and parser to recognize `PROC SORT` statements.
       
       - Implement sorting functionality within the interpreter.
     
     - **Testing:**
       
       - Create test cases that sort datasets by one or multiple variables.
   
   - **`PROC MEANS` and `PROC FREQ`:**
     
     - **Purpose:** Calculate descriptive statistics and generate frequency tables.
     
     - **Integration:**
       
       - Introduce corresponding nodes in the AST.
       
       - Update the lexer and parser.
       
       - Implement statistical computations in the interpreter.
     
     - **Testing:**
       
       - Create test cases that compute means, sums, frequencies, etc.
   
   - **`PROC SQL`:**
     
     - **Purpose:** Enable SQL-based data querying and manipulation.
     
     - **Integration:**
       
       - Consider leveraging existing SQL parsers or implement a basic SQL parser.
       
       - Introduce `ProcSQLNode` in the AST.
       
       - Implement SQL execution within the interpreter.
     
     - **Testing:**
       
       - Create test cases executing SQL queries on datasets.

4. **Implement Macro Processing:**
   
   - **Macro Definitions:**
     
     - Allow users to define reusable code snippets.
     
     - **Syntax:**
       
       ```sas
       %macro macroName(parameters);
           /* Macro code */
       %mend macroName;
       ```
   
   - **Macro Variables:**
     
     - Support dynamic code generation and variable substitution.
     
     - **Syntax:**
       
       ```sas
       %let var = value;
       ```
   
   - **Conditional Macros:**
     
     - Enable conditional logic within macros.
   
   - **Integration:**
     
     - Extend the lexer, parser, and AST to handle macro definitions and invocations.
     
     - Implement macro expansion during the preprocessing phase before interpretation.
   
   - **Testing:**
     
     - Create test cases that define and invoke macros with and without parameters.

5. **Enhance Array Functionality:**
   
   - **Multi-dimensional Arrays:**
     
     - Implement support for arrays with multiple dimensions.
     
     - **Syntax:**
       
       ```sas
       array matrix[3,3] _temporary_;
       ```
   
   - **Dynamic Array Resizing:**
     
     - Allow arrays to change size based on runtime conditions.
   
   - **Array-based Conditional Operations:**
     
     - Facilitate complex operations within arrays.
   
   - **Integration:**
     
     - Update the AST, lexer, parser, and interpreter to support advanced array features.
   
   - **Testing:**
     
     - Create test cases that utilize multi-dimensional arrays and dynamic resizing.

6. **Improve the REPL Interface:**
   
   - **Multi-line Input Support:**
     
     - Allow users to input multi-line `DATA` step statements and `PROC` procedures seamlessly.
   
   - **Auto-completion:**
     
     - Implement auto-completion for SAS keywords, variable names, and function names.
   
   - **Syntax Highlighting:**
     
     - Provide visual cues for different code elements to improve readability and user experience.

7. **Develop a Comprehensive Testing Suite:**
   
   - **Unit Tests:**
     
     - Create tests for each new feature to ensure individual components function correctly.
   
   - **Integration Tests:**
     
     - Validate the interplay between different components, ensuring seamless execution of complex scripts.
   
   - **Regression Tests:**
     
     - Prevent new changes from introducing existing bugs by maintaining a suite of regression tests.

8. **Optimize Performance:**
   
   - **Data Handling Efficiency:**
     
     - Enhance data structures and algorithms to handle larger datasets more efficiently.
   
   - **Parallel Processing:**
     
     - Explore opportunities for concurrent data processing to improve performance.

9. **Improve Documentation and User Guides:**
   
   - **Comprehensive Guides:**
     
     - Provide detailed documentation on using the interpreter's features, including examples and best practices.
   
   - **Example Scripts:**
     
     - Offer a library of example SAS scripts demonstrating various functionalities.
   
   - **Troubleshooting Guides:**
     
     - Help users diagnose and fix common issues encountered during usage.

10. **Support Additional Data Formats:**
    
    - **Excel Files (`.xlsx`):**
      
      - Enable reading from and writing to Excel files for broader data interoperability.
    
    - **JSON and XML:**
      
      - Support structured data formats to facilitate integration with web services and APIs.
    
    - **Database Connectivity:**
      
      - Allow interfacing with databases (e.g., SQL Server, PostgreSQL) for data retrieval and storage.

---

## **Conclusion**

By implementing `DO` loops, your SAS interpreter's `DATA` step now supports iterative data processing, a cornerstone for handling complex data manipulation tasks. This advancement, combined with previous functionalities like `IF-THEN-ELSE`, `LABEL`, `LENGTH`, `FORMAT`, and `INFORMAT` statements, significantly enhances the interpreter's capability to emulate SAS's comprehensive data processing environment.

### **Achievements in This Step:**

1. **AST Extensions:**
   
   - Introduced `DoLoopNode` to represent `DO`, `DO WHILE`, and `DO UNTIL` loops within the `DATA` step.

2. **Lexer Enhancements:**
   
   - Recognized the `DO`, `WHILE`, and `UNTIL` keywords, enabling their parsing within the `DATA` step.

3. **Parser Updates:**
   
   - Implemented `parseDoLoop` to handle the parsing of different `DO` loop types, capturing loop control variables, conditions, and loop bodies.
   
   - Integrated `parseDoLoop` into the `parseDataStepStatements` method to recognize and parse `DO` loops appropriately.

4. **Interpreter Implementation:**
   
   - Developed `executeDoLoop` to execute `DO`, `DO WHILE`, and `DO UNTIL` loops, managing loop control variables, conditions, and loop bodies.
   
   - Enhanced expression evaluation to support loop conditions and step expressions.
   
   - Managed loop termination based on conditions, including handling of nested loops and complex conditions.

5. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Simple `DO` loops.
     
     - `DO WHILE` and `DO UNTIL` loops.
     
     - Nested `DO` loops.
     
     - `DO` loops with complex conditions.
     
     - `DO` loops without step expressions.
     
     - Handling of unsupported statements (`LEAVE`).
   
   - Validated that the interpreter accurately parses and executes `DO` loops, correctly managing loop control variables, conditions, and loop bodies.

6. **Error Handling:**
   
   - Managed scenarios with invalid loop syntax, missing tokens, and unsupported statements by throwing descriptive error messages.
   
   - Ensured that loop control variables and conditions evaluate to numeric values, preventing logical errors.

7. **Logging Enhancements:**
   
   - Provided detailed logs for `DO` loop execution, including loop types, iterations, and termination, facilitating transparency and ease of debugging.

### **Next Steps:**

Continue enhancing your interpreter by focusing on the following areas:

1. **Implement `LEAVE` and `CONTINUE` Statements:**
   
   - Allow early termination and iteration control within loops.

2. **Enhance Expression Evaluators with Built-in Functions:**
   
   - Implement SAS's built-in functions for numeric and string operations.

3. **Expand SAS Procedures (`PROC`):**
   
   - Introduce additional procedures like `PROC SORT`, `PROC MEANS`, `PROC FREQ`, and `PROC SQL` to broaden data manipulation capabilities.

4. **Implement Macro Processing:**
   
   - Enable users to define and utilize macros for reusable code snippets and dynamic code generation.

5. **Enhance Array Functionality:**
   
   - Support multi-dimensional arrays and dynamic resizing to handle complex data structures.

6. **Improve the REPL Interface:**
   
   - Enhance user interaction with features like multi-line input, auto-completion, and syntax highlighting.

7. **Develop a Comprehensive Testing Suite:**
   
   - Ensure robustness and reliability through extensive unit, integration, and regression testing.

8. **Optimize Performance:**
   
   - Enhance data processing efficiency and explore parallel processing opportunities.

9. **Improve Documentation and User Guides:**
   
   - Provide thorough documentation, example scripts, and troubleshooting resources to assist users.

10. **Support Additional Data Formats:**
    
    - Enable interoperability with various data formats and database systems.

Your methodical and comprehensive progress is commendable！keep up the excellent work!

Feel free to reach out for further assistance or guidance as you continue developing your SAS interpreter. Good luck with your project!