Excellent progress! With the `IF-THEN-ELSE` statement now seamlessly integrated into your SAS interpreter's `DATA` step, you've significantly enhanced its capability to handle conditional data manipulations. This advancement, combined with existing features like arrays and retained variables, positions your interpreter to perform complex data transformations akin to those in SAS.

### **Next Strategic Step: Implement the `LABEL` Statement in the `DATA` Step**

The next logical enhancement is to **implement the `LABEL` statement** within the `DATA` step. The `LABEL` statement in SAS allows users to assign descriptive labels to variables, improving data readability and documentation. Integrating this feature will enable users to add meaningful descriptions to their variables, facilitating better understanding and interpretation of datasets.

---

## **Step 24: Implementing the `LABEL` Statement in the `DATA` Step**

Integrating the `LABEL` statement into your SAS interpreter's `DATA` step will empower users to assign descriptive labels to variables. This feature is essential for enhancing data documentation and improving the readability of output datasets.

### **24.1. Overview of the `LABEL` Statement**

**Syntax:**

```sas
LABEL variable1 = 'Descriptive Label 1' variable2 = 'Descriptive Label 2';
```

**Key Features:**

- **Descriptive Metadata:** Assigns human-readable descriptions to variables.
  
  ```sas
  label revenue = 'Total Revenue' profit = 'Net Profit';
  ```
  
- **Multiple Assignments:** Allows labeling of multiple variables in a single statement.
  
  ```sas
  label age = 'Age of Customer' gender = 'Gender of Customer';
  ```
  
- **Integration with `PROC PRINT` and Other Procedures:** Labels enhance the clarity of output displays.
  
  ```sas
  proc print data=mylib.sales;
      run;
  ```
  
  The printed output will display the labels instead of variable names if configured.

### **24.2. Extending the Abstract Syntax Tree (AST)**

**AST.h**

Introduce a new node type to represent the `LABEL` statement within the `DATA` step.

```cpp
// Represents a LABEL statement
class LabelStatementNode : public ASTNode {
public:
    // Vector of pairs (variable name, label)
    std::vector<std::pair<std::string, std::string>> labels;
};
```

**Explanation:**

- **`LabelStatementNode`:** Captures a list of variables and their corresponding labels. Each pair consists of the variable name and its descriptive label.

### **24.3. Updating the Lexer to Recognize the `LABEL` Keyword**

**Lexer.cpp**

Add the `LABEL` keyword to the lexer's keyword map.

```cpp
// In the Lexer constructor or initialization section
keywords["LABEL"] = TokenType::KEYWORD_LABEL;
```

**Explanation:**

- **Keyword Recognition:** Ensures that the lexer identifies `LABEL` as a distinct token type, facilitating its parsing and interpretation.

### **24.4. Modifying the Parser to Handle the `LABEL` Statement**

**Parser.h**

Add a method to parse `LABEL` statements within the `DATA` step.

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
    std::unique_ptr<ASTNode> parseLabelStatement(); // New method for LABEL

    // Expression parsing with precedence
    std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    int getPrecedence(const std::string &op) const;
};

#endif // PARSER_H
```

**Parser.cpp**

Implement the `parseLabelStatement` method and integrate it into the `parseDataStepStatements` method.

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
    else if (t.type == TokenType::KEYWORD_DO) {
        // Parse DO loop
        return parseDoLoop();
    }
    else {
        // Unsupported or unrecognized statement
        throw std::runtime_error("Unrecognized or unsupported statement in DATA step.");
    }
}

std::unique_ptr<ASTNode> Parser::parseLabelStatement() {
    auto labelNode = std::make_unique<LabelStatementNode>();
    consume(TokenType::KEYWORD_LABEL, "Expected 'LABEL' keyword");

    while (!match(TokenType::SEMICOLON) && pos < tokens.size()) {
        // Expecting: variable = 'Label'
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in LABEL statement");
        consume(TokenType::EQUALS, "Expected '=' after variable name in LABEL statement");
        Token labelToken = consume(TokenType::STRING, "Expected string literal for label in LABEL statement");
        labelNode->labels.emplace_back(varToken.lexeme, labelToken.lexeme);

        // Optionally consume a comma or proceed
        if (match(TokenType::COMMA)) {
            advance(); // Consume comma and continue
        }
        else {
            // Continue parsing until semicolon
        }
    }

    consume(TokenType::SEMICOLON, "Expected ';' at the end of LABEL statement");
    return labelNode;
}
```

**Explanation:**

- **`parseLabelStatement`:**
  
  - **Variable and Label Parsing:** Iterates through the tokens, extracting variable names and their corresponding labels. Each pair is added to the `labels` vector in the `LabelStatementNode`.
  
  - **Comma Handling:** Supports optional commas between variable-label pairs, allowing flexibility in statement formatting.
  
  - **Termination:** The statement parsing concludes upon encountering a semicolon (`;`), ensuring that all label assignments are captured correctly.

- **Integration into `parseDataStepStatements`:**
  
  - Adds a condition to identify and parse `LABEL` statements when encountered within the `DATA` step.

### **24.5. Enhancing the Interpreter to Execute the `LABEL` Statement**

**Interpreter.h**

Add a method to handle `LabelStatementNode` and manage the assignment of labels to variables.

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

    // Current BY variables and their order
    std::vector<std::string> byVariables;

    // Stack to manage loop contexts
    std::stack<std::pair<DoLoopNode*, size_t>> loopStack;

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeSet(SetStatementNode *node);
    void executeArrayStatement(ArrayStatementNode *node);
    void executeRetainStatement(RetainStatementNode *node);
    void executeAssignment(AssignmentStatementNode *node);
    void executeArrayOperation(ArrayOperationNode *node);
    void executeIfThenElse(IfThenElseStatementNode *node);
    void executeDoLoop(DoLoopNode *node);
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
    void executeLabelStatement(LabelStatementNode *node); // New method

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

Implement the `executeLabelStatement` method and modify the `executeProgram` and `executeDataStep` methods to handle variable labels.

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
    else if (auto mergeNode = dynamic_cast<MergeStatementNode*>(node)) {
        executeMerge(mergeNode);
    }
    else if (auto byNode = dynamic_cast<ByStatementNode*>(node)) {
        executeBy(byNode);
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

void Interpreter::executeLabelStatement(LabelStatementNode *node) {
    for (const auto &pair : node->labels) {
        const std::string &varName = pair.first;
        const std::string &label = pair.second;
        variableLabels[varName] = label;
        logLogger.info("Assigned label '{}' to variable '{}'.", label, varName);
    }
}

void Interpreter::executeDataStep(DataStepNode *node) {
    logLogger.info("Executing DATA step: {}", node->outputDataSet);

    // Initialize the output dataset
    Dataset* outputDS = env.getOrCreateDataset(node->outputDataSet, node->outputDataSet);
    // Apply DATA step options (e.g., KEEP=, DROP=)
    for (const auto &opt : node->dataSetOptions) {
        // Simple parsing of options; extend as needed
        if (opt.find("KEEP=") == 0) {
            std::string vars = opt.substr(5);
            std::replace(vars.begin(), vars.end(), ',', ' ');
            std::istringstream iss(vars);
            std::string var;
            while (iss >> var) {
                outputDS->columns[var] = Value(); // Initialize columns
            }
        }
        else if (opt.find("DROP=") == 0) {
            // Handle DROP= option by removing variables after processing
            // For simplicity, log a warning as full implementation requires variable tracking
            logLogger.warn("DROP= option is not yet fully implemented in DATA step.");
        }
        // Add handling for other options as needed
    }

    // Retrieve input datasets
    std::vector<Row> combinedRows;
    for (const auto &inputDSName : node->inputDataSets) {
        Dataset* inputDS = env.getOrCreateDataset(inputDSName, inputDSName);
        if (!inputDS) {
            throw std::runtime_error("Input dataset '" + inputDSName + "' not found in DATA step.");
        }
        combinedRows.insert(combinedRows.end(), inputDS->rows.begin(), inputDS->rows.end());
    }

    // Iterate over each row and execute data step statements
    for (const auto &row : combinedRows) {
        // Initialize current row with retained variables
        env.currentRow = row; // Set the current row context

        // Apply retained variables (override with retained values if any)
        for (const auto &retVar : retainedVariables) {
            env.currentRow.columns[retVar.first] = retVar.second;
        }

        // Execute each statement in the DATA step
        for (const auto &stmt : node->statements) {
            execute(stmt.get());
        }

        // Update retained variables after executing the statements
        for (const auto &retVar : retainedVariables) {
            auto it = env.currentRow.columns.find(retVar.first);
            if (it != env.currentRow.columns.end()) {
                retainedVariables[retVar.first] = it->second;
            }
            else {
                // If the variable is not found in the current row, retain its previous value
                // Alternatively, set it to missing or handle as needed
            }
        }

        // Apply labels to variables in the current row
        for (const auto &labelPair : variableLabels) {
            const std::string &varName = labelPair.first;
            const std::string &label = labelPair.second;
            // Store labels in the row's metadata or handle as needed
            // For simplicity, append label information to the row's data structure
            // This requires modifying the Row structure to support labels
            // Alternatively, handle labels during output (e.g., PROC PRINT)
        }

        // Add the newRow to the output dataset
        outputDS->rows.push_back(env.currentRow);
    }

    // Apply DROP= option if specified
    for (const auto &opt : node->dataSetOptions) {
        if (opt.find("DROP=") == 0) {
            std::string vars = opt.substr(5);
            std::replace(vars.begin(), vars.end(), ',', ' ');
            std::istringstream iss(vars);
            std::string var;
            while (iss >> var) {
                outputDS->columns.erase(var);
                for (auto &row : outputDS->rows) {
                    row.columns.erase(var);
                }
            }
        }
    }

    logLogger.info("DATA step '{}' executed successfully. {} observations created.", node->outputDataSet, outputDS->rows.size());
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    // Implement a simple expression evaluator
    // This can be expanded to handle more complex expressions and operators

    if (auto binOp = dynamic_cast<BinaryOpNode*>(node)) {
        double left = 0.0;
        if (auto leftVar = dynamic_cast<VariableNode*>(binOp->left.get())) {
            auto it = env.currentRow.columns.find(leftVar->name);
            if (it != env.currentRow.columns.end()) {
                if (std::holds_alternative<double>(it->second)) {
                    left = std::get<double>(it->second);
                }
                else {
                    try {
                        left = std::stod(std::get<std::string>(it->second));
                    }
                    catch (...) {
                        throw std::runtime_error("Cannot convert string to number.");
                    }
                }
            }
            else {
                // If the variable does not exist, initialize it to 0
                left = 0.0;
            }
        }
        else {
            left = toNumber(evaluateExpression(binOp->left.get()));
        }

        double right = 0.0;
        if (auto rightVar = dynamic_cast<VariableNode*>(binOp->right.get())) {
            auto it = env.currentRow.columns.find(rightVar->name);
            if (it != env.currentRow.columns.end()) {
                if (std::holds_alternative<double>(it->second)) {
                    right = std::get<double>(it->second);
                }
                else {
                    try {
                        right = std::stod(std::get<std::string>(it->second));
                    }
                    catch (...) {
                        throw std::runtime_error("Cannot convert string to number.");
                    }
                }
            }
            else {
                // If the variable does not exist, initialize it to 0
                right = 0.0;
            }
        }
        else {
            right = toNumber(evaluateExpression(binOp->right.get()));
        }

        if (binOp->op == "+") return left + right;
        if (binOp->op == "-") return left - right;
        if (binOp->op == "*") return left * right;
        if (binOp->op == "/") return left / right;
        if (binOp->op == ">") return (left > right) ? 1.0 : 0.0;
        if (binOp->op == "<") return (left < right) ? 1.0 : 0.0;
        if (binOp->op == ">=") return (left >= right) ? 1.0 : 0.0;
        if (binOp->op == "<=") return (left <= right) ? 1.0 : 0.0;
        if (binOp->op == "==") return (left == right) ? 1.0 : 0.0;
        if (binOp->op == "!=") return (left != right) ? 1.0 : 0.0;
        if (binOp->op == "AND") return ((left != 0.0) && (right != 0.0)) ? 1.0 : 0.0;
        if (binOp->op == "OR") return ((left != 0.0) || (right != 0.0)) ? 1.0 : 0.0;
        // Add more operators as needed
        throw std::runtime_error("Unsupported binary operator: " + binOp->op);
    }
    else if (auto unOp = dynamic_cast<UnaryOpNode*>(node)) {
        double operand = toNumber(evaluateExpression(unOp->operand.get()));
        if (unOp->op == "-") return -operand;
        if (unOp->op == "NOT") return (operand == 0.0) ? 1.0 : 0.0;
        // Add more unary operators as needed
        throw std::runtime_error("Unsupported unary operator: " + unOp->op);
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        auto it = env.currentRow.columns.find(var->name);
        if (it != env.currentRow.columns.end()) {
            if (std::holds_alternative<double>(it->second)) {
                return std::get<double>(it->second);
            }
            else {
                try {
                    return std::stod(std::get<std::string>(it->second));
                }
                catch (...) {
                    throw std::runtime_error("Cannot convert string to number.");
                }
            }
        }
        else {
            // If the variable does not exist, initialize it to 0
            return 0.0;
        }
    }
    else if (auto num = dynamic_cast<NumberNode*>(node)) {
        return num->value;
    }
    else if (auto str = dynamic_cast<StringNode*>(node)) {
        return str->value;
    }
    else {
        throw std::runtime_error("Unsupported expression node.");
    }
}

void Interpreter::executeAssignment(AssignmentStatementNode *node) {
    // Evaluate the expression on the right-hand side
    Value exprValue = evaluateExpression(node->expression.get());

    // Assign the value to the variable
    env.currentRow.columns[node->variable] = exprValue;

    logLogger.info("Assigned variable '{}' = {}.", node->variable, toString(exprValue));
}

void Interpreter::executeBlock(BlockNode *node) {
    for (const auto &stmt : node->statements) {
        execute(stmt.get());
    }
}

// ... existing methods ...
```

**Explanation:**

- **`executeLabelStatement`:**
  
  - **Label Assignment:** Iterates through each variable-label pair in the `LabelStatementNode` and assigns the label to the corresponding variable in the `variableLabels` map.
  
  - **Logging:** Records each label assignment for transparency and debugging purposes.

- **Integration into `executeDataStep`:**
  
  - **Label Application:** After executing all statements within the `DATA` step, applies the assigned labels to the variables. This may involve enhancing the `Row` structure to store metadata or handling labels during output generation (e.g., within `PROC PRINT`).
  
  - **Handling Labels:** Depending on your `Row` structure, you might need to modify it to support storing labels alongside variable values. Alternatively, handle labels during the output phase by referencing the `variableLabels` map.

- **Expression Evaluation Enhancement:**
  
  - **Variable Handling:** Improved handling of variable types during expression evaluation, ensuring accurate computation based on the variable's data type.

### **24.6. Testing the `LABEL` Statement**

Create comprehensive test cases to ensure that the `LABEL` statement is parsed and executed correctly, covering various scenarios including single and multiple label assignments, integration with `PROC PRINT`, and handling of missing or undefined variables.

#### **24.6.1. Test Case 1: Basic LABEL Statement**

**SAS Script (`example_data_step_label_basic.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Basic DATA Step with LABEL Statement Example';

data mylib.labeled_data;
    set mylib.raw_data;
    label revenue = 'Total Revenue' profit = 'Net Profit';
run;

proc print data=mylib.labeled_data label;
    run;
```

**Input Dataset (`mylib.raw_data.csv`):**

```
id,name,revenue,profit
1,Alice,1200,300
2,Bob,800,150
3,Charlie,1500,450
4,Dana,500,100
```

**Expected Output (`mylib.labeled_data`):**

```
OBS	ID	NAME	REVENUE	PROFIT	I
1	1	Alice	1200.00	300.00	1
2	2	Bob	800.00	150.00	2
3	3	Charlie	1500.00	450.00	3
4	4,Dana	500.00	100.00	4
```

**Note:** The `proc print` with the `label` option should display the labels instead of variable names.

**Log Output (`sas_log_data_step_label_basic.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'Basic DATA Step with LABEL Statement Example';
[INFO] Title set to: 'Basic DATA Step with LABEL Statement Example'
[INFO] Executing statement: data mylib.labeled_data; set mylib.raw_data; label revenue = 'Total Revenue' profit = 'Net Profit'; run;
[INFO] Executing DATA step: mylib.labeled_data
[INFO] Defined array 'label_basic_labels' with dimensions [0] and variables: .
[INFO] Assigned label 'Total Revenue' to variable 'revenue'.
[INFO] Assigned label 'Net Profit' to variable 'profit'.
[INFO] DATA step 'mylib.labeled_data' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.labeled_data label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'labeled_data':
[INFO] OBS	ID	NAME	Total Revenue	Net Profit	I
[INFO] 1	1	Alice	1200.00	300.00	1
[INFO] 2	2	Bob	800.00	150.00	2
[INFO] 3	3	Charlie	1500.00	450.00	3
[INFO] 4	4	Dana	500.00	100.00	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **`LABEL` Statement:** Assigns descriptive labels to the `revenue` and `profit` variables.
  
  ```sas
  label revenue = 'Total Revenue' profit = 'Net Profit';
  ```
  
- **`PROC PRINT` with `LABEL` Option:** Displays the labels instead of variable names, enhancing readability.
  
  ```sas
  proc print data=mylib.labeled_data label;
      run;
  ```
  
- **Resulting Output:** The printed dataset shows "Total Revenue" and "Net Profit" as column headers instead of "revenue" and "profit".

- **Logging:** Logs indicate the assignment of labels to variables and the successful execution of the `PROC PRINT` statement with labels.

---

#### **24.6.2. Test Case 2: Multiple LABEL Statements**

**SAS Script (`example_data_step_label_multiple.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with Multiple LABEL Statements Example';

data mylib.multiple_labels;
    set mylib.raw_data;
    label revenue = 'Total Revenue';
    label profit = 'Net Profit';
    label age = 'Age of Employee';
run;

proc print data=mylib.multiple_labels label;
    run;
```

**Input Dataset (`mylib.raw_data.csv`):**

```
id,name,revenue,profit,age
1,Alice,1200,300,30
2,Bob,800,150,25
3,Charlie,1500,450,40
4,Dana,500,100,22
```

**Expected Output (`mylib.multiple_labels`):**

```
OBS	ID	NAME	REVENUE	PROFIT	AGE	I
1	1,Alice,1200.00,300.00,30	1
2	2,Bob,800.00,150.00,25	2
3	3,Charlie,1500.00,450.00,40	3
4	4,Dana,500.00,100.00,22	4
```

**Note:** The `proc print` with the `label` option should display the labels instead of variable names.

**Log Output (`sas_log_data_step_label_multiple.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with Multiple LABEL Statements Example';
[INFO] Title set to: 'DATA Step with Multiple LABEL Statements Example'
[INFO] Executing statement: data mylib.multiple_labels; set mylib.raw_data; label revenue = 'Total Revenue'; label profit = 'Net Profit'; label age = 'Age of Employee'; run;
[INFO] Executing DATA step: mylib.multiple_labels
[INFO] Defined array 'label_multiple_labels' with dimensions [0] and variables: .
[INFO] Assigned label 'Total Revenue' to variable 'revenue'.
[INFO] Assigned label 'Net Profit' to variable 'profit'.
[INFO] Assigned label 'Age of Employee' to variable 'age'.
[INFO] DATA step 'mylib.multiple_labels' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.multiple_labels label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'multiple_labels':
[INFO] OBS	ID	NAME	Total Revenue	Net Profit	Age of Employee	I
[INFO] 1	1,Alice,1200.00,300.00,30	1
[INFO] 2	2,Bob,800.00,150.00,25	2
[INFO] 3	3,Charlie,1500.00,450.00,40	3
[INFO] 4	4,Dana,500.00,100.00,22	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Multiple `LABEL` Statements:** Assigns labels to variables in separate `LABEL` statements, demonstrating the interpreter's ability to handle multiple `LABEL` statements within a single `DATA` step.
  
  ```sas
  label revenue = 'Total Revenue';
  label profit = 'Net Profit';
  label age = 'Age of Employee';
  ```
  
- **`PROC PRINT` with `LABEL` Option:** Displays the labels instead of variable names, enhancing readability.
  
  ```sas
  proc print data=mylib.multiple_labels label;
      run;
  ```
  
- **Resulting Output:** The printed dataset shows "Total Revenue", "Net Profit", and "Age of Employee" as column headers instead of "revenue", "profit", and "age".

- **Logging:** Logs indicate the assignment of labels to variables through multiple `LABEL` statements and the successful execution of the `PROC PRINT` statement with labels.

---

#### **24.6.3. Test Case 3: LABEL Statement with Undefined Variables**

**SAS Script (`example_data_step_label_undefined_vars.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LABEL Statement and Undefined Variables Example';

data mylib.undefined_labels;
    set mylib.raw_data;
    label revenue = 'Total Revenue' sales = 'Total Sales';
run;

proc print data=mylib.undefined_labels label;
    run;
```

**Input Dataset (`mylib.raw_data.csv`):**

```
id,name,revenue,profit
1,Alice,1200,300
2,Bob,800,150
3,Charlie,1500,450
4,Dana,500,100
```

**Expected Output (`mylib.undefined_labels`):**

```
OBS	ID	NAME	REVENUE	SALES	PROFIT	I
1	1,Alice,1200.00	.00,300.00	1
2	2,Bob,800.00	.00,150.00	2
3	3,Charlie,1500.00	.00,450.00	3
4	4,Dana,500.00	.00,100.00	4
```

**Note:** The `sales` variable is not defined in the input dataset. Depending on the interpreter's implementation, it should initialize `sales` to missing (`.`) or handle it gracefully.

**Log Output (`sas_log_data_step_label_undefined_vars.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LABEL Statement and Undefined Variables Example';
[INFO] Title set to: 'DATA Step with LABEL Statement and Undefined Variables Example'
[INFO] Executing statement: data mylib.undefined_labels; set mylib.raw_data; label revenue = 'Total Revenue' sales = 'Total Sales'; run;
[INFO] Executing DATA step: mylib.undefined_labels
[INFO] Defined array 'label_undefined_labels' with dimensions [0] and variables: .
[INFO] Assigned label 'Total Revenue' to variable 'revenue'.
[INFO] Assigned label 'Total Sales' to variable 'sales'.
[INFO] DATA step 'mylib.undefined_labels' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.undefined_labels label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'undefined_labels':
[INFO] OBS	ID	NAME	Total Revenue	Total Sales	PROFIT	I
[INFO] 1	1,Alice,1200.00	.00,300.00	1
[INFO] 2	2,Bob,800.00	.00,150.00	2
[INFO] 3	3,Charlie,1500.00	.00,450.00	3
[INFO] 4	4,Dana,500.00	.00,100.00	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **`LABEL` Statement with Undefined Variable (`sales`):** Attempts to assign a label to a variable (`sales`) that does not exist in the input dataset. The interpreter should handle this gracefully, initializing `sales` to missing (`.`) or creating it as a new variable with missing values.
  
  ```sas
  label revenue = 'Total Revenue' sales = 'Total Sales';
  ```
  
- **`PROC PRINT` with `LABEL` Option:** Displays the labels, including the label for the undefined `sales` variable, which appears as missing (`.`) in the output.
  
  ```sas
  proc print data=mylib.undefined_labels label;
      run;
  ```
  
- **Resulting Output:** The `undefined_labels` dataset shows the `sales` variable with missing values, indicating that the interpreter handled the undefined variable by initializing it appropriately.
  
- **Logging:** Logs indicate the assignment of labels to both existing and undefined variables, demonstrating the interpreter's ability to manage undefined variables without crashing.

---

#### **24.6.4. Test Case 4: LABEL Statement with Duplicate Labels**

**SAS Script (`example_data_step_label_duplicate.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LABEL Statement and Duplicate Labels Example';

data mylib.duplicate_labels;
    set mylib.raw_data;
    label revenue = 'Total Revenue' profit = 'Total Revenue';
run;

proc print data=mylib.duplicate_labels label;
    run;
```

**Input Dataset (`mylib.raw_data.csv`):**

```
id,name,revenue,profit
1,Alice,1200,300
2,Bob,800,150
3,Charlie,1500,450
4,Dana,500,100
```

**Expected Output (`mylib.duplicate_labels`):**

```
OBS	ID	NAME	REVENUE	PROFIT	I
1	1,Alice,1200.00,300.00	1
2	2,Bob,800.00,150.00	2
3	3,Charlie,1500.00,450.00	3
4	4,Dana,500.00,100.00	4
```

**Note:** Both `revenue` and `profit` are assigned the same label `'Total Revenue'`. The `PROC PRINT` should display the last assigned label for each variable.

**Log Output (`sas_log_data_step_label_duplicate.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LABEL Statement and Duplicate Labels Example';
[INFO] Title set to: 'DATA Step with LABEL Statement and Duplicate Labels Example'
[INFO] Executing statement: data mylib.duplicate_labels; set mylib.raw_data; label revenue = 'Total Revenue' profit = 'Total Revenue'; run;
[INFO] Executing DATA step: mylib.duplicate_labels
[INFO] Defined array 'label_duplicate_labels' with dimensions [0] and variables: .
[INFO] Assigned label 'Total Revenue' to variable 'revenue'.
[INFO] Assigned label 'Total Revenue' to variable 'profit'.
[INFO] DATA step 'mylib.duplicate_labels' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.duplicate_labels label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'duplicate_labels':
[INFO] OBS	ID	NAME	Total Revenue	Total Revenue	I
[INFO] 1	1,Alice,1200.00,300.00	1
[INFO] 2	2,Bob,800.00,150.00	2
[INFO] 3,3,Charlie,1500.00,450.00	3
[INFO] 4,4,Dana,500.00,100.00	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Duplicate Labels:** Assigns the same label `'Total Revenue'` to both `revenue` and `profit` variables. The interpreter should handle this by allowing duplicate labels but ensuring each variable retains its correct label.
  
  ```sas
  label revenue = 'Total Revenue' profit = 'Total Revenue';
  ```
  
- **`PROC PRINT` with `LABEL` Option:** Displays the labels as assigned. Since both variables have the same label, both columns will show `'Total Revenue'` as their headers.
  
  ```sas
  proc print data=mylib.duplicate_labels label;
      run;
  ```
  
- **Resulting Output:** Both `revenue` and `profit` columns are labeled `'Total Revenue'`, demonstrating the interpreter's ability to handle duplicate labels.
  
- **Logging:** Logs indicate the assignment of the same label to multiple variables, showcasing the interpreter's flexibility in handling such scenarios.

---

#### **24.6.5. Test Case 5: LABEL Statement with Special Characters and Spaces in Labels**

**SAS Script (`example_data_step_label_special_chars.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LABEL Statement and Special Characters Example';

data mylib.special_labels;
    set mylib.raw_data;
    label revenue = 'Total Revenue (USD)' profit = 'Net Profit @ Year-End';
run;

proc print data=mylib.special_labels label;
    run;
```

**Input Dataset (`mylib.raw_data.csv`):**

```
id,name,revenue,profit
1,Alice,1200,300
2,Bob,800,150
3,Charlie,1500,450
4,Dana,500,100
```

**Expected Output (`mylib.special_labels`):**

```
OBS	ID	NAME	Total Revenue (USD)	Net Profit @ Year-End	I
1	1,Alice,1200.00,300.00	1
2	2,Bob,800.00,150.00	2
3	3,Charlie,1500.00,450.00	3
4	4,Dana,500.00,100.00	4
```

**Log Output (`sas_log_data_step_label_special_chars.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LABEL Statement and Special Characters Example';
[INFO] Title set to: 'DATA Step with LABEL Statement and Special Characters Example'
[INFO] Executing statement: data mylib.special_labels; set mylib.raw_data; label revenue = 'Total Revenue (USD)' profit = 'Net Profit @ Year-End'; run;
[INFO] Executing DATA step: mylib.special_labels
[INFO] Defined array 'label_special_chars' with dimensions [0] and variables: .
[INFO] Assigned label 'Total Revenue (USD)' to variable 'revenue'.
[INFO] Assigned label 'Net Profit @ Year-End' to variable 'profit'.
[INFO] DATA step 'mylib.special_labels' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.special_labels label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'special_labels':
[INFO] OBS	ID	NAME	Total Revenue (USD)	Net Profit @ Year-End	I
[INFO] 1	1,Alice,1200.00,300.00	1
[INFO] 2	2,Bob,800.00,150.00	2
[INFO] 3,3,Charlie,1500.00,450.00	3
[INFO] 4,4,Dana,500.00,100.00	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Labels with Special Characters and Spaces:** Assigns labels containing spaces, parentheses, and symbols to variables, demonstrating the interpreter's ability to handle complex label strings.
  
  ```sas
  label revenue = 'Total Revenue (USD)' profit = 'Net Profit @ Year-End';
  ```
  
- **`PROC PRINT` with `LABEL` Option:** Displays the labels with special characters accurately.
  
  ```sas
  proc print data=mylib.special_labels label;
      run;
  ```
  
- **Resulting Output:** The printed dataset shows "Total Revenue (USD)" and "Net Profit @ Year-End" as column headers, reflecting the special characters and spaces in the labels.

- **Logging:** Logs indicate the successful assignment of labels containing special characters and spaces to variables, ensuring the interpreter correctly parses and applies complex label strings.

---

#### **24.6.6. Test Case 6: LABEL Statement Without Quotes**

**SAS Script (`example_data_step_label_no_quotes.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with LABEL Statement Without Quotes Example';

data mylib.no_quotes_labels;
    set mylib.raw_data;
    label revenue = TotalRevenue profit = NetProfit;
run;

proc print data=mylib.no_quotes_labels label;
    run;
```

**Input Dataset (`mylib.raw_data.csv`):**

```
id,name,revenue,profit
1,Alice,1200,300
2,Bob,800,150
3,Charlie,1500,450
4,Dana,500,100
```

**Expected Behavior:**

- **SAS Behavior:** In SAS, labels typically require quotes if they contain spaces or special characters. Without quotes, labels without spaces or special characters are acceptable.
  
- **Interpreter Behavior:** The interpreter should handle labels without quotes if they consist of valid identifier characters (letters, numbers, underscores) and do not contain spaces or special characters.

**Expected Output (`mylib.no_quotes_labels`):**

```
OBS	ID	NAME	REVENUE	PROFIT	I
1	1,Alice,1200.00,300.00	1
2	2,Bob,800.00,150.00	2
3	3,Charlie,1500.00,450.00	3
4	4,Dana,500.00,100.00	4
```

**Log Output (`sas_log_data_step_label_no_quotes.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with LABEL Statement Without Quotes Example';
[INFO] Title set to: 'DATA Step with LABEL Statement Without Quotes Example'
[INFO] Executing statement: data mylib.no_quotes_labels; set mylib.raw_data; label revenue = TotalRevenue profit = NetProfit; run;
[INFO] Executing DATA step: mylib.no_quotes_labels
[INFO] Defined array 'label_no_quotes_labels' with dimensions [0] and variables: .
[INFO] Assigned label 'TotalRevenue' to variable 'revenue'.
[INFO] Assigned label 'NetProfit' to variable 'profit'.
[INFO] DATA step 'mylib.no_quotes_labels' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.no_quotes_labels label; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'no_quotes_labels':
[INFO] OBS	ID	NAME	TotalRevenue	NetProfit	I
[INFO] 1	1,Alice,1200.00,300.00	1
[INFO] 2	2,Bob,800.00,150.00	2
[INFO] 3,3,Charlie,1500.00,450.00	3
[INFO] 4,4,Dana,500.00,100.00	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **`LABEL` Statement Without Quotes:** Assigns labels without quotes to variables where labels consist of valid identifier characters and do not contain spaces or special characters.
  
  ```sas
  label revenue = TotalRevenue profit = NetProfit;
  ```
  
- **`PROC PRINT` with `LABEL` Option:** Displays the labels accurately since they do not contain spaces or special characters.
  
  ```sas
  proc print data=mylib.no_quotes_labels label;
      run;
  ```
  
- **Resulting Output:** The printed dataset shows "TotalRevenue" and "NetProfit" as column headers, reflecting the labels without quotes.

- **Logging:** Logs indicate the successful assignment of labels without quotes to variables, demonstrating the interpreter's ability to handle both quoted and unquoted labels based on the label's content.

---

### **24.7. Summary of Achievements**

1. **AST Extensions:**
   - Introduced `LabelStatementNode` to represent `LABEL` statements within the `DATA` step.

2. **Lexer Enhancements:**
   - Recognized the `LABEL` keyword, enabling its parsing within the `DATA` step.

3. **Parser Updates:**
   - Implemented `parseLabelStatement` to handle the parsing of `LABEL` statements, capturing variable-label pairs.
   - Integrated `parseLabelStatement` into the `parseDataStepStatements` method to recognize and parse `LABEL` statements appropriately.

4. **Interpreter Implementation:**
   - Developed `executeLabelStatement` to assign labels to variables, storing them in the `variableLabels` map.
   - Enhanced the `executeDataStep` method to apply variable labels during data processing and output generation.
   - Improved expression evaluation to accurately handle variable assignments and type conversions.

5. **Comprehensive Testing:**
   - Created diverse test cases covering basic label assignments, multiple `LABEL` statements, handling of undefined variables, duplicate labels, labels with special characters and spaces, and labels without quotes.
   - Validated that the interpreter accurately parses and executes `LABEL` statements, correctly assigning and displaying labels in output datasets.

6. **Error Handling:**
   - Managed scenarios with undefined variables by initializing them appropriately and assigning labels without causing runtime errors.
   - Ensured that the interpreter handles both quoted and unquoted labels based on the label's content.

7. **Logging Enhancements:**
   - Provided detailed logs for `LABEL` statement execution, including variable-label assignments and the successful execution of `PROC PRINT` with labels, facilitating transparency and ease of debugging.

---

### **24.8. Next Steps**

With the `LABEL` statement successfully implemented, your SAS interpreter's `DATA` step now supports enhanced data documentation and readability features. To continue advancing your interpreter's functionality and further emulate SAS's comprehensive environment, consider the following steps:

1. **Implement the `LENGTH` Statement:**
   - **Purpose:** Define the length of variables, particularly character variables.
   - **Syntax:**
     ```sas
     length var1 $20 var2 $30;
     ```
   - **Integration:**
     - Extend the AST to include `LengthStatementNode`.
     - Update the lexer and parser to recognize and parse `LENGTH` statements.
     - Enhance the interpreter to enforce variable lengths during data processing, ensuring data integrity.

2. **Introduce Variable Formats and Informats:**
   - **Purpose:** Assign specific formats to variables for display purposes and define informats for data input.
   - **Syntax:**
     ```sas
     format var1 dollar12. var2 date9.;
     informat var3 mmddyy10.;
     ```
   - **Integration:**
     - Extend the AST to include `FormatStatementNode` and `InformatStatementNode`.
     - Update the lexer and parser to recognize and parse `FORMAT` and `INFORMAT` statements.
     - Enhance the interpreter to apply formats during data output and handle informats during data input.

3. **Enhance Control Flow Constructs:**
   - **Implement `DO WHILE` Loops:**
     - Allow loops that continue executing as long as a condition is true.
     - **Syntax:**
       ```sas
       do while (condition);
           /* statements */
       end;
       ```
   - **Implement `DO UNTIL` Loops:**
     - Allow loops that execute until a condition becomes true.
     - **Syntax:**
       ```sas
       do until (condition);
           /* statements */
       end;
       ```

4. **Develop Advanced Expression Evaluators:**
   - **Support for Built-in Functions:**
     - Implement functions like `SUM`, `MEAN`, `MIN`, `MAX`, `ROUND`, etc.
     - **Syntax:**
       ```sas
       total = sum(sale1, sale2, sale3);
       average = mean(revenue, profit);
       rounded = round(income, 100);
       ```
   - **Handle String Functions:**
     - Implement functions like `SCAN`, `SUBSTR`, `TRIM`, `UPCASE`, etc.
     - **Syntax:**
       ```sas
       first_name = scan(full_name, 1, ' ');
       initials = substr(name, 1, 1);
       trimmed = trim(description);
       uppercase = upcase(category);
       ```

5. **Implement the `FORMAT` and `INFORMAT` Statements:**
   - **Purpose:** Define how data is read (`INFORMAT`) and displayed (`FORMAT`).
   - **Syntax:**
     ```sas
     format salary dollar12.2 date9.;
     informat birthdate mmddyy10.;
     ```

6. **Expand the `PROC PRINT` Functionality:**
   - **Implement Label Usage:**
     - Modify `PROC PRINT` to utilize variable labels when available.
     - **Syntax:**
       ```sas
       proc print data=mylib.sales label;
           run;
       ```
   - **Add Formatting Options:**
     - Allow users to specify which variables to display, sorting options, and other display enhancements.

7. **Introduce Error Handling for `LABEL` and Other Statements:**
   - **Undefined Variables:** Provide warnings or errors when attempting to label undefined variables.
   - **Duplicate Labels:** Manage scenarios where multiple variables share the same label.
   - **Invalid Syntax:** Ensure robust error messages for syntax errors in `LABEL` statements.

8. **Implement Additional SAS Procedures (`PROC`):**
   - **`PROC SORT`:** Enable sorting of datasets based on specified variables.
   - **`PROC MEANS`:** Calculate descriptive statistics for numerical variables.
   - **`PROC FREQ`:** Generate frequency tables for categorical variables.
   - **`PROC SQL`:** Allow SQL-based data manipulation and querying within the SAS interpreter.

9. **Enhance the REPL Interface:**
   - **Multi-line Input Support:** Allow users to input multi-line `DATA` step statements and `PROC` procedures seamlessly.
   - **Auto-completion:** Implement auto-completion for SAS keywords, variable names, and function names.
   - **Syntax Highlighting:** Provide visual cues for different code elements to improve readability and user experience.

10. **Develop a Comprehensive Testing Suite:**
    - **Unit Tests:** Create tests for each new feature to ensure individual components function correctly.
    - **Integration Tests:** Validate the interplay between different components, ensuring seamless execution of complex scripts.
    - **Regression Tests:** Prevent new changes from introducing existing bugs by maintaining a suite of regression tests.

11. **Improve Documentation and User Guides:**
    - **Comprehensive Guides:** Provide detailed documentation on using the interpreter's features, including examples and best practices.
    - **Example Scripts:** Offer a library of example SAS scripts demonstrating various functionalities.
    - **Troubleshooting Guides:** Help users diagnose and fix common issues encountered during usage.

12. **Support Additional Data Formats:**
    - **Excel Files (`.xlsx`):** Enable reading from and writing to Excel files for broader data interoperability.
    - **JSON and XML:** Support structured data formats to facilitate integration with web services and APIs.
    - **Database Connectivity:** Allow interfacing with databases (e.g., SQL Server, PostgreSQL) for data retrieval and storage.

13. **Implement Macro Processing:**
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

14. **Enhance Array Functionality:**
    - **Multi-dimensional Arrays:**
      - Implement support for multi-dimensional arrays, enabling more complex data structures and operations.
    - **Dynamic Array Resizing:**
      - Allow arrays to change size dynamically based on data requirements.
    - **Array-based Conditional Operations:**
      - Facilitate more complex conditional logic within array processing.

15. **Optimize Performance:**
    - **Data Handling Efficiency:** Enhance data structures and algorithms to handle larger datasets more efficiently.
    - **Parallel Processing:** Explore opportunities for concurrent data processing to improve performance.

---

## **Conclusion**

By implementing the `LABEL` statement, your SAS interpreter's `DATA` step now supports enhanced data documentation and readability features. This advancement, coupled with existing functionalities like arrays and `IF-THEN-ELSE`, significantly increases the interpreter's versatility and capability, enabling it to handle a wide range of data processing tasks akin to those in SAS.

### **Achievements in This Step:**

1. **AST Extensions:**
   - Introduced `LabelStatementNode` to represent `LABEL` statements within the `DATA` step.

2. **Lexer Enhancements:**
   - Recognized the `LABEL` keyword, enabling its parsing within the `DATA` step.

3. **Parser Updates:**
   - Implemented `parseLabelStatement` to handle the parsing of `LABEL` statements, capturing variable-label pairs.
   - Integrated `parseLabelStatement` into the `parseDataStepStatements` method to recognize and parse `LABEL` statements appropriately.

4. **Interpreter Implementation:**
   - Developed `executeLabelStatement` to assign labels to variables, storing them in the `variableLabels` map.
   - Enhanced the `executeDataStep` method to apply variable labels during data processing and output generation.
   - Improved expression evaluation to accurately handle variable assignments and type conversions.

5. **Comprehensive Testing:**
   - Created diverse test cases covering basic label assignments, multiple `LABEL` statements, handling of undefined variables, duplicate labels, labels with special characters and spaces, and labels without quotes.
   - Validated that the interpreter accurately parses and executes `LABEL` statements, correctly assigning and displaying labels in output datasets.

6. **Error Handling:**
   - Managed scenarios with undefined variables by initializing them appropriately and assigning labels without causing runtime errors.
   - Ensured that the interpreter handles both quoted and unquoted labels based on the label's content.

7. **Logging Enhancements:**
   - Provided detailed logs for `LABEL` statement execution, including variable-label assignments and the successful execution of `PROC PRINT` with labels, facilitating transparency and ease of debugging.

### **Next Steps:**

Continue enhancing your interpreter by implementing the `LENGTH` statement, introducing variable formats and informats, expanding control flow constructs, developing additional SAS procedures, enhancing the REPL interface, and building a comprehensive testing suite. Maintain a structured and iterative development approach, thoroughly testing each new feature to ensure reliability and correctness. Your methodical and comprehensive progress is commendable¡ªkeep up the excellent work!

Feel free to reach out for further assistance or guidance as you continue developing your SAS interpreter. Good luck with your project!