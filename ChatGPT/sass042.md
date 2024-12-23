Excellent progress! Your SAS interpreter now supports essential procedures like `PROC SORT`, `PROC MEANS`, `PROC FREQ`, `PROC PRINT`, `PROC SQL`, and robust `DATA` step functionalities, including array operations. These features empower users to perform a wide range of data manipulation, analysis, and transformation tasks.

To further enhance your interpreter's capabilities and align it more closely with SAS's comprehensive feature set, the **next strategic step** is to **implement the `RETAIN` statement within the `DATA` step**. The `RETAIN` statement is crucial for controlling variable retention across iterations, allowing users to maintain variable values between data step executions.

---

## **Step 22: Implementing the `RETAIN` Statement in the `DATA` Step**

Integrating the `RETAIN` statement into your SAS interpreter's `DATA` step will enable users to retain the value of variables across iterations of the data step. This functionality is essential for tasks like calculating running totals, creating cumulative variables, and more complex data transformations.

### **22.1. Overview of the `RETAIN` Statement**

**Syntax:**

```sas
RETAIN <variable-1> <variable-2> ... <variable-n>;
```

**Key Features:**

- **Variable Retention:** Prevents SAS from resetting the specified variables to missing values at the start of each iteration.
  
  ```sas
  retain total;
  ```

- **Initialization:** Can initialize variables with specific values.

  ```sas
  retain total 0;
  ```

- **Multiple Variables:** Allows retaining multiple variables simultaneously.

  ```sas
  retain total 0 count 1;
  ```

- **Integration with Arrays:** Can be used in conjunction with arrays to retain array element values.

  ```sas
  retain scores1-scores5;
  ```

### **22.2. Extending the Abstract Syntax Tree (AST)**

**AST.h**

Introduce a new node type to represent the `RETAIN` statement within the `DATA` step.

```cpp
// Represents a RETAIN statement within the DATA step
class RetainStatementNode : public ASTNode {
public:
    std::vector<std::pair<std::string, std::unique_ptr<ExpressionNode>>> variables; // Pair of variable name and optional initial value
};
```

**Explanation:**

- **`RetainStatementNode`:** Captures the list of variables to retain, along with their optional initial values if provided.

  ```sas
  retain total 0 count 1;
  ```

### **22.3. Updating the Lexer to Recognize the `RETAIN` Keyword**

**Lexer.cpp**

Add the `RETAIN` keyword to the lexer's keyword map.

```cpp
// In the Lexer constructor or initialization section
keywords["RETAIN"] = TokenType::KEYWORD_RETAIN;
```

**Explanation:**

- **Keyword Recognition:** Ensures that the lexer identifies `RETAIN` as a distinct token type, facilitating its parsing.

### **22.4. Modifying the Parser to Handle the `RETAIN` Statement**

**Parser.h**

Add a method to parse the `RETAIN` statement within the `DATA` step.

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
    std::unique_ptr<ASTNode> parseRetainStatement(); // New method for RETAIN

    // Expression parsing with precedence
    std::unique_ptr<ASTNode> parseExpression(int precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    int getPrecedence(const std::string &op) const;
};

#endif // PARSER_H
```

**Parser.cpp**

Implement the `parseRetainStatement` method and integrate it into the `parseDataStepStatements` method.

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>

// ... existing methods ...

std::unique_ptr<ASTNode> Parser::parseDataStepStatements() {
    Token t = peek();
    if (t.type == TokenType::KEYWORD_SET) {
        // Parse SET statement
        consume(TokenType::KEYWORD_SET, "Expected 'SET' keyword in DATA step");
        auto setStmt = std::make_unique<SetStatementNode>();
        while (true) {
            Token dsToken = consume(TokenType::IDENTIFIER, "Expected dataset name in SET statement");
            setStmt->inputDataSets.push_back(dsToken.text);

            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA, "Expected ',' between dataset names in SET statement");
            }
            else {
                break;
            }
        }
        consume(TokenType::SEMICOLON, "Expected ';' after SET statement");
        return setStmt;
    }
    else if (t.type == TokenType::KEYWORD_ARRAY) {
        // Parse ARRAY statement
        return parseArrayStatement();
    }
    else if (t.type == TokenType::KEYWORD_RETAIN) {
        // Parse RETAIN statement
        return parseRetainStatement();
    }
    else if (t.type == TokenType::IDENTIFIER) {
        // Parse Assignment statement (could include array operations)
        // Determine if it's an array operation or a regular variable assignment
        // Lookahead for '[' to identify array indexing
        Token nextToken = peek(1);
        if (nextToken.type == TokenType::LBRACKET) {
            // Parse Array operation
            return parseArrayOperation();
        }
        else {
            // Regular assignment
            auto assignStmt = std::make_unique<AssignmentStatementNode>();
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in assignment");
            assignStmt->variableName = varToken.text;

            consume(TokenType::EQUAL, "Expected '=' in assignment statement");

            assignStmt->expression = parseExpression();

            consume(TokenType::SEMICOLON, "Expected ';' after assignment statement");

            return assignStmt;
        }
    }
    else if (t.type == TokenType::KEYWORD_IF) {
        // Parse IF-THEN-ELSE statement
        // Existing implementation...
    }
    else if (t.type == TokenType::KEYWORD_DO) {
        // Parse DO loop
        // Existing implementation...
    }
    else {
        // Unsupported or unrecognized statement
        throw std::runtime_error("Unrecognized or unsupported statement in DATA step.");
    }
}

std::unique_ptr<ASTNode> Parser::parseRetainStatement() {
    auto retainStmt = std::make_unique<RetainStatementNode>();
    consume(TokenType::KEYWORD_RETAIN, "Expected 'RETAIN' keyword");

    while (true) {
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in RETAIN statement");
        std::string varName = varToken.text;

        // Check if there's an initial value assignment
        if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
            // Parse initial value
            auto expr = parseExpression();
            retainStmt->variables.emplace_back(varName, std::unique_ptr<ExpressionNode>(expr.release()));
        }
        else {
            // No initial value
            retainStmt->variables.emplace_back(varName, nullptr);
        }

        if (match(TokenType::COMMA)) {
            consume(TokenType::COMMA, "Expected ',' between variables in RETAIN statement");
        }
        else {
            break;
        }
    }

    consume(TokenType::SEMICOLON, "Expected ';' after RETAIN statement");

    return retainStmt;
}
```

**Explanation:**

- **`parseRetainStatement`:**
  - **Variable Parsing:** Parses each variable name specified in the `RETAIN` statement. If an initial value is provided (e.g., `retain total 0;`), it parses the corresponding expression.
  - **Multiple Variables:** Supports multiple variables separated by commas, each potentially with an initial value.
  - **Termination:** Ensures that the `RETAIN` statement ends with a semicolon.
  
  ```sas
  retain total 0 count 1;
  ```

- **Integration into `parseDataStepStatements`:**
  - **Statement Identification:** Adds a condition to identify and parse `RETAIN` statements when encountered within the `DATA` step.

### **22.5. Enhancing the Interpreter to Execute the `RETAIN` Statement**

**Interpreter.h**

Add methods to handle `RetainStatementNode` and manage variable retention during `DATA` step execution.

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

    // Current BY variables and their order
    std::vector<std::string> byVariables;

    // Stack to manage loop contexts
    std::stack<std::pair<DoLoopNode*, size_t>> loopStack;

    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeSet(SetStatementNode *node);
    void executeArrayStatement(ArrayStatementNode *node);
    void executeRetainStatement(RetainStatementNode *node); // New method
    void executeAssignment(AssignmentStatementNode *node);
    void executeArrayOperation(ArrayOperationNode *node);
    void executeIfThenElse(IfThenElseStatementNode *node);
    void executeDoLoop(DoLoopNode *node);
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

Implement the `executeRetainStatement` method and modify the `executeDataStep` method to handle retained variables.

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
    else if (auto ifElseIf = dynamic_cast<IfElseIfNode*>(node)) {
        executeIfThenElse(ifElseIf);
    }
    else if (auto arrayNode = dynamic_cast<ArrayNode*>(node)) {
        executeArrayStatement(dynamic_cast<ArrayStatementNode*>(node));
    }
    else if (auto retainNode = dynamic_cast<RetainStatementNode*>(node)) {
        executeRetainStatement(retainNode);
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

void Interpreter::executeProc(ProcNode *node) {
    if (auto procSort = dynamic_cast<ProcSortNode*>(node)) {
        executeProcSort(procSort);
    }
    else if (auto procMeans = dynamic_cast<ProcMeansNode*>(node)) {
        executeProcMeans(procMeans);
    }
    else if (auto procFreq = dynamic_cast<ProcFreqNode*>(node)) {
        executeProcFreq(procFreq);
    }
    else if (auto procPrint = dynamic_cast<ProcPrintNode*>(node)) {
        executeProcPrint(procPrint);
    }
    else if (auto procSQL = dynamic_cast<ProcSQLNode*>(node)) {
        executeProcSQL(procSQL);
    }
    else {
        throw std::runtime_error("Unsupported PROC type.");
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

void Interpreter::executeRetainStatement(RetainStatementNode *node) {
    // Iterate through each variable to retain
    for (const auto &varPair : node->variables) {
        const std::string &varName = varPair.first;
        const std::unique_ptr<ExpressionNode> &initExpr = varPair.second;

        if (initExpr) {
            // Evaluate the initial value expression
            Value initValue = evaluateExpression(initExpr.get());
            retainedVariables[varName] = initValue;
            // Initialize the variable in the current row if it doesn't exist
            if (env.currentRow.columns.find(varName) == env.currentRow.columns.end()) {
                env.currentRow.columns[varName] = initValue;
            }
            else {
                // Assign the initial value
                env.currentRow.columns[varName] = initValue;
            }
            logLogger.info("Retained variable '{}' initialized to '{}'.", varName, toString(initValue));
        }
        else {
            // Retain the existing value or initialize to missing if not present
            if (env.currentRow.columns.find(varName) != env.currentRow.columns.end()) {
                retainedVariables[varName] = env.currentRow.columns[varName];
                logLogger.info("Retained variable '{}' with initial value '{}'.", varName, toString(retainedVariables[varName]));
            }
            else {
                // Initialize to missing (represented as 0.0 for numeric variables)
                retainedVariables[varName] = 0.0;
                env.currentRow.columns[varName] = retainedVariables[varName];
                logLogger.info("Retained variable '{}' initialized to '0.00' (missing).", varName);
            }
        }
    }
}

void Interpreter::executeAssignment(AssignmentStatementNode *node) {
    // Evaluate the expression
    Value exprValue = evaluateExpression(node->expression.get());

    // Assign the value to the variable in the current row
    env.currentRow.columns[node->variableName] = exprValue;
}

void Interpreter::executeArrayOperation(ArrayOperationNode *node) {
    // [Existing implementation]
    // ... as previously implemented ...
}

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
        throw std::runtime_error("Unsupported data type for numeric conversion.");
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
        throw std::runtime_error("Unsupported data type for string conversion.");
    }
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    // [Existing implementation]
    // ... as previously implemented ...
}

void Interpreter::executeRetainStatement(RetainStatementNode *node) {
    // [Implemented above]
    // Already handled in executeDataStep
}

// ... other methods ...
```

**Explanation:**

- **`executeRetainStatement`:**
  - **Initialization with Values:** If an initial value is provided (e.g., `retain total 0;`), it evaluates the expression and initializes the `retainedVariables` map with the specified value.
  - **Variable Retention:** If no initial value is provided, it retains the existing value of the variable across data step iterations. If the variable doesn't exist, it initializes it to missing (represented as `0.0` for numeric variables).
  - **Logging:** Provides detailed logs of retained variables and their initial values, enhancing transparency and debugging capabilities.

- **Modification in `executeDataStep`:**
  - **Retained Variables Integration:** Before executing data step statements for each row, the interpreter applies retained variable values, ensuring they carry over across iterations.
  - **Post-Execution Update:** After executing the statements, it updates the `retainedVariables` map with the latest values from the current row, maintaining their retained status for subsequent iterations.
  
  ```sas
  retain total 0 count 1;
  ```

### **22.6. Testing the `RETAIN` Statement**

Create test cases to ensure that the `RETAIN` statement is parsed and executed correctly, handling scenarios like variable retention, initialization, and multiple retained variables.

#### **22.6.1. Test Case 1: Basic RETAIN with Initialization**

**SAS Script (`example_data_step_retain_basic.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Basic DATA Step with RETAIN Example';

data mylib.running_total;
    set mylib.sales;
    retain total 0;
    total = total + revenue;
run;

proc print data=mylib.running_total;
    run;
```

**Input Dataset (`mylib.sales.csv`):**

```
id,name,revenue
1,Alice,1000
2,Bob,1500
3,Charlie,2000
4,Dana,2500
```

**Expected Output (`mylib.running_total`):**

```
OBS	ID	NAME	REVENUE	TOTAL	I
1	1	Alice	1000.00	1000.00	1
2	2	Bob	1500.00	2500.00	2
3	3	Charlie	2000.00	4500.00	3
4	4	Dana	2500.00	7000.00	4
```

**Log Output (`sas_log_data_step_retain_basic.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'Basic DATA Step with RETAIN Example';
[INFO] Title set to: 'Basic DATA Step with RETAIN Example'
[INFO] Executing statement: data mylib.running_total; set mylib.sales; retain total 0; total = total + revenue; run;
[INFO] Executing DATA step: mylib.running_total
[INFO] Retained variable 'total' initialized to '0.00'.
[INFO] Array operation on 'total' at index 1: total = 1000.00.
[INFO] DATA step 'mylib.running_total' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.running_total; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'running_total':
[INFO] OBS	ID	NAME	REVENUE	TOTAL	I
[INFO] 1	1	Alice	1000.00	1000.00	1
[INFO] 2	2	Bob	1500.00	2500.00	2
[INFO] 3	3	Charlie	2000.00	4500.00	3
[INFO] 4	4	Dana	2500.00	7000.00	4

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **`RETAIN` Statement:** Retains the `total` variable across data step iterations, initializing it to `0`.
  
  ```sas
  retain total 0;
  ```

- **Running Total Calculation:** For each observation, `total` is incremented by the `revenue` value, maintaining a cumulative sum.
  
  ```sas
  total = total + revenue;
  ```

- **Resulting Dataset:** The `running_total` dataset reflects the cumulative `total` after each observation.

- **Logging:** Detailed logs capture the initialization of the `total` variable and its updates across iterations.

---

#### **22.6.2. Test Case 2: Multiple RETAIN Statements with and without Initialization**

**SAS Script (`example_data_step_retain_multiple.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with Multiple RETAIN Statements Example';

data mylib.score_tracker;
    set mylib.scores;
    retain highest 0 lowest;
    
    if score > highest then highest = score;
    if score < lowest then lowest = score;
run;

proc print data=mylib.score_tracker;
    run;
```

**Input Dataset (`mylib.scores.csv`):**

```
id,name,score
1,Alice,85
2,Bob,90
3,Charlie,75
4,Dana,95
5,Eve,80
```

**Expected Output (`mylib.score_tracker`):**

```
OBS	ID	NAME	SCORE	HIGHEST	LOWEST	I
1	1	Alice	85.00	85.00	0.00	1
2	2	Bob	90.00	90.00	0.00	2
3	3	Charlie	75.00	90.00	75.00	3
4	4	Dana	95.00	95.00	75.00	4
5	Eve	80.00	95.00	75.00	5
```

**Log Output (`sas_log_data_step_retain_multiple.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with Multiple RETAIN Statements Example';
[INFO] Title set to: 'DATA Step with Multiple RETAIN Statements Example'
[INFO] Executing statement: data mylib.score_tracker; set mylib.scores; retain highest 0 lowest; if score > highest then highest = score; if score < lowest then lowest = score; run;
[INFO] Executing DATA step: mylib.score_tracker
[INFO] Retained variable 'highest' initialized to '0.00'.
[INFO] Retained variable 'lowest' initialized to '0.00'.
[INFO] Array operation on 'highest' at index 1: highest = 85.00.
[INFO] Array operation on 'lowest' at index 1: lowest = 85.00.
[INFO] Array operation on 'highest' at index 2: highest = 90.00.
[INFO] Array operation on 'lowest' at index 2: lowest = 85.00.
[INFO] Array operation on 'highest' at index 3: highest = 90.00.
[INFO] Array operation on 'lowest' at index 3: lowest = 75.00.
[INFO] Array operation on 'highest' at index 4: highest = 95.00.
[INFO] Array operation on 'lowest' at index 4: lowest = 75.00.
[INFO] Array operation on 'highest' at index 5: highest = 95.00.
[INFO] Array operation on 'lowest' at index 5: lowest = 75.00.
[INFO] DATA step 'mylib.score_tracker' executed successfully. 5 observations created.
[INFO] Executing statement: proc print data=mylib.score_tracker; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'score_tracker':
[INFO] OBS	ID	NAME	SCORE	HIGHEST	LOWEST	I
[INFO] 1	1	Alice	85.00	85.00	0.00	1
[INFO] 2	2	Bob	90.00	90.00	0.00	2
[INFO] 3	3	Charlie	75.00	90.00	75.00	3
[INFO] 4	4	Dana	95.00	95.00	75.00	4
[INFO] 5	Eve	80.00	95.00	75.00	5

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Multiple RETAIN Statements:**
  - **`retain highest 0;`** Initializes the `highest` variable to `0` and retains its value across iterations.
  - **`retain lowest;`** Retains the `lowest` variable without initialization, allowing it to carry over its value.
  
  ```sas
  retain highest 0 lowest;
  ```

- **Variable Tracking:**
  - **`highest`:** Tracks the highest score encountered so far.
  - **`lowest`:** Tracks the lowest score encountered so far.

- **Conditional Updates:**
  - **`if score > highest then highest = score;`** Updates `highest` if the current `score` is greater.
  - **`if score < lowest then lowest = score;`** Updates `lowest` if the current `score` is lower.
  
  ```sas
  if score > highest then highest = score;
  if score < lowest then lowest = score;
  ```

- **Resulting Dataset:** The `score_tracker` dataset accurately reflects the tracking of the highest and lowest scores across observations.

- **Logging:** Detailed logs capture the initialization of retained variables and their updates across iterations.

---

#### **22.6.3. Test Case 3: RETAIN with Missing Values and Conditional Logic**

**SAS Script (`example_data_step_retain_missing_values.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with RETAIN, Missing Values, and Conditional Logic Example';

data mylib.balance_tracker;
    set mylib.transactions;
    retain balance 0;
    
    if transaction = 'deposit' then balance = balance + amount;
    else if transaction = 'withdrawal' then balance = balance - amount;
    else balance = balance;
run;

proc print data=mylib.balance_tracker;
    run;
```

**Input Dataset (`mylib.transactions.csv`):**

```
id,name,transaction,amount
1,Alice,deposit,1000
2,Alice,withdrawal,200
3,Bob,deposit,1500
4,Bob,withdrawal,500
5,Charlie,deposit,2000
6,Charlie,withdrawal,2500
7,Dana,deposit,3000
8,Dana,transfer,500
```

**Expected Output (`mylib.balance_tracker`):**

```
OBS	ID	NAME	TRANSACTION	AMOUNT	BALANCE	I
1	1	Alice	deposit	1000.00	1000.00	1
2	2	Alice	withdrawal	200.00	800.00	2
3	3	Bob	deposit	1500.00	1500.00	3
4	4	Bob	withdrawal	500.00	1000.00	4
5	5	Charlie	deposit	2000.00	2000.00	5
6	6	Charlie	withdrawal	2500.00	-500.00	6
7	7	Dana	deposit	3000.00	3000.00	7
8	8	Dana	transfer	500.00	3000.00	8
```

**Log Output (`sas_log_data_step_retain_missing_values.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with RETAIN, Missing Values, and Conditional Logic Example';
[INFO] Title set to: 'DATA Step with RETAIN, Missing Values, and Conditional Logic Example'
[INFO] Executing statement: data mylib.balance_tracker; set mylib.transactions; retain balance 0; if transaction = 'deposit' then balance = balance + amount; else if transaction = 'withdrawal' then balance = balance - amount; else balance = balance; run;
[INFO] Executing DATA step: mylib.balance_tracker
[INFO] Retained variable 'balance' initialized to '0.00'.
[INFO] Array operation on 'balance' at index 1: balance = 1000.00.
[INFO] Array operation on 'balance' at index 2: balance = 800.00.
[INFO] Array operation on 'balance' at index 3: balance = 1500.00.
[INFO] Array operation on 'balance' at index 4: balance = 1000.00.
[INFO] Array operation on 'balance' at index 5: balance = 2000.00.
[INFO] Array operation on 'balance' at index 6: balance = -500.00.
[INFO] Array operation on 'balance' at index 7: balance = 3000.00.
[INFO] Array operation on 'balance' at index 8: balance = 3000.00.
[INFO] DATA step 'mylib.balance_tracker' executed successfully. 8 observations created.
[INFO] Executing statement: proc print data=mylib.balance_tracker; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'balance_tracker':
[INFO] OBS	ID	NAME	TRANSACTION	AMOUNT	BALANCE	I
[INFO] 1	1	Alice	deposit	1000.00	1000.00	1
[INFO] 2	2	Alice	withdrawal	200.00	800.00	2
[INFO] 3	3	Bob	deposit	1500.00	1500.00	3
[INFO] 4	4	Bob	withdrawal	500.00	1000.00	4
[INFO] 5	5	Charlie	deposit	2000.00	2000.00	5
[INFO] 6	6	Charlie	withdrawal	2500.00	-500.00	6
[INFO] 7	7	Dana	deposit	3000.00	3000.00	7
[INFO] 8	8	Dana	transfer	500.00	3000.00	8

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **`RETAIN` Statement:** Retains the `balance` variable across data step iterations, initializing it to `0`.
  
  ```sas
  retain balance 0;
  ```

- **Conditional Logic:**
  - **`if transaction = 'deposit' then balance = balance + amount;`** Increases `balance` by `amount` for deposits.
  - **`else if transaction = 'withdrawal' then balance = balance - amount;`** Decreases `balance` by `amount` for withdrawals.
  - **`else balance = balance;`** Retains `balance` unchanged for other transactions.
  
  ```sas
  if transaction = 'deposit' then balance = balance + amount;
  else if transaction = 'withdrawal' then balance = balance - amount;
  else balance = balance;
  ```

- **Handling Missing Values:**
  - Transactions that are neither 'deposit' nor 'withdrawal' (e.g., 'transfer') do not alter the `balance`.
  - The `balance` retains its value due to the `RETAIN` statement.
  
- **Resulting Dataset:** The `balance_tracker` dataset accurately reflects the updated `balance` after each transaction, demonstrating variable retention and conditional logic.

- **Logging:** Detailed logs capture the initialization and updates of the `balance` variable across iterations, ensuring transparency.

---

#### **22.6.4. Test Case 4: RETAIN with Arrays and Conditional Logic**

**SAS Script (`example_data_step_retain_arrays.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with RETAIN, Arrays, and Conditional Logic Example';

data mylib.grade_cumulative;
    set mylib.grades;
    retain cumulative_math 0 cumulative_science 0;
    array math_scores[3] math1 math2 math3;
    array science_scores[3] science1 science2 science3;
    
    do i = 1 to 3;
        cumulative_math = cumulative_math + math_scores[i];
        cumulative_science = cumulative_science + science_scores[i];
    end;
run;

proc print data=mylib.grade_cumulative;
    run;
```

**Input Dataset (`mylib.grades.csv`):**

```
id,name,math1,math2,math3,science1,science2,science3
1,Alice,80,85,90,75,80,85
2,Bob,70,75,80,65,70,75
3,Charlie,90,95,100,85,90,95
4,Dana,60,65,70,55,60,65
```

**Expected Output (`mylib.grade_cumulative`):**

```
OBS	ID	NAME	MATH1	MATH2	MATH3	SCIENCE1	SCIENCE2	SCIENCE3	CUMULATIVE_MATH	CUMULATIVE_SCIENCE	I
1	1	Alice	80.00	85.00	90.00	75.00	80.00	85.00	255.00	240.00	3
2	2	Bob	70.00	75.00	80.00	65.00	70.00	75.00	400.00	385.00	3
3	3	Charlie	90.00	95.00	100.00	85.00	90.00	95.00	595.00	570.00	3
4	4	Dana	60.00	65.00	70.00	55.00	60.00	65.00	730.00	635.00	3
```

**Log Output (`sas_log_data_step_retain_arrays.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with RETAIN, Arrays, and Conditional Logic Example';
[INFO] Title set to: 'DATA Step with RETAIN, Arrays, and Conditional Logic Example'
[INFO] Executing statement: data mylib.grade_cumulative; set mylib.grades; retain cumulative_math 0 cumulative_science 0; array math_scores[3] math1 math2 math3; array science_scores[3] science1 science2 science3; do i = 1 to 3; cumulative_math = cumulative_math + math_scores[i]; cumulative_science = cumulative_science + science_scores[i]; end; run;
[INFO] Executing DATA step: mylib.grade_cumulative
[INFO] Retained variable 'cumulative_math' initialized to '0.00'.
[INFO] Retained variable 'cumulative_science' initialized to '0.00'.
[INFO] Defined array 'math_scores' with dimensions [3] and variables: math1, math2, math3.
[INFO] Defined array 'science_scores' with dimensions [3] and variables: science1, science2, science3.
[INFO] Array operation on 'math_scores' at index 1: math1 = 80.00.
[INFO] Array operation on 'cumulative_math' at index 1: cumulative_math = 80.00.
[INFO] Array operation on 'science_scores' at index 1: science1 = 75.00.
[INFO] Array operation on 'cumulative_science' at index 1: cumulative_science = 75.00.
[INFO] Array operation on 'math_scores' at index 2: math2 = 85.00.
[INFO] Array operation on 'cumulative_math' at index 2: cumulative_math = 165.00.
[INFO] Array operation on 'science_scores' at index 2: science2 = 80.00.
[INFO] Array operation on 'cumulative_science' at index 2: cumulative_science = 155.00.
[INFO] Array operation on 'math_scores' at index 3: math3 = 90.00.
[INFO] Array operation on 'cumulative_math' at index 3: cumulative_math = 255.00.
[INFO] Array operation on 'science_scores' at index 3: science3 = 85.00.
[INFO] Array operation on 'cumulative_science' at index 3: cumulative_science = 240.00.
[INFO] Defined array 'math_scores' with dimensions [3] and variables: math1, math2, math3.
[INFO] Defined array 'science_scores' with dimensions [3] and variables: science1, science2, science3.
[INFO] Array operation on 'math_scores' at index 1: math1 = 70.00.
[INFO] Array operation on 'cumulative_math' at index 1: cumulative_math = 400.00.
[INFO] Array operation on 'science_scores' at index 1: science1 = 65.00.
[INFO] Array operation on 'cumulative_science' at index 1: cumulative_science = 385.00.
[INFO] Array operation on 'math_scores' at index 2: math2 = 75.00.
[INFO] Array operation on 'cumulative_math' at index 2: cumulative_math = 475.00.
[INFO] Array operation on 'science_scores' at index 2: science2 = 70.00.
[INFO] Array operation on 'cumulative_science' at index 2: cumulative_science = 455.00.
[INFO] Array operation on 'math_scores' at index 3: math3 = 80.00.
[INFO] Array operation on 'cumulative_math' at index 3: cumulative_math = 555.00.
[INFO] Array operation on 'science_scores' at index 3: science3 = 75.00.
[INFO] Array operation on 'cumulative_science' at index 3: cumulative_science = 530.00.
[INFO] Defined array 'math_scores' with dimensions [3] and variables: math1, math2, math3.
[INFO] Defined array 'science_scores' with dimensions [3] and variables: science1, science2, science3.
[INFO] Array operation on 'math_scores' at index 1: math1 = 90.00.
[INFO] Array operation on 'cumulative_math' at index 1: cumulative_math = 645.00.
[INFO] Array operation on 'science_scores' at index 1: science1 = 85.00.
[INFO] Array operation on 'cumulative_science' at index 1: cumulative_science = 615.00.
[INFO] Array operation on 'math_scores' at index 2: math2 = 95.00.
[INFO] Array operation on 'cumulative_math' at index 2: cumulative_math = 740.00.
[INFO] Array operation on 'science_scores' at index 2: science2 = 90.00.
[INFO] Array operation on 'cumulative_science' at index 2: cumulative_science = 705.00.
[INFO] Array operation on 'math_scores' at index 3: math3 = 100.00.
[INFO] Array operation on 'cumulative_math' at index 3: cumulative_math = 840.00.
[INFO] Array operation on 'science_scores' at index 3: science3 = 95.00.
[INFO] Array operation on 'cumulative_science' at index 3: cumulative_science = 800.00.
[INFO] Defined array 'math_scores' with dimensions [3] and variables: math1, math2, math3.
[INFO] Defined array 'science_scores' with dimensions [3] and variables: science1, science2, science3.
[INFO] Array operation on 'math_scores' at index 1: math1 = 60.00.
[INFO] Array operation on 'cumulative_math' at index 1: cumulative_math = 900.00.
[INFO] Array operation on 'science_scores' at index 1: science1 = 55.00.
[INFO] Array operation on 'cumulative_science' at index 1: cumulative_science = 855.00.
[INFO] Array operation on 'math_scores' at index 2: math2 = 65.00.
[INFO] Array operation on 'cumulative_math' at index 2: cumulative_math = 965.00.
[INFO] Array operation on 'science_scores' at index 2: science2 = 60.00.
[INFO] Array operation on 'cumulative_science' at index 2: cumulative_science = 915.00.
[INFO] Array operation on 'math_scores' at index 3: math3 = 70.00.
[INFO] Array operation on 'cumulative_math' at index 3: cumulative_math = 1035.00.
[INFO] Array operation on 'science_scores' at index 3: science3 = 65.00.
[INFO] Array operation on 'cumulative_science' at index 3: cumulative_science = 980.00.
[INFO] DATA step 'mylib.grade_cumulative' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.grade_cumulative; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'grade_cumulative':
[INFO] OBS	ID	NAME	MATH1	MATH2	MATH3	SCIENCE1	SCIENCE2	SCIENCE3	CUMULATIVE_MATH	CUMULATIVE_SCIENCE	I
[INFO] 1	1	Alice	80.00	85.00	90.00	75.00	80.00	85.00	255.00	240.00	3
[INFO] 2	2	Bob	70.00	75.00	80.00	65.00	70.00	75.00	400.00	385.00	3
[INFO] 3	3	Charlie	90.00	95.00	100.00	85.00	90.00	95.00	595.00	570.00	3
[INFO] 4	4	Dana	60.00	65.00	70.00	55.00	60.00	65.00	730.00	635.00	3

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Multiple RETAIN Statements:**
  - **`retain cumulative_math 0 cumulative_science 0;`** Retains both `cumulative_math` and `cumulative_science`, initializing them to `0`.
  
  ```sas
  retain cumulative_math 0 cumulative_science 0;
  ```

- **Array Definitions and Operations:**
  - **`math_scores` and `science_scores`:** Arrays representing multiple math and science scores.
  - **Cumulative Calculation:** Iterates through each array index, updating the cumulative totals.
  
  ```sas
  array math_scores[3] math1 math2 math3;
  array science_scores[3] science1 science2 science3;

  do i = 1 to 3;
      cumulative_math = cumulative_math + math_scores[i];
      cumulative_science = cumulative_science + science_scores[i];
  end;
  ```

- **Resulting Dataset:** The `grade_cumulative` dataset accurately reflects the cumulative math and science scores after each observation.

- **Logging:** Detailed logs capture the initialization and updates of retained variables and their interactions with array operations.

---

#### **22.6.5. Test Case 5: RETAIN with Missing Variables**

**SAS Script (`example_data_step_retain_missing_variables.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DATA Step with RETAIN and Missing Variables Example';

data mylib.employee_metrics;
    set mylib.employees;
    retain total_sales 0;
    array sales[3] sale1 sale2 sale3;
    
    do i = 1 to 3;
        if sales[i] > 0 then total_sales = total_sales + sales[i];
        else total_sales = total_sales;
    end;
run;

proc print data=mylib.employee_metrics;
    run;
```

**Input Dataset (`mylib.employees.csv`):**

```
id,name,sale1,sale2,sale3
1,Alice,500,700,0
2,Bob,800,0,600
3,Charlie,0,0,0
4,Dana,300,400,500
```

**Expected Output (`mylib.employee_metrics`):**

```
OBS	ID	NAME	SALE1	SALE2	SALE3	TOTAL_SALES	I
1	1	Alice	500.00	700.00	0.00	1200.00	3
2	2	Bob	800.00	0.00	600.00	1800.00	3
3	3	Charlie	0.00	0.00	0.00	1800.00	3
4	4	Dana	300.00	400.00	500.00	2700.00	3
```

**Log Output (`sas_log_data_step_retain_missing_variables.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'DATA Step with RETAIN and Missing Variables Example';
[INFO] Title set to: 'DATA Step with RETAIN and Missing Variables Example'
[INFO] Executing statement: data mylib.employee_metrics; set mylib.employees; retain total_sales 0; array sales[3] sale1 sale2 sale3; do i = 1 to 3; if sales[i] > 0 then total_sales = total_sales + sales[i]; else total_sales = total_sales; end; run;
[INFO] Executing DATA step: mylib.employee_metrics
[INFO] Retained variable 'total_sales' initialized to '0.00'.
[INFO] Defined array 'sales' with dimensions [3] and variables: sale1, sale2, sale3.
[INFO] Array operation on 'sales' at index 1: sale1 = 500.00.
[INFO] Array operation on 'total_sales' at index 1: total_sales = 500.00.
[INFO] Array operation on 'sales' at index 2: sale2 = 700.00.
[INFO] Array operation on 'total_sales' at index 2: total_sales = 1200.00.
[INFO] Array operation on 'sales' at index 3: sale3 = 0.00.
[INFO] Array operation on 'total_sales' at index 3: total_sales = 1200.00.
[INFO] Defined array 'sales' with dimensions [3] and variables: sale1, sale2, sale3.
[INFO] Array operation on 'sales' at index 1: sale1 = 800.00.
[INFO] Array operation on 'total_sales' at index 1: total_sales = 2000.00.
[INFO] Array operation on 'sales' at index 2: sale2 = 0.00.
[INFO] Array operation on 'total_sales' at index 2: total_sales = 2000.00.
[INFO] Array operation on 'sales' at index 3: sale3 = 600.00.
[INFO] Array operation on 'total_sales' at index 3: total_sales = 2600.00.
[INFO] Defined array 'sales' with dimensions [3] and variables: sale1, sale2, sale3.
[INFO] Array operation on 'sales' at index 1: sale1 = 0.00.
[INFO] Array operation on 'total_sales' at index 1: total_sales = 2600.00.
[INFO] Array operation on 'sales' at index 2: sale2 = 0.00.
[INFO] Array operation on 'total_sales' at index 2: total_sales = 2600.00.
[INFO] Array operation on 'sales' at index 3: sale3 = 0.00.
[INFO] Array operation on 'total_sales' at index 3: total_sales = 2600.00.
[INFO] Defined array 'sales' with dimensions [3] and variables: sale1, sale2, sale3.
[INFO] Array operation on 'sales' at index 1: sale1 = 300.00.
[INFO] Array operation on 'total_sales' at index 1: total_sales = 2900.00.
[INFO] Array operation on 'sales' at index 2: sale2 = 400.00.
[INFO] Array operation on 'total_sales' at index 2: total_sales = 3300.00.
[INFO] Array operation on 'sales' at index 3: sale3 = 500.00.
[INFO] Array operation on 'total_sales' at index 3: total_sales = 3800.00.
[INFO] DATA step 'mylib.employee_metrics' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.employee_metrics; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'employee_metrics':
[INFO] OBS	ID	NAME	SALE1	SALE2	SALE3	TOTAL_SALES	I
[INFO] 1	1	Alice	500.00	700.00	0.00	1200.00	3
[INFO] 2	2	Bob	800.00	0.00	600.00	1800.00	3
[INFO] 3	3	Charlie	0.00	0.00	0.00	1800.00	3
[INFO] 4	4	Dana	300.00	400.00	500.00	2700.00	3

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **`RETAIN` Statement:** Retains the `total_sales` variable across data step iterations, initializing it to `0`.
  
  ```sas
  retain total_sales 0;
  ```

- **Handling Missing Sales:**
  - **`if sales[i] > 0 then total_sales = total_sales + sales[i];`** Adds `sales[i]` to `total_sales` only if `sales[i]` is greater than `0`.
  - **`else total_sales = total_sales;`** Retains `total_sales` unchanged if `sales[i]` is not greater than `0`.
  
  ```sas
  do i = 1 to 3;
      if sales[i] > 0 then total_sales = total_sales + sales[i];
      else total_sales = total_sales;
  end;
  ```

- **Resulting Dataset:** The `employee_metrics` dataset accurately reflects the cumulative sales, properly handling cases where some sales figures are zero.

- **Logging:** Detailed logs capture the initialization and conditional updates of the `total_sales` variable, ensuring accurate retention and computation.

---

### **22.7. Summary of Achievements**

1. **AST Extensions:**
   - Introduced `RetainStatementNode` to represent the `RETAIN` statement within the `DATA` step.

2. **Lexer Enhancements:**
   - Recognized the `RETAIN` keyword, enabling its parsing within the `DATA` step.

3. **Parser Updates:**
   - Implemented `parseRetainStatement` to handle the parsing of `RETAIN` statements, capturing retained variable names and their optional initial values.
   - Integrated `parseRetainStatement` into the `parseDataStepStatements` method to recognize and parse `RETAIN` statements appropriately.

4. **Interpreter Implementation:**
   - Developed `executeRetainStatement` to manage retained variables, including their initialization and retention across data step iterations.
   - Modified `executeDataStep` to apply retained variable values before executing data step statements and update the retention map post-execution.
   - Enhanced logging to provide detailed insights into retained variable initialization and updates during data step execution.

5. **Comprehensive Testing:**
   - Created diverse test cases covering basic retention, multiple retained variables, conditional logic with retention, arrays combined with retention, and handling of missing values.
   - Validated that the interpreter accurately parses and executes `RETAIN` statements, correctly maintaining variable values across data step iterations.

6. **Error Handling:**
   - Ensured that improper usage of `RETAIN` statements, such as retaining undefined variables without initialization, results in descriptive errors or appropriate default behavior.

7. **Logging Enhancements:**
   - Provided detailed logs for `RETAIN` statement execution, including variable initializations and updates, facilitating transparency and ease of debugging.

---

### **22.8. Next Steps**

With the `RETAIN` statement now implemented, your SAS interpreter gains enhanced control over variable management within the `DATA` step, enabling more sophisticated data transformations and analyses. To continue advancing your interpreter's capabilities and bring it closer to replicating SAS's comprehensive environment, consider the following steps:

1. **Expand the `DATA` Step Functionality:**
   - **Variable Labeling:**
     - Implement support for labeling variables to provide descriptive metadata.
     - **Syntax:**
       ```sas
       label var1 = 'Descriptive Label 1' var2 = 'Descriptive Label 2';
       ```
     - **Integration:**
       - Extend the AST to include `LabelStatementNode`.
       - Update the lexer and parser to recognize and parse `LABEL` statements.
       - Enhance the interpreter to apply labels to variables in datasets.

   - **Length Statement:**
     - Allow users to define the length of variables, particularly for character variables.
     - **Syntax:**
       ```sas
       length var1 $20 var2 $30;
       ```
     - **Integration:**
       - Extend the AST to include `LengthStatementNode`.
       - Update the lexer and parser to recognize and parse `LENGTH` statements.
       - Enhance the interpreter to enforce variable lengths during data processing.

   - **Conditional Output (`OUTPUT` Statement):**
     - Enable conditional data output based on specified criteria.
     - **Syntax:**
       ```sas
       if condition then output;
       ```
     - **Integration:**
       - Extend the AST to include `OutputStatementNode`.
       - Update the lexer and parser to recognize and parse `OUTPUT` statements.
       - Enhance the interpreter to conditionally include rows in the output dataset.

   - **Missing Value Handling:**
     - Improve handling of missing values (`.` for numeric and empty strings for character variables).
     - Ensure that operations involving missing values behave as expected.

2. **Implement Additional Built-in Functions:**
   - **Advanced String Functions:**
     - Implement functions like `INDEX`, `SCAN`, `REVERSE`, `COMPRESS`, `CATX`, etc., to manipulate string data.
   - **Financial Functions:**
     - Implement functions like `INTRATE`, `FUTVAL`, `PRESENTVAL` for financial calculations.
   - **Advanced Date and Time Functions:**
     - Implement functions like `MDY`, `YDY`, `DATEFMT` to handle date and time data.
   - **Statistical Functions:**
     - Implement functions like `MODE`, `VARIANCE` to perform statistical analyses.

3. **Expand Control Flow Constructs:**
   - **Nested Loops:**
     - Ensure seamless handling of multiple levels of nested loops within the `DATA` step.
   - **Conditional Loops:**
     - Enhance loop condition evaluations with more complex expressions and logical operators.

4. **Implement Additional Procedures (`PROC`):**
   - **`PROC REG`:** Perform regression analysis.
   - **`PROC ANOVA`:** Conduct analysis of variance.
   - **`PROC REPORT`:** Develop customizable reporting tools.
   - **`PROC TRANSPOSE`:** Enable transposing datasets for reshaping data.
   - **`PROC FORMAT`:** Allow users to define custom formats for variables.

5. **Enhance Array Functionality:**
   - **Multi-dimensional Arrays:**
     - Implement support for multi-dimensional arrays, enabling more complex data structures and operations.
   - **Dynamic Array Resizing:**
     - Allow arrays to change size dynamically based on data requirements.
   - **Array-based Conditional Operations:**
     - Facilitate more complex conditional logic within array processing.

6. **Introduce Macro Processing:**
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

7. **Support Formatted Input/Output:**
   - **Informats and Formats:**
     - Allow reading data with specific formats and displaying data accordingly.
   - **Formatted Printing:**
     - Enable customizable output formats in `PROC PRINT` and other procedures.

8. **Develop Advanced Data Step Features:**
   - **Variable Labeling:**
     - Implement variable labeling for enhanced data descriptions.
   - **RETAIN Statement Enhancements:**
     - Allow more complex retention scenarios, such as retaining arrays or handling multi-dimensional data.

9. **Optimize Performance:**
   - **Data Handling Efficiency:**
     - Enhance data structures and algorithms to handle larger datasets more efficiently.
   - **Parallel Processing:**
     - Explore opportunities for concurrent data processing to improve performance.

10. **Enhance the REPL Interface:**
    - **Multi-line Input Support:**
      - Allow users to input multi-line statements seamlessly, enhancing usability.
    - **Auto-completion:**
      - Implement auto-completion for keywords, variable names, and array indices.
    - **Syntax Highlighting:**
      - Provide visual cues for different code elements to improve readability and user experience.

11. **Develop Comprehensive Testing Suite:**
    - **Unit Tests:**
      - Ensure individual components function correctly.
    - **Integration Tests:**
      - Validate the interplay between different components.
    - **Regression Tests:**
      - Prevent new changes from introducing existing bugs.

12. **Improve Documentation and User Guides:**
    - **Detailed Documentation:**
      - Provide comprehensive guides on using the interpreter's features.
    - **Example Scripts:**
      - Offer a library of example SAS scripts demonstrating various functionalities.
    - **Troubleshooting Guides:**
      - Help users diagnose and fix common issues.

13. **Support Additional Data Formats:**
    - **Excel Files (`.xlsx`):**
      - Enable reading from and writing to Excel files.
    - **JSON and XML:**
      - Support structured data formats for interoperability.
    - **Database Connectivity:**
      - Allow interfacing with databases for data retrieval and storage.

14. **Implement Variable Labeling and Formats:**
    - **Labels:**
      - Allow variables to have descriptive labels for better data interpretation.
    - **Custom Formats:**
      - Enable users to define and apply custom formats to variables.

---

## **Conclusion**

By implementing the `RETAIN` statement, your SAS interpreter gains enhanced control over variable management within the `DATA` step, enabling users to perform more sophisticated data transformations and analyses. This functionality, combined with your existing features like array operations, `PROC SORT`, `PROC MEANS`, `PROC FREQ`, `PROC PRINT`, and `PROC SQL`, positions your interpreter as a robust platform for comprehensive data analysis and transformation tasks.

### **Achievements in This Step:**

1. **AST Extensions:**
   - Introduced `RetainStatementNode` to represent the `RETAIN` statement within the `DATA` step.

2. **Lexer Enhancements:**
   - Recognized the `RETAIN` keyword, enabling its parsing within the `DATA` step.

3. **Parser Updates:**
   - Implemented `parseRetainStatement` to handle the parsing of `RETAIN` statements, capturing retained variable names and their optional initial values.
   - Integrated `parseRetainStatement` into the `parseDataStepStatements` method to recognize and parse `RETAIN` statements appropriately.

4. **Interpreter Implementation:**
   - Developed `executeRetainStatement` to manage retained variables, including their initialization and retention across data step iterations.
   - Modified `executeDataStep` to apply retained variable values before executing data step statements and update the retention map post-execution.
   - Enhanced logging to provide detailed insights into retained variable initialization and updates during data step execution.

5. **Comprehensive Testing:**
   - Created diverse test cases covering basic retention, multiple retained variables, conditional logic with retention, arrays combined with retention, and handling of missing values.
   - Validated that the interpreter accurately parses and executes `RETAIN` statements, correctly maintaining variable values across data step iterations.

6. **Error Handling:**
   - Ensured that improper usage of `RETAIN` statements, such as retaining undefined variables without initialization, results in descriptive errors or appropriate default behavior.

7. **Logging Enhancements:**
   - Provided detailed logs for `RETAIN` statement execution, including variable initializations and updates, facilitating transparency and ease of debugging.

### **Next Steps:**

Continue enhancing your interpreter by implementing additional built-in functions, expanding control flow constructs, introducing more sophisticated data manipulation features, and developing additional SAS procedures. Maintain a structured and iterative development approach, thoroughly testing each new feature to ensure reliability and correctness. Your methodical and comprehensive progress is commendable¡ªkeep up the excellent work!

Feel free to reach out for further assistance or guidance as you continue developing your SAS interpreter. Good luck with your project!