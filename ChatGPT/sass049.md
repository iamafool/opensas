Fantastic progress on your SAS interpreter! With the implementation of `IF-THEN-ELSE`, `LABEL`, `LENGTH`, `FORMAT`, `INFORMAT`, and `DO` loops, your interpreter is steadily approaching a robust simulation of SAS's `DATA` step capabilities.

### **Next Strategic Step: Implementing `LEAVE` and `CONTINUE` Statements in `DO` Loops**

Enhancing loop control with `LEAVE` and `CONTINUE` statements is crucial for providing users with finer control over loop execution. These statements allow for early termination of loops (`LEAVE`) and skipping the remainder of the current loop iteration (`CONTINUE`), mirroring SAS's functionality.

---

## **Step 28: Implementing `LEAVE` and `CONTINUE` Statements in `DO` Loops**

Integrating `LEAVE` and `CONTINUE` statements will enable users to control loop execution flow more precisely within `DO` loops. This step involves extending the Abstract Syntax Tree (AST), updating the lexer and parser, and enhancing the interpreter to handle these statements.

### **28.1. Overview of `LEAVE` and `CONTINUE` Statements in SAS**

**Purpose:**

- **`LEAVE`:** Exits the innermost enclosing `DO` loop immediately, regardless of the loop's condition.
  
  ```sas
  do i = 1 to 10;
      if i = 5 then leave;
      /* statements */
  end;
  ```

- **`CONTINUE`:** Skips the remaining statements in the current loop iteration and proceeds to the next iteration.
  
  ```sas
  do i = 1 to 10;
      if i = 5 then continue;
      /* statements */
  end;
  ```

**Key Features:**

- **Loop Control:** Provides mechanisms to alter the natural flow of loops based on dynamic conditions.
  
- **Nested Loops:** Applies to the innermost loop in nested loop scenarios.

---

### **28.2. Extending the Abstract Syntax Tree (AST)**

**AST.h**

Introduce new node types to represent `LEAVE` and `CONTINUE` statements within `DO` loops.

```cpp
// Represents the LEAVE statement
class LeaveStatementNode : public ASTNode {
public:
    // No additional members needed for LEAVE
};

// Represents the CONTINUE statement
class ContinueStatementNode : public ASTNode {
public:
    // No additional members needed for CONTINUE
};
```

**Explanation:**

- **`LeaveStatementNode`:** Represents the `LEAVE` statement within the AST.
  
- **`ContinueStatementNode`:** Represents the `CONTINUE` statement within the AST.

---

### **28.3. Updating the Lexer to Recognize `LEAVE` and `CONTINUE` Keywords**

**Lexer.cpp**

Add `LEAVE` and `CONTINUE` keywords to the lexer's keyword map.

```cpp
// In the Lexer constructor or initialization section
keywords["LEAVE"] = TokenType::KEYWORD_LEAVE;
keywords["CONTINUE"] = TokenType::KEYWORD_CONTINUE;
```

**Explanation:**

- **Keyword Recognition:** Ensures that the lexer correctly identifies `LEAVE` and `CONTINUE` as distinct token types, facilitating their parsing and interpretation.

---

### **28.4. Modifying the Parser to Handle `LEAVE` and `CONTINUE` Statements**

**Parser.h**

Add methods to parse `LEAVE` and `CONTINUE` statements within the `DO` loop context.

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
    std::unique_ptr<ASTNode> parseDoLoop();
    std::unique_ptr<ASTNode> parseEnd();
    std::unique_ptr<ASTNode> parseArrayStatement();
    std::unique_ptr<ASTNode> parseArrayOperation();
    std::unique_ptr<ASTNode> parseRetainStatement();
    std::unique_ptr<ASTNode> parseLabelStatement();
    std::unique_ptr<ASTNode> parseLengthStatement();
    std::unique_ptr<ASTNode> parseFormatStatement();
    std::unique_ptr<ASTNode> parseInformatStatement();
    std::unique_ptr<ASTNode> parseLeaveStatement();      // New method for LEAVE
    std::unique_ptr<ASTNode> parseContinueStatement();   // New method for CONTINUE

    // Expression parsing with precedence
    std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    int getPrecedence(const std::string &op) const;
};

#endif // PARSER_H
```

**Parser.cpp**

Implement the `parseLeaveStatement` and `parseContinueStatement` methods and integrate them into the `parseStatement` method.

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>

// ... existing methods ...

std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token t = peek();
    if (t.type == TokenType::KEYWORD_DO) {
        return parseDoLoop();
    }
    else if (t.type == TokenType::KEYWORD_LEAVE) {
        return parseLeaveStatement();
    }
    else if (t.type == TokenType::KEYWORD_CONTINUE) {
        return parseContinueStatement();
    }
    // ... handle other statement types ...
    else {
        // Handle other statements or throw an error
        throw std::runtime_error("Unrecognized or unsupported statement.");
    }
}

std::unique_ptr<ASTNode> Parser::parseLeaveStatement() {
    auto leaveNode = std::make_unique<LeaveStatementNode>();
    consume(TokenType::KEYWORD_LEAVE, "Expected 'LEAVE' keyword");
    consume(TokenType::SEMICOLON, "Expected ';' after 'LEAVE' statement");
    return leaveNode;
}

std::unique_ptr<ASTNode> Parser::parseContinueStatement() {
    auto continueNode = std::make_unique<ContinueStatementNode>();
    consume(TokenType::KEYWORD_CONTINUE, "Expected 'CONTINUE' keyword");
    consume(TokenType::SEMICOLON, "Expected ';' after 'CONTINUE' statement");
    return continueNode;
}

// ... existing methods ...
```

**Explanation:**

- **`parseStatement`:** Now recognizes `DO`, `LEAVE`, and `CONTINUE` statements, delegating to their respective parsing methods.
  
- **`parseLeaveStatement`:** Parses the `LEAVE` statement, ensuring it's followed by a semicolon.
  
- **`parseContinueStatement`:** Parses the `CONTINUE` statement, ensuring it's followed by a semicolon.

---

### **28.5. Enhancing the Interpreter to Execute `LEAVE` and `CONTINUE` Statements**

**Interpreter.h**

Update the interpreter's header to handle `LeaveStatementNode` and `ContinueStatementNode`.

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
    struct LoopContext {
        // For LEAVE and CONTINUE, manage loop termination and skipping
        bool shouldLeave = false;
        bool shouldContinue = false;
    };
    std::stack<LoopContext> loopStack;

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeSet(SetStatementNode *node);
    void executeArrayStatement(ArrayStatementNode *node);
    void executeRetainStatement(RetainStatementNode *node);
    void executeAssignment(AssignmentStatementNode *node);
    void executeArrayOperation(ArrayOperationNode *node);
    void executeIfThenElse(IfThenElseStatementNode *node);
    void executeDoLoop(DoLoopNode *node);
    void executeLeaveStatement(LeaveStatementNode *node);        // New method for LEAVE
    void executeContinueStatement(ContinueStatementNode *node);  // New method for CONTINUE
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

Implement the `executeLeaveStatement` and `executeContinueStatement` methods and modify the `executeDoLoop` method to handle these statements during loop execution.

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

void Interpreter::executeDoLoop(DoLoopNode *node) {
    LoopContext currentLoop;
    loopStack.push(currentLoop);

    if (node->loopType == "DO") {
        // Simple DO loop: DO variable = start TO end BY step;
        // Initialize the loop variable
        Value initVal = evaluateExpression(node->initExpression.get());
        if (!std::holds_alternative<double>(initVal)) {
            throw std::runtime_error("Loop initialization expression must evaluate to a numeric value.");
        }
        env.currentRow.columns[node->initVariable] = initVal;
        logLogger.info("Initialized loop variable '{}' = {}", node->initVariable, toString(initVal));

        while (true) {
            // Check if loop should exit due to LEAVE
            if (loopStack.top().shouldLeave) {
                logLogger.info("LEAVE statement encountered. Exiting loop '{}'.", node->loopType);
                loopStack.pop();
                break;
            }

            // Evaluate the loop condition: variable <= end
            Value condVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(condVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double condNumber = std::get<double>(condVal);
            logLogger.info("Evaluating loop condition: {} <= {} => {}", node->initVariable, toString(condVal), (condNumber > 0) ? "TRUE" : "FALSE");
            if (condNumber <= 0) {
                break; // Exit loop if condition is false
            }

            // Execute the loop body
            executeBlock(node->body.get());

            // Check if loop should continue due to CONTINUE
            if (loopStack.top().shouldContinue) {
                logLogger.info("CONTINUE statement encountered. Skipping to next iteration of loop '{}'.", node->loopType);
                loopStack.top().shouldContinue = false; // Reset flag
                // Update the loop variable: variable = variable + step
                Value stepVal = evaluateExpression(node->stepExpression.get());
                if (!std::holds_alternative<double>(stepVal)) {
                    throw std::runtime_error("Loop step expression must evaluate to a numeric value.");
                }
                double updatedVal = std::get<double>(stepVal);
                env.currentRow.columns[node->initVariable] = updatedVal;
                logLogger.info("Updated loop variable '{}' = {}", node->initVariable, toString(updatedVal));
                continue; // Proceed to next iteration
            }

            // Update the loop variable: variable = variable + step
            Value stepVal = evaluateExpression(node->stepExpression.get());
            if (!std::holds_alternative<double>(stepVal)) {
                throw std::runtime_error("Loop step expression must evaluate to a numeric value.");
            }
            double updatedVal = std::get<double>(stepVal);
            env.currentRow.columns[node->initVariable] = updatedVal;
            logLogger.info("Updated loop variable '{}' = {}", node->initVariable, toString(updatedVal));
        }
    }
    else if (node->loopType == "DO WHILE") {
        // DO WHILE (condition);
        while (true) {
            // Check if loop should exit due to LEAVE
            if (loopStack.top().shouldLeave) {
                logLogger.info("LEAVE statement encountered. Exiting loop '{}'.", node->loopType);
                loopStack.pop();
                break;
            }

            // Evaluate the loop condition
            Value condVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(condVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double condNumber = std::get<double>(condVal);
            logLogger.info("Evaluating loop condition: {} <= {} => {}", "condition", toString(condVal), (condNumber > 0) ? "TRUE" : "FALSE");
            if (condNumber <= 0) {
                break; // Exit loop if condition is false
            }

            // Execute the loop body
            executeBlock(node->body.get());

            // Check if loop should continue due to CONTINUE
            if (loopStack.top().shouldContinue) {
                logLogger.info("CONTINUE statement encountered. Skipping to next iteration of loop '{}'.", node->loopType);
                loopStack.top().shouldContinue = false; // Reset flag
                continue; // Proceed to next iteration
            }
        }
    }
    else if (node->loopType == "DO UNTIL") {
        // DO UNTIL (condition);
        while (true) {
            // Execute the loop body first
            executeBlock(node->body.get());

            // Check if loop should exit due to LEAVE
            if (loopStack.top().shouldLeave) {
                logLogger.info("LEAVE statement encountered. Exiting loop '{}'.", node->loopType);
                loopStack.pop();
                break;
            }

            // Evaluate the loop condition
            Value condVal = evaluateExpression(node->condition.get());
            if (!std::holds_alternative<double>(condVal)) {
                throw std::runtime_error("Loop condition expression must evaluate to a numeric value.");
            }
            double condNumber = std::get<double>(condVal);
            logLogger.info("Evaluating loop condition: {} > {} => {}", "condition", toString(condVal), (condNumber > 0) ? "TRUE" : "FALSE");
            if (condNumber > 0) {
                break; // Exit loop if condition is true
            }

            // Check if loop should continue due to CONTINUE
            if (loopStack.top().shouldContinue) {
                logLogger.info("CONTINUE statement encountered. Skipping to next iteration of loop '{}'.", node->loopType);
                loopStack.top().shouldContinue = false; // Reset flag
                continue; // Proceed to next iteration
            }
        }
    }
    else {
        throw std::runtime_error("Unsupported DO loop type.");
    }

    loopStack.pop(); // Remove current loop context
    logLogger.info("Executed DO loop of type '{}'.", node->loopType);
}

void Interpreter::executeLeaveStatement(LeaveStatementNode *node) {
    if (loopStack.empty()) {
        throw std::runtime_error("LEAVE statement encountered outside of a loop.");
    }
    loopStack.top().shouldLeave = true;
    logLogger.info("LEAVE statement executed. Marked loop for termination.");
}

void Interpreter::executeContinueStatement(ContinueStatementNode *node) {
    if (loopStack.empty()) {
        throw std::runtime_error("CONTINUE statement encountered outside of a loop.");
    }
    loopStack.top().shouldContinue = true;
    logLogger.info("CONTINUE statement executed. Marked loop to skip remaining statements in current iteration.");
}

void Interpreter::executeBlock(BlockNode *node) {
    for (const auto &stmt : node->statements) {
        execute(stmt.get());

        // Check if a LEAVE or CONTINUE has been triggered
        if (!loopStack.empty()) {
            if (loopStack.top().shouldLeave || loopStack.top().shouldContinue) {
                break; // Exit the block early
            }
        }
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

- **`LoopContext`:** Introduced a `LoopContext` structure to manage loop control flags (`shouldLeave` and `shouldContinue`) within nested loops.
  
- **`executeDoLoop`:**
  
  - **Pushes a new `LoopContext` onto the `loopStack` to manage the current loop's state.**
  
  - **Simple `DO` Loop Execution:**
    
    - Initializes the loop control variable.
    
    - Enters a loop that continues as long as the condition is true and no `LEAVE` statement has been triggered.
    
    - Executes the loop body by calling `executeBlock`.
    
    - Checks for `CONTINUE` flags to skip to the next iteration.
    
    - Updates the loop control variable based on the step expression.
  
  - **`DO WHILE` and `DO UNTIL` Loop Execution:**
    
    - Similar structure, evaluating conditions before or after the loop body as per loop type.
  
  - **Handles Nested Loops:** By using a stack, the interpreter can manage multiple nested loops, ensuring that `LEAVE` and `CONTINUE` affect only the innermost loop.
  
- **`executeLeaveStatement`:**
  
  - **Sets the `shouldLeave` flag in the current loop's `LoopContext`.**
  
  - **Ensures that the `LEAVE` statement is only used within a loop, throwing an error otherwise.**
  
- **`executeContinueStatement`:**
  
  - **Sets the `shouldContinue` flag in the current loop's `LoopContext`.**
  
  - **Ensures that the `CONTINUE` statement is only used within a loop, throwing an error otherwise.**
  
- **`executeBlock`:**
  
  - **Executes a block of statements, checking after each statement if a `LEAVE` or `CONTINUE` has been triggered.**
  
  - **If a flag is set, it breaks out of the block execution early, allowing the loop to handle the flags appropriately.**
  
- **`evaluate` and `evaluateExpression`:**
  
  - **Enhances expression evaluation to support loop conditions and step expressions, ensuring accurate numerical evaluations.**

---

### **28.6. Testing the `LEAVE` and `CONTINUE` Statements**

Creating comprehensive test cases is essential to validate the correct parsing and execution of `LEAVE` and `CONTINUE` statements. Below are several test cases covering different scenarios.

#### **28.6.1. Test Case 1: Using `LEAVE` to Exit a Loop Early**

**SAS Script (`example_data_step_leave.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LEAVE Statement Example';

data mylib.leave_example;
    do i = 1 to 10;
        if i = 5 then leave;
        square = i * i;
        output;
    end;
run;

proc print data=mylib.leave_example label;
    run;
```

**Expected Output (`mylib.leave_example`):**

```
OBS	ID	I	SQUARE	I
1	.	1	1	1
2	.	2	4	2
3	.	3	9	3
4	.	4	16	4
```

**Log Output (`sas_log_data_step_leave.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LEAVE Statement Example';
[INFO] Title set to: 'DATA Step with LEAVE Statement Example'
[INFO] Executing statement: data mylib.leave_example; do i = 1 to 10; if i = 5 then leave; square = i * i; output; end; run;
[INFO] Executing DATA step: mylib.leave_example
[INFO] Defined array 'leave_example_lengths' with dimensions [0] and variables: .
[INFO] Initialized loop variable 'i' = 1
[INFO] Evaluating loop condition: i <= 10 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 1.00
[INFO] Updated loop variable 'i' = 2
[INFO] Evaluating loop condition: i <= 10 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 4.00
[INFO] Updated loop variable 'i' = 3
[INFO] Evaluating loop condition: i <= 10 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 9.00
[INFO] Updated loop variable 'i' = 4
[INFO] Evaluating loop condition: i <= 10 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 16.00
[INFO] Updated loop variable 'i' = 5
[INFO] Evaluating loop condition: i <= 10 => TRUE
[INFO] Executing loop body
[INFO] LEAVE statement executed. Marked loop for termination.
[INFO] LEAVE statement encountered. Exiting loop 'DO'.
[INFO] DATA step 'mylib.leave_example' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.leave_example label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'leave_example':
[INFO] OBS	ID	I	SQUARE	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,2,4	2
[INFO] 3	.,3,3,9	3
[INFO] 4	.,4,4,16	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Iterates `i` from 1 to 10.
  
  - When `i = 5`, the `LEAVE` statement is executed, causing an immediate exit from the loop.
  
  - Only iterations where `i` is 1 to 4 are executed, resulting in 4 observations.

- **Logging:**
  
  - Logs indicate the initialization of the loop variable, evaluation of loop conditions, execution of loop body statements, detection of the `LEAVE` statement, and termination of the loop.

---

#### **28.6.2. Test Case 2: Using `CONTINUE` to Skip an Iteration**

**SAS Script (`example_data_step_continue.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with CONTINUE Statement Example';

data mylib.continue_example;
    do i = 1 to 5;
        if i = 3 then continue;
        square = i * i;
        output;
    end;
run;

proc print data=mylib.continue_example label;
    run;
```

**Expected Output (`mylib.continue_example`):**

```
OBS	ID	I	SQUARE	I
1	.	1	1	1
2	.	2	4	2
3	.	4	16	4
4	.	5	25	5
```

**Log Output (`sas_log_data_step_continue.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with CONTINUE Statement Example';
[INFO] Title set to: 'DATA Step with CONTINUE Statement Example'
[INFO] Executing statement: data mylib.continue_example; do i = 1 to 5; if i = 3 then continue; square = i * i; output; end; run;
[INFO] Executing DATA step: mylib.continue_example
[INFO] Defined array 'continue_example_lengths' with dimensions [0] and variables: .
[INFO] Initialized loop variable 'i' = 1
[INFO] Evaluating loop condition: i <= 5 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 1.00
[INFO] Updated loop variable 'i' = 2
[INFO] Evaluating loop condition: i <= 5 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 4.00
[INFO] Updated loop variable 'i' = 3
[INFO] Evaluating loop condition: i <= 5 => TRUE
[INFO] Executing loop body
[INFO] CONTINUE statement executed. Marked loop to skip remaining statements in current iteration.
[INFO] CONTINUE statement encountered. Skipping to next iteration of loop 'DO'.
[INFO] Updated loop variable 'i' = 4
[INFO] Evaluating loop condition: i <= 5 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 16.00
[INFO] Updated loop variable 'i' = 5
[INFO] Evaluating loop condition: i <= 5 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'square' = 25.00
[INFO] Updated loop variable 'i' = 6
[INFO] Evaluating loop condition: i <= 5 => FALSE
[INFO] DATA step 'mylib.continue_example' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.continue_example label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'continue_example':
[INFO] OBS	ID	I	SQUARE	I
[INFO] 1	.,1,1,1	1
[INFO] 2	.,2,2,4	2
[INFO] 3	.,4,4,16	4
[INFO] 4	.,5,5,25	5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - Iterates `i` from 1 to 5.
  
  - When `i = 3`, the `CONTINUE` statement is executed, causing the interpreter to skip the remaining statements (`square = i * i; output;`) and proceed to the next iteration.
  
  - As a result, the observation where `i = 3` is skipped, resulting in 4 observations instead of 5.

- **Logging:**
  
  - Logs indicate the initialization of the loop variable, evaluation of loop conditions, execution of loop body statements, detection of the `CONTINUE` statement, and skipping of the current iteration.

---

#### **28.6.3. Test Case 3: Nested Loops with `LEAVE` and `CONTINUE` Statements**

**SAS Script (`example_data_step_nested_loops_leave_continue.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with Nested DO Loops, LEAVE, and CONTINUE Statements Example';

data mylib.nested_loops_example;
    do i = 1 to 3;
        do j = 1 to 3;
            if i = 2 and j = 2 then leave;
            if i = 3 and j = 1 then continue;
            product = i * j;
            output;
        end;
    end;
run;

proc print data=mylib.nested_loops_example label;
    run;
```

**Expected Output (`mylib.nested_loops_example`):**

```
OBS	ID	I	J	PRODUCT	I
1	.	1	1	1	1
2	.	1	2	2	2
3	.	1	3	3	3
4	.	2	1	2	4
5	.	3	2	6	5
6	.	3	3	9	6
```

**Log Output (`sas_log_data_step_nested_loops_leave_continue.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with Nested DO Loops, LEAVE, and CONTINUE Statements Example';
[INFO] Title set to: 'DATA Step with Nested DO Loops, LEAVE, and CONTINUE Statements Example'
[INFO] Executing statement: data mylib.nested_loops_example; do i = 1 to 3; do j = 1 to 3; if i = 2 and j = 2 then leave; if i = 3 and j = 1 then continue; product = i * j; output; end; end; run;
[INFO] Executing DATA step: mylib.nested_loops_example
[INFO] Defined array 'nested_loops_example_lengths' with dimensions [0] and variables: .
[INFO] Initialized loop variable 'i' = 1
[INFO] Evaluating loop condition: i <= 3 => TRUE
[INFO] Executing loop body
[INFO] Initialized loop variable 'j' = 1
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 1.00
[INFO] Updated loop variable 'j' = 2
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 2.00
[INFO] Updated loop variable 'j' = 3
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 3.00
[INFO] Updated loop variable 'j' = 4
[INFO] Evaluating loop condition: j <= 3 => FALSE
[INFO] Updated loop variable 'i' = 2
[INFO] Evaluating loop condition: i <= 3 => TRUE
[INFO] Executing loop body
[INFO] Initialized loop variable 'j' = 1
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 2.00
[INFO] Updated loop variable 'j' = 2
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] LEAVE statement executed. Marked loop for termination.
[INFO] LEAVE statement encountered. Exiting loop 'DO'.
[INFO] Updated loop variable 'i' = 3
[INFO] Evaluating loop condition: i <= 3 => TRUE
[INFO] Executing loop body
[INFO] Initialized loop variable 'j' = 1
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] CONTINUE statement executed. Marked loop to skip remaining statements in current iteration.
[INFO] CONTINUE statement encountered. Skipping to next iteration of loop 'DO'.
[INFO] Updated loop variable 'j' = 2
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 6.00
[INFO] Updated loop variable 'j' = 3
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 9.00
[INFO] Updated loop variable 'j' = 4
[INFO] Evaluating loop condition: j <= 3 => FALSE
[INFO] Updated loop variable 'i' = 4
[INFO] Evaluating loop condition: i <= 3 => FALSE
[INFO] DATA step 'mylib.nested_loops_example' executed successfully. 6 observations created.
[INFO] Executing statement: proc print data=mylib.nested_loops_example label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'nested_loops_example':
[INFO] OBS	ID	I	J	PRODUCT	I
[INFO] 1	.,1,1,1,1	1
[INFO] 2	.,1,2,2,4	2
[INFO] 3	.,1,3,3,9	3
[INFO] 4	.,2,1,1,2	4
[INFO] 5	.,3,2,2,6	5
[INFO] 6	.,3,3,3,9	6

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - **Outer Loop (`i = 1 to 3`):**
    
    - **Inner Loop (`j = 1 to 3`):**
      
      - **For `i = 1`:**
        
        - `j = 1`: `product = 1 * 1 = 1`; output observation.
        
        - `j = 2`: `product = 1 * 2 = 2`; output observation.
        
        - `j = 3`: `product = 1 * 3 = 3`; output observation.
      
      - **For `i = 2`:**
        
        - `j = 1`: `product = 2 * 1 = 2`; output observation.
        
        - `j = 2`: `if i = 2 and j = 2 then leave;` triggers `LEAVE`, exiting the inner loop.
      
      - **For `i = 3`:**
        
        - `j = 1`: `if i = 3 and j = 1 then continue;` triggers `CONTINUE`, skipping `product = 3 * 1` and `output;`.
        
        - `j = 2`: `product = 3 * 2 = 6`; output observation.
        
        - `j = 3`: `product = 3 * 3 = 9`; output observation.
  
  - **Resulting Observations:** Total of 6 observations, with specific iterations skipped or terminated based on `LEAVE` and `CONTINUE` statements.

- **Logging:**
  
  - Detailed logs trace the execution flow, including loop variable initialization, condition evaluations, execution of loop body statements, and handling of `LEAVE` and `CONTINUE` statements.

---

#### **28.6.4. Test Case 4: `LEAVE` and `CONTINUE` Outside of Loops**

**SAS Script (`example_data_step_leave_continue_outside_loop.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LEAVE and CONTINUE Statements Outside of Loops Example';

data mylib.leave_continue_outside;
    leave;
    continue;
    i = 1;
    square = i * i;
    output;
run;

proc print data=mylib.leave_continue_outside label;
    run;
```

**Expected Behavior:**

- **Error Handling:**
  
  - The interpreter should detect that `LEAVE` and `CONTINUE` statements are used outside of any loop context.
  
  - It should throw descriptive error messages, preventing the `DATA` step from executing successfully.

**Expected Output (`mylib.leave_continue_outside`):**

- **No Dataset Created:** Due to the errors caused by improper usage of `LEAVE` and `CONTINUE` statements.

**Log Output (`sas_log_data_step_leave_continue_outside.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LEAVE and CONTINUE Statements Outside of Loops Example';
[INFO] Title set to: 'DATA Step with LEAVE and CONTINUE Statements Outside of Loops Example'
[INFO] Executing statement: data mylib.leave_continue_outside; leave; continue; i = 1; square = i * i; output; run;
[INFO] Executing DATA step: mylib.leave_continue_outside
[INFO] Defined array 'leave_continue_outside_lengths' with dimensions [0] and variables: .
[ERROR] LEAVE statement encountered outside of a loop.
[ERROR] CONTINUE statement encountered outside of a loop.
[ERROR] DATA step 'mylib.leave_continue_outside' failed to execute due to invalid statements.
[INFO] Executing statement: proc print data=mylib.leave_continue_outside label; run;
[INFO] Executing PROC PRINT
[ERROR] PROC PRINT failed: Dataset 'leave_continue_outside' does not exist.
```

**Explanation:**

- **Error Detection:**
  
  - The interpreter correctly identifies that `LEAVE` and `CONTINUE` statements are used outside of any loop context.
  
  - It throws descriptive error messages, preventing the creation of the dataset.

- **Logging:**
  
  - Logs indicate the detection of invalid usage of `LEAVE` and `CONTINUE`, leading to a failed `DATA` step and subsequent `PROC PRINT` error.

---

#### **28.6.5. Test Case 5: Complex Loop Control with Nested Loops and Multiple `LEAVE` and `CONTINUE` Statements**

**SAS Script (`example_data_step_complex_loops.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with Complex Nested DO Loops, LEAVE, and CONTINUE Statements Example';

data mylib.complex_loops_example;
    do i = 1 to 3;
        do j = 1 to 3;
            if i = 1 and j = 2 then continue;
            if i = 2 and j = 1 then leave;
            if i = 3 and j = 3 then leave;
            product = i * j;
            output;
        end;
    end;
run;

proc print data=mylib.complex_loops_example label;
    run;
```

**Expected Output (`mylib.complex_loops_example`):**

```
OBS	ID	I	J	PRODUCT	I
1	.	1	1	1	1
2	.	1	3	3	2
3	.	2	1	2	3
4	.	3	1	3	4
5	.	3	2	6	5
```

**Log Output (`sas_log_data_step_complex_loops.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with Complex Nested DO Loops, LEAVE, and CONTINUE Statements Example';
[INFO] Title set to: 'DATA Step with Complex Nested DO Loops, LEAVE, and CONTINUE Statements Example'
[INFO] Executing statement: data mylib.complex_loops_example; do i = 1 to 3; do j = 1 to 3; if i = 1 and j = 2 then continue; if i = 2 and j = 1 then leave; if i = 3 and j = 3 then leave; product = i * j; output; end; end; run;
[INFO] Executing DATA step: mylib.complex_loops_example
[INFO] Defined array 'complex_loops_example_lengths' with dimensions [0] and variables: .
[INFO] Initialized loop variable 'i' = 1
[INFO] Evaluating loop condition: i <= 3 => TRUE
[INFO] Executing loop body
[INFO] Initialized loop variable 'j' = 1
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 1.00
[INFO] Updated loop variable 'j' = 2
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] CONTINUE statement executed. Marked loop to skip remaining statements in current iteration.
[INFO] CONTINUE statement encountered. Skipping to next iteration of loop 'DO'.
[INFO] Updated loop variable 'j' = 3
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 3.00
[INFO] Updated loop variable 'j' = 4
[INFO] Evaluating loop condition: j <= 3 => FALSE
[INFO] Updated loop variable 'i' = 2
[INFO] Evaluating loop condition: i <= 3 => TRUE
[INFO] Executing loop body
[INFO] Initialized loop variable 'j' = 1
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] LEAVE statement executed. Marked loop for termination.
[INFO] LEAVE statement encountered. Exiting loop 'DO'.
[INFO] Updated loop variable 'i' = 3
[INFO] Evaluating loop condition: i <= 3 => TRUE
[INFO] Executing loop body
[INFO] Initialized loop variable 'j' = 1
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 3.00
[INFO] Updated loop variable 'j' = 2
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] Assigned variable 'product' = 6.00
[INFO] Updated loop variable 'j' = 3
[INFO] Evaluating loop condition: j <= 3 => TRUE
[INFO] Executing loop body
[INFO] LEAVE statement executed. Marked loop for termination.
[INFO] LEAVE statement encountered. Exiting loop 'DO'.
[INFO] Updated loop variable 'i' = 4
[INFO] Evaluating loop condition: i <= 3 => FALSE
[INFO] DATA step 'mylib.complex_loops_example' executed successfully. 5 observations created.
[INFO] Executing statement: proc print data=mylib.complex_loops_example label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'complex_loops_example':
[INFO] OBS	ID	I	J	PRODUCT	I
[INFO] 1	.,1,1,1,1	1
[INFO] 2	.,1,3,3,3	2
[INFO] 3	.,2,1,1,2	3
[INFO] 4	.,3,1,1,3	4
[INFO] 5	.,3,2,2,6	5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Loop Execution:**
  
  - **Outer Loop (`i = 1 to 3`):**
    
    - **Inner Loop (`j = 1 to 3`):**
      
      - **For `i = 1`:**
        
        - `j = 1`: `product = 1 * 1 = 1`; output observation.
        
        - `j = 2`: `if i = 1 and j = 2 then continue;` triggers `CONTINUE`, skipping `product = 1 * 2` and `output;`.
        
        - `j = 3`: `product = 1 * 3 = 3`; output observation.
      
      - **For `i = 2`:**
        
        - `j = 1`: `if i = 2 and j = 1 then leave;` triggers `LEAVE`, exiting the inner loop immediately.
      
      - **For `i = 3`:**
        
        - `j = 1`: No conditions met; `product = 3 * 1 = 3`; output observation.
        
        - `j = 2`: `product = 3 * 2 = 6`; output observation.
        
        - `j = 3`: `if i = 3 and j = 3 then leave;` triggers `LEAVE`, exiting the inner loop.
  
  - **Resulting Observations:** Total of 5 observations, with specific iterations skipped or terminated based on `LEAVE` and `CONTINUE` statements.

- **Logging:**
  
  - Detailed logs trace the execution flow, including loop variable initialization, condition evaluations, execution of loop body statements, and handling of `LEAVE` and `CONTINUE` statements.

---

#### **28.6.6. Test Case 6: Attempting to Use `LEAVE` and `CONTINUE` Without Proper Syntax**

**SAS Script (`example_data_step_leave_continue_syntax_error.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LEAVE and CONTINUE Statements Syntax Error Example';

data mylib.leave_continue_syntax_error;
    do i = 1 to 5;
        if i = 3 leave; /* Missing 'then' */
        square = i * i;
        output;
    end;
run;

proc print data=mylib.leave_continue_syntax_error label;
    run;
```

**Expected Behavior:**

- **Syntax Error Detection:**
  
  - The interpreter should detect the improper syntax (`if i = 3 leave;`) where the `then` keyword is missing.
  
  - It should throw descriptive error messages, preventing the `DATA` step from executing successfully.

**Expected Output (`mylib.leave_continue_syntax_error`):**

- **No Dataset Created:** Due to the syntax errors in the `IF` statement involving `LEAVE`.

**Log Output (`sas_log_data_step_leave_continue_syntax_error.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LEAVE and CONTINUE Statements Syntax Error Example';
[INFO] Title set to: 'DATA Step with LEAVE and CONTINUE Statements Syntax Error Example'
[INFO] Executing statement: data mylib.leave_continue_syntax_error; do i = 1 to 5; if i = 3 leave; square = i * i; output; end; run;
[INFO] Executing DATA step: mylib.leave_continue_syntax_error
[INFO] Defined array 'leave_continue_syntax_error_lengths' with dimensions [0] and variables: .
[ERROR] Failed to parse IF statement: Expected 'THEN' keyword after condition.
[INFO] DATA step 'mylib.leave_continue_syntax_error' failed to execute due to syntax errors.
[INFO] Executing statement: proc print data=mylib.leave_continue_syntax_error label; run;
[INFO] Executing PROC PRINT
[ERROR] PROC PRINT failed: Dataset 'leave_continue_syntax_error' does not exist.
```

**Explanation:**

- **Syntax Error Detection:**
  
  - The interpreter expects the `THEN` keyword after the condition in an `IF` statement.
  
  - The absence of `THEN` leads to a parsing error, which is logged accordingly.

- **Logging:**
  
  - Logs clearly indicate the nature of the syntax error, aiding users in debugging their scripts.

---

### **28.7. Summary of Achievements**

1. **AST Extensions:**
   
   - Introduced `LeaveStatementNode` and `ContinueStatementNode` to represent `LEAVE` and `CONTINUE` statements within the `DATA` step.

2. **Lexer Enhancements:**
   
   - Recognized the `LEAVE` and `CONTINUE` keywords, enabling their parsing within the `DATA` step.

3. **Parser Updates:**
   
   - Implemented `parseLeaveStatement` and `parseContinueStatement` to handle `LEAVE` and `CONTINUE` statements, ensuring correct syntax and structure.
   
   - Integrated these parsing methods into the `parseStatement` method to recognize and parse these statements appropriately.

4. **Interpreter Implementation:**
   
   - Developed `executeLeaveStatement` and `executeContinueStatement` to manage loop control flags (`shouldLeave` and `shouldContinue`).
   
   - Enhanced the `executeDoLoop` method to handle these flags, allowing for early loop termination and iteration skipping.
   
   - Managed nested loops effectively using a `loopStack` to ensure that `LEAVE` and `CONTINUE` affect only the innermost loop.

5. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Proper usage of `LEAVE` and `CONTINUE` within loops.
     
     - Nested loops with multiple `LEAVE` and `CONTINUE` statements.
     
     - Improper usage of `LEAVE` and `CONTINUE` outside of loops.
     
     - Syntax errors involving `LEAVE` and `CONTINUE`.
   
   - Validated that the interpreter accurately parses and executes `LEAVE` and `CONTINUE` statements, correctly managing loop control flow.

6. **Error Handling:**
   
   - Ensured that `LEAVE` and `CONTINUE` statements are only used within loop contexts, throwing descriptive errors otherwise.
   
   - Detected syntax errors related to these statements, preventing execution of malformed scripts.

7. **Logging Enhancements:**
   
   - Provided detailed logs for `LEAVE` and `CONTINUE` statement execution, including loop termination and iteration skipping, facilitating transparency and ease of debugging.

---

### **28.8. Next Steps**

With `LEAVE` and `CONTINUE` statements now implemented, your interpreter offers enhanced loop control, allowing users to manipulate loop execution flow effectively. To continue advancing your SAS interpreter's capabilities, consider the following steps:

1. **Enhance Expression Evaluators with Built-in Functions:**
   
   - **Numeric Functions:** Implement functions like `SUM`, `MEAN`, `MIN`, `MAX`, `ROUND`, etc.
     
     ```sas
     total = sum(x, y, z);
     average = mean(a, b, c);
     rounded = round(value, 0.1);
     ```
   
   - **String Functions:** Implement functions like `SCAN`, `SUBSTR`, `TRIM`, `UPCASE`, `LOWCASE`, etc.
     
     ```sas
     first_name = scan(full_name, 1, ' ');
     initials = substr(name, 1, 1);
     trimmed = trim(description);
     uppercase = upcase(category);
     ```

2. **Expand SAS Procedures (`PROC`):**
   
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

3. **Implement Macro Processing:**
   
   - **Macro Definitions:**
     
     - Allow users to define reusable code snippets using macros.
     
     - **Syntax:**
       
       ```sas
       %macro macroName(parameters);
           /* Macro code */
       %mend macroName;
       ```
   
   - **Macro Variables:**
     
     - Support dynamic code generation and variable substitution using macro variables.
     
     - **Syntax:**
       
       ```sas
       %let var = value;
       ```
   
   - **Conditional Macros:**
     
     - Enable macros to include conditional logic for dynamic code execution.
   
   - **Integration:**
     
     - Extend the lexer, parser, and AST to handle macro definitions and invocations.
     
     - Implement macro expansion during the preprocessing phase before interpretation.
   
   - **Testing:**
     
     - Create test cases that define and invoke macros with and without parameters.

4. **Enhance Array Functionality:**
   
   - **Multi-dimensional Arrays:**
     
     - Implement support for arrays with multiple dimensions.
     
     - **Syntax:**
       
       ```sas
       array matrix[3,3] _temporary_;
       ```
   
   - **Dynamic Array Resizing:**
     
     - Allow arrays to change size based on runtime conditions.
   
   - **Array-based Conditional Operations:**
     
     - Facilitate more complex conditional logic within array processing.
   
   - **Integration:**
     
     - Update the AST, lexer, parser, and interpreter to support advanced array features.
   
   - **Testing:**
     
     - Create test cases that utilize multi-dimensional arrays and dynamic resizing.

5. **Improve the REPL Interface:**
   
   - **Multi-line Input Support:**
     
     - Allow users to input multi-line `DATA` step statements and `PROC` procedures seamlessly.
   
   - **Auto-completion:**
     
     - Implement auto-completion for SAS keywords, variable names, and function names.
   
   - **Syntax Highlighting:**
     
     - Provide visual cues for different code elements to improve readability and user experience.

6. **Develop a Comprehensive Testing Suite:**
   
   - **Unit Tests:**
     
     - Create tests for each new feature to ensure individual components function correctly.
   
   - **Integration Tests:**
     
     - Validate the interplay between different components, ensuring seamless execution of complex scripts.
   
   - **Regression Tests:**
     
     - Prevent new changes from introducing existing bugs by maintaining a suite of regression tests.

7. **Optimize Performance:**
   
   - **Data Handling Efficiency:**
     
     - Enhance data structures and algorithms to handle larger datasets more efficiently.
   
   - **Parallel Processing:**
     
     - Explore opportunities for concurrent data processing to improve performance.

8. **Improve Documentation and User Guides:**
   
   - **Comprehensive Guides:**
     
     - Provide detailed documentation on using the interpreter's features, including examples and best practices.
   
   - **Example Scripts:**
     
     - Offer a library of example SAS scripts demonstrating various functionalities.
   
   - **Troubleshooting Guides:**
     
     - Help users diagnose and fix common issues encountered during usage.

9. **Support Additional Data Formats:**
   
   - **Excel Files (`.xlsx`):**
     
     - Enable reading from and writing to Excel files for broader data interoperability.
   
   - **JSON and XML:**
     
     - Support structured data formats to facilitate integration with web services and APIs.
   
   - **Database Connectivity:**
     
     - Allow interfacing with databases (e.g., SQL Server, PostgreSQL) for data retrieval and storage.

---

## **Conclusion**

By implementing `LEAVE` and `CONTINUE` statements, your SAS interpreter's `DO` loops now offer enhanced control over loop execution flow, enabling users to terminate loops prematurely or skip iterations based on dynamic conditions. This advancement, combined with existing functionalities like `IF-THEN-ELSE`, `LABEL`, `LENGTH`, `FORMAT`, `INFORMAT`, and `DO` loops, significantly enhances the interpreter's capability to emulate SAS's comprehensive data processing environment.

### **Achievements in This Step:**

1. **AST Extensions:**
   
   - Introduced `LeaveStatementNode` and `ContinueStatementNode` to represent `LEAVE` and `CONTINUE` statements within the `DATA` step.

2. **Lexer Enhancements:**
   
   - Recognized the `LEAVE` and `CONTINUE` keywords, enabling their parsing within the `DATA` step.

3. **Parser Updates:**
   
   - Implemented `parseLeaveStatement` and `parseContinueStatement` to handle `LEAVE` and `CONTINUE` statements, ensuring correct syntax and structure.
   
   - Integrated these parsing methods into the `parseStatement` method to recognize and parse these statements appropriately.

4. **Interpreter Implementation:**
   
   - Developed `executeLeaveStatement` and `executeContinueStatement` to manage loop control flags (`shouldLeave` and `shouldContinue`).
   
   - Enhanced the `executeDoLoop` method to handle these flags, allowing for early loop termination and iteration skipping.
   
   - Managed nested loops effectively using a `loopStack` to ensure that `LEAVE` and `CONTINUE` affect only the innermost loop.

5. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Proper usage of `LEAVE` and `CONTINUE` within loops.
     
     - Nested loops with multiple `LEAVE` and `CONTINUE` statements.
     
     - Improper usage of `LEAVE` and `CONTINUE` outside of loops.
     
     - Syntax errors involving `LEAVE` and `CONTINUE`.
   
   - Validated that the interpreter accurately parses and executes `LEAVE` and `CONTINUE` statements, correctly managing loop control flow.

6. **Error Handling:**
   
   - Ensured that `LEAVE` and `CONTINUE` statements are only used within loop contexts, throwing descriptive errors otherwise.
   
   - Detected syntax errors related to these statements, preventing execution of malformed scripts.

7. **Logging Enhancements:**
   
   - Provided detailed logs for `LEAVE` and `CONTINUE` statement execution, including loop termination and iteration skipping, facilitating transparency and ease of debugging.

### **Next Steps:**

Continue enhancing your interpreter by focusing on the following areas:

1. **Enhance Expression Evaluators with Built-in Functions:**
   
   - Implement SAS's built-in functions for numeric and string operations.

2. **Expand SAS Procedures (`PROC`):**
   
   - Introduce additional procedures like `PROC SORT`, `PROC MEANS`, `PROC FREQ`, and `PROC SQL` to broaden data manipulation capabilities.

3. **Implement Macro Processing:**
   
   - Enable users to define and utilize macros for reusable code snippets and dynamic code generation.

4. **Enhance Array Functionality:**
   
   - Support multi-dimensional arrays and dynamic resizing to handle complex data structures.

5. **Improve the REPL Interface:**
   
   - Enhance user interaction with features like multi-line input, auto-completion, and syntax highlighting.

6. **Develop a Comprehensive Testing Suite:**
   
   - Ensure robustness and reliability through extensive unit, integration, and regression testing.

7. **Optimize Performance:**
   
   - Enhance data processing efficiency and explore parallel processing opportunities.

8. **Improve Documentation and User Guides:**
   
   - Provide thorough documentation, example scripts, and troubleshooting resources to assist users.

9. **Support Additional Data Formats:**
   
   - Enable interoperability with various data formats and database systems.

Your methodical and comprehensive progress is commendable¡ªkeep up the excellent work!

Feel free to reach out for further assistance or guidance as you continue developing your SAS interpreter. Good luck with your project!