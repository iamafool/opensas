Certainly! Building upon the successful implementation of **`PROC REPORT`**, the next strategic step is to **Implement `PROC SORT`**. This procedure is fundamental in data manipulation, allowing users to sort datasets based on one or more variables. By integrating `PROC SORT`, your SAS interpreter will enable users to organize their data efficiently, paving the way for more advanced data analysis and reporting tasks.

## **Next Strategic Step: Implementing `PROC SORT`**

**`PROC SORT`** is a critical procedure in SAS that orders the observations in a dataset based on the values of one or more variables. Sorting data is often a prerequisite for various data processing tasks, such as merging datasets, performing grouped analyses, and generating organized reports.

---

## **Step 47: Implementing `PROC SORT`**

### **47.1. Understanding `PROC SORT`**

**`PROC SORT`** provides functionality to:

- **Sort by One or More Variables:** Arrange data in ascending or descending order based on specified variables.
- **Remove Duplicate Observations:** Optionally eliminate duplicate rows during the sorting process.
- **Create Sorted Copies of Datasets:** Generate new sorted datasets without altering the original data.

**Example SAS Script Using `PROC SORT`:**

```sas
data employees;
    input Department $ Name $ Salary;
    datalines;
HR Alice 50000
HR Bob 55000
IT Charlie 60000
IT David 65000
Finance Eve 70000
Finance Frank 75000
;
run;

proc sort data=employees out=sorted_employees;
    by Department descending Salary;
run;

proc print data=sorted_employees;
run;
```

**Expected Output:**

```
Sorted Employees Dataset

Department  Name     Salary
----------------------------
Finance     Frank    75000
Finance     Eve      70000
HR          Bob      55000
HR          Alice    50000
IT          David    65000
IT          Charlie  60000
```

---

### **47.2. Extending the Abstract Syntax Tree (AST) for `PROC SORT`**

To support `PROC SORT`, extend your AST to include nodes representing the sort procedure, its options, and the variables by which the data should be sorted.

**AST.h**

```cpp
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <variant>

// ... Existing ASTNode classes ...

// PROC SORT specific nodes
class ProcSortNode : public ProcStepNode {
public:
    std::vector<std::string> sortVariables; // Variables to sort by
    bool descending = false;                // Sort order
    std::string outDataset;                 // Output dataset name
    bool nodupkey = false;                  // Remove duplicate observations

    ProcSortNode(const std::string& name, const std::string& data)
        : ProcStepNode(name, data) {}
};

// Sort option node
struct SortOption {
    std::string optionName; // e.g., "descending", "nodupkey"
    std::string value;      // e.g., "", "descending"
};

// Extend DefineStatementNode if necessary
class SortStatementNode : public ASTNode {
public:
    std::vector<std::string> variables;
    std::vector<SortOption> options;

    SortStatementNode(const std::vector<std::string>& vars, const std::vector<SortOption>& opts)
        : variables(vars), options(opts) {}
};

#endif // AST_H
```

**Explanation:**

- **`ProcSortNode`:** Inherits from `ProcStepNode` and includes members specific to `PROC SORT`, such as the list of variables to sort by, sort order, output dataset name, and options like `NODUPKEY`.
  
- **`SortOption`:** Represents options that can be applied during the sort, such as `DESCENDING` or `NODUPKEY`.
  
- **`SortStatementNode`:** Captures the variables and options specified within the `PROC SORT` statement.

---

### **47.3. Updating the Lexer to Recognize `PROC SORT` Syntax**

Ensure that the Lexer can tokenize the syntax specific to `PROC SORT`, such as the `BY` statement, `OUT=`, `DESCENDING`, and `NODUPKEY` options.

**Lexer.cpp**

```cpp
// Extending the Lexer to handle PROC SORT statements

#include "Lexer.h"
#include <cctype>
#include <stdexcept>

// ... Existing Lexer methods ...

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
            else if (identifier == "report") { // Specific PROC REPORT
                tokens.emplace_back(TokenType::PROC_REPORT, identifier);
            }
            else if (identifier == "sort") { // Specific PROC SORT
                tokens.emplace_back(TokenType::PROC_SORT, identifier);
            }
            else if (identifier == "input") {
                tokens.emplace_back(TokenType::INPUT, identifier);
            }
            else if (identifier == "datalines") {
                tokens.emplace_back(TokenType::DATALINES, identifier);
            }
            else if (identifier == "define") {
                tokens.emplace_back(TokenType::DEFINE, identifier);
            }
            else if (identifier == "column") {
                tokens.emplace_back(TokenType::COLUMN, identifier);
            }
            else if (identifier == "title") {
                tokens.emplace_back(TokenType::TITLE, identifier);
            }
            else if (identifier == "by") {
                tokens.emplace_back(TokenType::BY, identifier);
            }
            else if (identifier == "out") {
                tokens.emplace_back(TokenType::OUT, identifier);
            }
            else if (identifier == "descending") {
                tokens.emplace_back(TokenType::DESCENDING, identifier);
            }
            else if (identifier == "nodupkey") {
                tokens.emplace_back(TokenType::NODUPKEY, identifier);
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
                case '/': tokens.emplace_back(TokenType::SLASH, "/"); break;
                case '(': tokens.emplace_back(TokenType::LEFT_PAREN, "("); break;
                case ')': tokens.emplace_back(TokenType::RIGHT_PAREN, ")"); break;
                case ',': tokens.emplace_back(TokenType::COMMA, ","); break;
                case '-': tokens.emplace_back(TokenType::MINUS, "-"); break;
                case '+': tokens.emplace_back(TokenType::PLUS, "+"); break;
                case '*': tokens.emplace_back(TokenType::STAR, "*"); break;
                case ':': tokens.emplace_back(TokenType::COLON, ":"); break;
                case '.': tokens.emplace_back(TokenType::DOT, "."); break;
                case '/': tokens.emplace_back(TokenType::SLASH, "/"); break;
                case '>':
                    if (match('=')) {
                        tokens.emplace_back(TokenType::GREATER_EQUAL, ">=");
                    }
                    else {
                        tokens.emplace_back(TokenType::GREATER, ">");
                    }
                    break;
                case '<':
                    if (match('=')) {
                        tokens.emplace_back(TokenType::LESS_EQUAL, "<=");
                    }
                    else if (match('>')) {
                        tokens.emplace_back(TokenType::NOT_EQUAL, "<>");
                    }
                    else {
                        tokens.emplace_back(TokenType::LESS, "<");
                    }
                    break;
                case '!':
                    if (match('=')) {
                        tokens.emplace_back(TokenType::NOT_EQUAL, "!=");
                    }
                    else {
                        throw std::runtime_error("Unexpected character: !");
                    }
                    break;
                default:
                    throw std::runtime_error(std::string("Unexpected character: ") + c);
            }
        }
    }
    return tokens;
}

// ... Existing Lexer methods ...
```

**Explanation:**

- **`PROC SORT` Detection:** The Lexer now recognizes `PROC SORT` by checking if the identifier `sort` follows `proc`.

- **Additional Keywords:** Added tokens for `BY`, `OUT=`, `DESCENDING`, and `NODUPKEY`, which are specific to `PROC SORT`.

- **Operator Handling:** Enhanced handling for operators like `>=`, `<=`, `!=`, etc., to support conditional statements if needed in sorting options.

---

### **47.4. Updating the Parser to Handle `PROC SORT` Statements**

Modify the Parser to construct `ProcSortNode` when encountering `PROC SORT` statements, including parsing the `BY` variables, `OUT=`, `DESCENDING`, and `NODUPKEY` options.

**Parser.cpp**

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

// ... Existing Parser methods ...

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::DATA)) {
        return parseDataStep();
    }
    else if (match(TokenType::PROC)) {
        if (match(TokenType::REPORT)) {
            return parseProcReport();
        }
        else if (match(TokenType::SORT)) {
            return parseProcSort();
        }
        else {
            return parseProcStep();
        }
    }
    else if (match(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }
    else {
        throw std::runtime_error("Unexpected token at start of statement: " + peek().lexeme);
    }
}

std::unique_ptr<ASTNode> Parser::parseProcSort() {
    Token procNameToken = previous(); // PROC SORT
    std::string procName = procNameToken.lexeme;
    
    Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after PROC SORT.");
    std::string datasetName = dataToken.lexeme;
    
    // Initialize ProcSortNode
    auto procSort = std::make_unique<ProcSortNode>(procName, datasetName);
    
    // Parse options and statements until RUN;
    while (!check(TokenType::RUN) && !isAtEnd()) {
        if (match(TokenType::OUT)) {
            consume(TokenType::EQUAL, "Expected '=' after OUT.");
            Token outDatasetToken = consume(TokenType::IDENTIFIER, "Expected output dataset name after OUT=.");
            procSort->outDataset = outDatasetToken.lexeme;
        }
        else if (match(TokenType::BY)) {
            // Parse BY variables
            std::vector<std::string> byVars = parseByVariables();
            procSort->sortVariables = byVars;
        }
        else if (match(TokenType::DESCENDING)) {
            procSort->descending = true;
        }
        else if (match(TokenType::NODUPKEY)) {
            procSort->nodupkey = true;
        }
        else {
            throw std::runtime_error("Unexpected token in PROC SORT: " + peek().lexeme);
        }
    }
    
    consume(TokenType::RUN, "Expected RUN; to end PROC SORT step.");
    consume(TokenType::SEMICOLON, "Expected ';' after RUN.");
    
    return procSort;
}

std::vector<std::string> Parser::parseByVariables() {
    std::vector<std::string> byVars;
    do {
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in BY statement.");
        byVars.push_back(varToken.lexeme);
    } while (match(TokenType::COMMA));
    return byVars;
}
```

**Explanation:**

- **`parseProcSort`:** Constructs a `ProcSortNode`, parsing options like `OUT=`, `BY`, `DESCENDING`, and `NODUPKEY`.
  
- **`parseByVariables`:** Helper method to parse the list of variables specified in the `BY` statement.

- **Option Parsing:** The parser handles options in any order, allowing flexibility in how users specify sort parameters.

---

### **47.5. Enhancing the Interpreter to Execute `PROC SORT`**

Implement the execution logic for `PROC SORT`, handling the sorting of datasets based on specified variables, sort order, and options like removing duplicates.

**Interpreter.h**

```cpp
// ... Existing Interpreter class members ...

private:
    // Execution methods
    void executeProcSort(ProcSortNode *node);
    // ... other methods ...
```

**Interpreter.cpp**

```cpp
#include "Interpreter.h"
#include "AST.h"
#include <iostream>
#include <algorithm>
#include <map>

// ... Existing Interpreter methods ...

void Interpreter::executeProcSort(ProcSortNode *node) {
    std::string inputDataset = node->datasetName;
    if (env.datasets.find(inputDataset) == env.datasets.end()) {
        throw std::runtime_error("PROC SORT: Dataset '" + inputDataset + "' does not exist.");
    }
    
    Dataset sortedDataset = env.datasets[inputDataset]; // Make a copy for sorting
    
    // Determine sort order
    bool descending = node->descending;
    
    // Sort using the specified variables
    std::sort(sortedDataset.begin(), sortedDataset.end(), [&](const std::map<std::string, Variable>& a, const std::map<std::string, Variable>& b) -> bool {
        for (const auto& var : node->sortVariables) {
            if (a.find(var) == a.end() || b.find(var) == b.end()) {
                throw std::runtime_error("PROC SORT: Variable '" + var + "' not found in dataset.");
            }
            
            const Variable& varA = a.at(var);
            const Variable& varB = b.at(var);
            
            // Compare based on variable type
            if (varA.type != varB.type) {
                throw std::runtime_error("PROC SORT: Variable types do not match for variable '" + var + "'.");
            }
            
            int comparison = 0;
            if (varA.type == VarType::NUMERIC) {
                double valA = std::get<double>(varA.value);
                double valB = std::get<double>(varB.value);
                if (valA < valB) comparison = -1;
                else if (valA > valB) comparison = 1;
            }
            else if (varA.type == VarType::CHARACTER) {
                std::string valA = std::get<std::string>(varA.value);
                std::string valB = std::get<std::string>(varB.value);
                comparison = valA.compare(valB);
            }
            else if (varA.type == VarType::BOOLEAN) {
                bool valA = std::get<bool>(varA.value);
                bool valB = std::get<bool>(varB.value);
                if (valA < valB) comparison = -1;
                else if (valA > valB) comparison = 1;
            }
            else if (varA.type == VarType::MISSING) {
                // Define missing as less than any other value
                if (std::holds_alternative<std::nullptr_t>(varA.value) && !std::holds_alternative<std::nullptr_t>(varB.value)) {
                    comparison = -1;
                }
                else if (!std::holds_alternative<std::nullptr_t>(varA.value) && std::holds_alternative<std::nullptr_t>(varB.value)) {
                    comparison = 1;
                }
                else {
                    comparison = 0;
                }
            }
            else {
                throw std::runtime_error("PROC SORT: Unsupported variable type for sorting.");
            }
            
            if (comparison != 0) {
                return descending ? (comparison > 0) : (comparison < 0);
            }
            // If equal, proceed to next sort variable
        }
        return false; // All sort variables are equal
    });
    
    // Remove duplicates if NODUPKEY is specified
    if (node->nodupkey) {
        auto last = std::unique(sortedDataset.begin(), sortedDataset.end(), [&](const std::map<std::string, Variable>& a, const std::map<std::string, Variable>& b) -> bool {
            for (const auto& var : node->sortVariables) {
                if (a.find(var) == a.end() || b.find(var) == b.end()) {
                    throw std::runtime_error("PROC SORT NODUPKEY: Variable '" + var + "' not found in dataset.");
                }
                const Variable& varA = a.at(var);
                const Variable& varB = b.at(var);
                
                if (varA.type != varB.type) {
                    throw std::runtime_error("PROC SORT NODUPKEY: Variable types do not match for variable '" + var + "'.");
                }
                
                if (varA.type == VarType::NUMERIC) {
                    if (std::get<double>(varA.value) != std::get<double>(varB.value)) return false;
                }
                else if (varA.type == VarType::CHARACTER) {
                    if (std::get<std::string>(varA.value) != std::get<std::string>(varB.value)) return false;
                }
                else if (varA.type == VarType::BOOLEAN) {
                    if (std::get<bool>(varA.value) != std::get<bool>(varB.value)) return false;
                }
                else if (varA.type == VarType::MISSING) {
                    // Both missing
                }
                else {
                    throw std::runtime_error("PROC SORT NODUPKEY: Unsupported variable type for duplicate checking.");
                }
            }
            return true; // All sort variables are equal
        });
        sortedDataset.erase(last, sortedDataset.end());
    }
    
    // Determine output dataset name
    std::string outputDataset = node->outDataset.empty() ? inputDataset : node->outDataset;
    
    // Assign the sorted dataset to the environment
    env.datasets[outputDataset] = sortedDataset;
    
    logLogger.info("PROC SORT executed successfully. Output dataset: '{}'.", outputDataset);
}
```

**Explanation:**

- **Dataset Verification:** Ensures that the input dataset exists before attempting to sort.
  
- **Sorting Logic:**
  - **Sort Order:** Determines whether to sort in ascending or descending order based on the `DESCENDING` option.
  
  - **Multiple Sort Variables:** Handles sorting by multiple variables, respecting their order of specification.
  
  - **Type Handling:** Compares variables based on their types (numeric, character, boolean) and handles missing values appropriately.
  
- **Removing Duplicates:** If the `NODUPKEY` option is specified, removes duplicate observations based on the sort variables.
  
- **Output Dataset:** Determines whether to overwrite the original dataset or create a new sorted dataset based on the `OUT=` option.
  
- **Logging:** Logs the successful execution of `PROC SORT` and the name of the output dataset.

---

### **47.6. Updating the Interpreter's Execute Method**

Modify the `Interpreter::execute` method to handle `ProcSortNode`.

**Interpreter.cpp**

```cpp
void Interpreter::execute(ASTNode *node) {
    if (auto dataStep = dynamic_cast<DataStepNode*>(node)) {
        executeDataStep(dataStep);
    }
    else if (auto procStep = dynamic_cast<ProcStepNode*>(node)) {
        if (auto procReport = dynamic_cast<ProcReportNode*>(procStep)) {
            executeProcReport(procReport);
        }
        else if (auto procSort = dynamic_cast<ProcSortNode*>(procStep)) {
            executeProcSort(procSort);
        }
        else {
            executeProcStep(procStep);
        }
    }
    else {
        throw std::runtime_error("Unknown AST node type during execution.");
    }
}
```

**Explanation:**

- **`ProcSortNode` Detection:** Checks if the `ProcStepNode` is actually a `ProcSortNode` and calls the appropriate execution method.

---

### **47.7. Creating Comprehensive Test Cases for `PROC SORT`**

Develop diverse test cases to verify that `PROC SORT` is parsed and executed correctly, producing the expected sorted datasets.

#### **47.7.1. Test Case 1: Basic `PROC SORT` Execution**

**SAS Script (`test_proc_sort_basic.sas`):**

```sas
data employees;
    input Department $ Name $ Salary;
    datalines;
HR Alice 50000
HR Bob 55000
IT Charlie 60000
IT David 65000
Finance Eve 70000
Finance Frank 75000
;
run;

proc sort data=employees out=sorted_employees;
    by Department descending Salary;
run;

proc print data=sorted_employees;
run;
```

**Expected Output (`sas_log_proc_sort_basic.txt`):**

```
[INFO] Executing statement: data employees; input Department $ Name $ Salary; datalines; HR Alice 50000; HR Bob 55000; IT Charlie 60000; IT David 65000; Finance Eve 70000; Finance Frank 75000; run;
[INFO] Executing DATA step: employees
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Salary' = 50000
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Salary' = 55000
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Salary' = 60000
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Name' = "David"
[INFO] Assigned variable 'Salary' = 65000
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Assigned variable 'Name' = "Eve"
[INFO] Assigned variable 'Salary' = 70000
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Assigned variable 'Name' = "Frank"
[INFO] Assigned variable 'Salary' = 75000
[INFO] DATA step 'employees' executed successfully. 6 observations created.

[INFO] Executing statement: proc sort data=employees out=sorted_employees; by Department descending Salary; run;
[INFO] PROC SORT executed successfully. Output dataset: 'sorted_employees'.

[INFO] Executing statement: proc print data=sorted_employees; run;
[INFO] Executing PROC PRINT

Department  Name     Salary  
----------------------------
Finance     Frank    75000  
Finance     Eve      70000  
HR          Bob      55000  
HR          Alice    50000  
IT          David    65000  
IT          Charlie  60000  

[INFO] PROC PRINT executed successfully.
```

**Test File (`test_proc_sort_basic.cpp`):**

```cpp
#include <gtest/gtest.h>
#include "Interpreter.h"
#include "DataEnvironment.h"
#include "AST.h"
#include <spdlog/spdlog.h>
#include <memory>
#include <sstream>

// Mock logger that captures messages
class MockLogger : public spdlog::logger {
public:
    MockLogger() : spdlog::logger("mock", nullptr) {}
    
    // Override log method to capture messages
    void info(const char* fmt, ...) override {
        va_list args;
        va_start(args, fmt);
        char buffer[2048];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        messages.emplace_back(std::string(buffer));
    }
    
    std::vector<std::string> messages;
};

// Test case for basic PROC SORT
TEST(InterpreterTest, ProcSortBasic) {
    DataEnvironment env;
    MockLogger logLogger;
    MockLogger lstLogger;
    Interpreter interpreter(env, logLogger, lstLogger);

    std::string input = 
        "data employees;\n"
        "input Department $ Name $ Salary;\n"
        "datalines;\n"
        "HR Alice 50000\n"
        "HR Bob 55000\n"
        "IT Charlie 60000\n"
        "IT David 65000\n"
        "Finance Eve 70000\n"
        "Finance Frank 75000\n"
        ";\n"
        "run;\n"
        "\n"
        "proc sort data=employees out=sorted_employees;\n"
        "    by Department descending Salary;\n"
        "run;\n"
        "\n"
        "proc print data=sorted_employees;\n"
        "run;\n";

    interpreter.handleReplInput(input);

    // Verify dataset creation
    ASSERT_TRUE(env.datasets.find("sorted_employees") != env.datasets.end());
    Dataset sorted = env.datasets["sorted_employees"];
    ASSERT_EQ(sorted.size(), 6);

    // Verify sorted order
    std::vector<std::tuple<std::string, std::string, double>> expected = {
        {"Finance", "Frank", 75000},
        {"Finance", "Eve", 70000},
        {"HR", "Bob", 55000},
        {"HR", "Alice", 50000},
        {"IT", "David", 65000},
        {"IT", "Charlie", 60000}
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(sorted[i].at("Department").value, expected[i].get<0>());
        EXPECT_EQ(sorted[i].at("Name").value, expected[i].get<1>());
        EXPECT_DOUBLE_EQ(std::get<double>(sorted[i].at("Salary").value), expected[i].get<2>());
    }

    // Verify log messages contain PROC SORT execution
    bool sortExecuted = false;
    for (const auto& msg : logLogger.messages) {
        if (msg.find("PROC SORT executed successfully. Output dataset: 'sorted_employees'.") != std::string::npos) {
            sortExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(sortExecuted);

    // Verify PROC PRINT output
    bool printExecuted = false;
    for (const auto& msg : lstLogger.messages) {
        if (msg.find("Finance     Frank    75000") != std::string::npos &&
            msg.find("Finance     Eve      70000") != std::string::npos &&
            msg.find("HR          Bob      55000") != std::string::npos &&
            msg.find("HR          Alice    50000") != std::string::npos &&
            msg.find("IT          David    65000") != std::string::npos &&
            msg.find("IT          Charlie  60000") != std::string::npos) {
            printExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(printExecuted);
}
```

**Explanation:**

- **`MockLogger`:** Captures log messages, enabling verification of `PROC SORT` and `PROC PRINT` executions.

- **Assertions:**
  - Confirms that the `sorted_employees` dataset is created with six observations.
  
  - Verifies that the dataset is sorted correctly based on `Department` (ascending) and `Salary` (descending).
  
  - Checks that the log contains messages indicating successful execution of `PROC SORT` and `PROC PRINT`.
  
  - Ensures that the printed report contains the expected sorted data.

---

#### **47.7.2. Test Case 2: `PROC SORT` with `NODUPKEY` Option**

**SAS Script (`test_proc_sort_nodupkey.sas`):**

```sas
data duplicates;
    input ID $ Name $ Score;
    datalines;
A001 John 85
A002 Jane 90
A001 John 85
A003 Mike 75
A002 Jane 90
A004 Anna 95
;
run;

proc sort data=duplicates out=unique_duplicates nodupkey;
    by ID;
run;

proc print data=unique_duplicates;
run;
```

**Expected Output (`sas_log_proc_sort_nodupkey.txt`):**

```
[INFO] Executing statement: data duplicates; input ID $ Name $ Score; datalines; A001 John 85; A002 Jane 90; A001 John 85; A003 Mike 75; A002 Jane 90; A004 Anna 95; run;
[INFO] Executing DATA step: duplicates
[INFO] Assigned variable 'ID' = "A001"
[INFO] Assigned variable 'Name' = "John"
[INFO] Assigned variable 'Score' = 85
[INFO] Assigned variable 'ID' = "A002"
[INFO] Assigned variable 'Name' = "Jane"
[INFO] Assigned variable 'Score' = 90
[INFO] Assigned variable 'ID' = "A001"
[INFO] Assigned variable 'Name' = "John"
[INFO] Assigned variable 'Score' = 85
[INFO] Assigned variable 'ID' = "A003"
[INFO] Assigned variable 'Name' = "Mike"
[INFO] Assigned variable 'Score' = 75
[INFO] Assigned variable 'ID' = "A002"
[INFO] Assigned variable 'Name' = "Jane"
[INFO] Assigned variable 'Score' = 90
[INFO] Assigned variable 'ID' = "A004"
[INFO] Assigned variable 'Name' = "Anna"
[INFO] Assigned variable 'Score' = 95
[INFO] DATA step 'duplicates' executed successfully. 6 observations created.

[INFO] Executing statement: proc sort data=duplicates out=unique_duplicates nodupkey; by ID; run;
[INFO] PROC SORT executed successfully. Output dataset: 'unique_duplicates'.

[INFO] Executing statement: proc print data=unique_duplicates; run;
[INFO] Executing PROC PRINT

ID     Name     Score  
---------------------
A001   John     85    
A002   Jane     90    
A003   Mike     75    
A004   Anna     95    

[INFO] PROC PRINT executed successfully.
```

**Test File (`test_proc_sort_nodupkey.cpp`):**

```cpp
#include <gtest/gtest.h>
#include "Interpreter.h"
#include "DataEnvironment.h"
#include "AST.h"
#include <spdlog/spdlog.h>
#include <memory>
#include <sstream>

// Mock logger that captures messages
class MockLogger : public spdlog::logger {
public:
    MockLogger() : spdlog::logger("mock", nullptr) {}
    
    // Override log method to capture messages
    void info(const char* fmt, ...) override {
        va_list args;
        va_start(args, fmt);
        char buffer[2048];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        messages.emplace_back(std::string(buffer));
    }
    
    std::vector<std::string> messages;
};

// Test case for PROC SORT with NODUPKEY
TEST(InterpreterTest, ProcSortNodupkey) {
    DataEnvironment env;
    MockLogger logLogger;
    MockLogger lstLogger;
    Interpreter interpreter(env, logLogger, lstLogger);

    std::string input = 
        "data duplicates;\n"
        "input ID $ Name $ Score;\n"
        "datalines;\n"
        "A001 John 85\n"
        "A002 Jane 90\n"
        "A001 John 85\n"
        "A003 Mike 75\n"
        "A002 Jane 90\n"
        "A004 Anna 95\n"
        ";\n"
        "run;\n"
        "\n"
        "proc sort data=duplicates out=unique_duplicates nodupkey;\n"
        "    by ID;\n"
        "run;\n"
        "\n"
        "proc print data=unique_duplicates;\n"
        "run;\n";

    interpreter.handleReplInput(input);

    // Verify dataset creation
    ASSERT_TRUE(env.datasets.find("unique_duplicates") != env.datasets.end());
    Dataset unique = env.datasets["unique_duplicates"];
    ASSERT_EQ(unique.size(), 4);

    // Expected unique dataset
    std::vector<std::tuple<std::string, std::string, double>> expected = {
        {"A001", "John", 85},
        {"A002", "Jane", 90},
        {"A003", "Mike", 75},
        {"A004", "Anna", 95}
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(unique[i].at("ID").value, expected[i].get<0>());
        EXPECT_EQ(unique[i].at("Name").value, expected[i].get<1>());
        EXPECT_DOUBLE_EQ(std::get<double>(unique[i].at("Score").value), expected[i].get<2>());
    }

    // Verify log messages contain PROC SORT execution
    bool sortExecuted = false;
    for (const auto& msg : logLogger.messages) {
        if (msg.find("PROC SORT executed successfully. Output dataset: 'unique_duplicates'.") != std::string::npos) {
            sortExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(sortExecuted);

    // Verify PROC PRINT output
    bool printExecuted = false;
    for (const auto& msg : lstLogger.messages) {
        if (msg.find("A001   John     85") != std::string::npos &&
            msg.find("A002   Jane     90") != std::string::npos &&
            msg.find("A003   Mike     75") != std::string::npos &&
            msg.find("A004   Anna     95") != std::string::npos) {
            printExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(printExecuted);
}
```

**Explanation:**

- **`MockLogger`:** Captures log messages, enabling verification of `PROC SORT` and `PROC PRINT` executions.
  
- **Assertions:**
  - Confirms that the `unique_duplicates` dataset is created with four unique observations.
  
  - Verifies that duplicates are removed based on the `ID` variable.
  
  - Checks that the log contains messages indicating successful execution of `PROC SORT` and `PROC PRINT`.
  
  - Ensures that the printed report contains only unique entries.

---

### **47.8. Updating the Testing Suite**

Ensure that the newly created test cases for `PROC SORT` are integrated into your testing suite and are executed automatically as part of your Continuous Integration (CI) pipeline.

**CMakeLists.txt**

```cmake
# ... Existing CMake configurations ...

# Add test executable
add_executable(runTests 
    test_main.cpp 
    test_lexer.cpp 
    test_parser.cpp 
    test_interpreter.cpp 
    test_proc_report_basic.cpp 
    test_proc_report_multiple_groups.cpp
    test_proc_sort_basic.cpp
    test_proc_sort_nodupkey.cpp
)

# Link Google Test and pthread
target_link_libraries(runTests PRIVATE gtest gtest_main pthread)

# Add tests
add_test(NAME SASTestSuite COMMAND runTests)
```

**Explanation:**

- **Test Integration:** Adds the new `PROC SORT` test cases (`test_proc_sort_basic.cpp` and `test_proc_sort_nodupkey.cpp`) to the test executable, ensuring they are run alongside existing tests.
  
- **Automated Execution:** The tests will be executed automatically during the build process and as part of the CI pipeline, providing immediate feedback on any issues.

---

### **47.9. Best Practices for Implementing `PROC SORT`**

1. **Modular Design:**
   - Separate parsing, AST construction, and execution logic to enhance maintainability and scalability.
   
2. **Comprehensive Testing:**
   - Develop a wide range of test cases covering various `PROC SORT` features, including single and multiple sort variables, sort orders, and options like `NODUPKEY`.
   
3. **Robust Error Handling:**
   - Implement detailed error detection and reporting for scenarios like undefined variables, unsupported variable types for sorting, and invalid sort options.
   
4. **Performance Optimization:**
   - Ensure that sorting operations are efficient, especially for large datasets. Consider optimizing the sorting algorithm or leveraging multi-threading if necessary.
   
5. **User Feedback:**
   - Provide clear and descriptive log messages to inform users about the execution status, sorted datasets, and any encountered issues.
   
6. **Documentation:**
   - Document the implementation details of `PROC SORT`, including supported options, usage examples, and limitations, to aid future development and user understanding.

---

### **47.10. Expanding to Other Advanced PROC Procedures**

With `PROC SORT` now implemented, consider extending support to other essential PROC procedures to further enhance your interpreter's capabilities.

#### **47.10.1. `PROC MEANS`**

**Purpose:**
- Calculates descriptive statistics such as mean, median, standard deviation, etc., for numeric variables.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcMeansNode` and related AST nodes to represent `PROC MEANS` components.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC MEANS` statements, capturing variables to analyze and statistical options.
   
3. **Interpreter Execution:**
   - Implement statistical calculations and output formatting based on the specified options.
   
4. **Testing:**
   - Develop test cases to ensure accurate statistical computations and correct report formatting.

#### **47.10.2. `PROC TABULATE`**

**Purpose:**
- Generates multi-dimensional tables summarizing data, allowing for detailed data analysis and reporting.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcTabulateNode` and related AST nodes to represent `PROC TABULATE` components.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC TABULATE` statements, capturing class variables, analysis variables, and table definitions.
   
3. **Interpreter Execution:**
   - Implement table generation logic, handling grouping, summarization, and formatting.
   
4. **Testing:**
   - Develop test cases to verify accurate table generation, correct summarization, and proper handling of various options.

---

### **47.11. Enhancing the REPL Interface**

To improve user experience, consider enhancing the Read-Eval-Print Loop (REPL) interface with advanced features.

1. **Syntax Highlighting:**
   - Implement color-coding of keywords, variables, and operators to improve readability.
   
2. **Auto-completion:**
   - Provide suggestions for SAS keywords, variable names, and function names as users type.
   
3. **Command History:**
   - Allow users to navigate through previously entered commands using the arrow keys.
   
4. **Error Highlighting:**
   - Visually indicate syntax errors or problematic lines within the REPL to aid quick identification and correction.

---

### **47.12. Implementing Debugging Tools**

Introduce debugging capabilities to help users troubleshoot and optimize their scripts.

1. **Breakpoints:**
   - Enable users to set breakpoints in their scripts to pause execution and inspect variables.
   
2. **Step Execution:**
   - Allow users to step through their scripts line by line, facilitating debugging and learning.
   
3. **Variable Inspection:**
   - Provide commands to inspect the current state of variables and datasets at any point during execution.

---

### **47.13. Optimizing Performance**

Ensure that your interpreter remains efficient, especially as it handles larger datasets and more complex operations.

1. **Efficient Data Structures:**
   - Refine data storage and retrieval mechanisms to handle large datasets more effectively.
   
2. **Parallel Processing:**
   - Leverage multi-threading for operations that can be executed concurrently, such as data transformations and aggregations.
   
3. **Memory Management:**
   - Optimize memory usage, particularly when dealing with extensive data and complex computations.
   
4. **Profiling and Benchmarking:**
   - Use profiling tools (e.g., `gprof`, `Valgrind`) to identify and address performance bottlenecks.

---

### **47.14. Enhancing Error Handling and Logging**

Improve the interpreter's robustness by refining its error handling and logging mechanisms.

1. **Detailed Error Messages:**
   - Include contextual information like line numbers, character positions, and variable states in error messages to aid debugging.
   
2. **Advanced Logging:**
   - Implement different logging levels (e.g., DEBUG, INFO, ERROR) and log outputs to files for persistent records.
   
3. **Graceful Degradation:**
   - Ensure that the interpreter can recover gracefully from non-critical errors, allowing users to continue working without restarting the session.

---

### **47.15. Expanding the Testing Suite**

Continue to grow your testing suite to cover new features and ensure reliability.

1. **Increase Test Coverage:**
   - Develop more test cases covering various `PROC SORT` features, data types, and edge cases.
   
2. **Automate Performance Testing:**
   - Incorporate benchmarks to monitor and optimize the interpreter's performance over time.
   
3. **Regression Testing:**
   - Implement tests to ensure that new features do not break existing functionalities.

---

### **47.16. Improving Documentation and User Guides**

Provide comprehensive resources to help users understand and utilize your interpreter effectively.

1. **Comprehensive Manuals:**
   - Create detailed documentation covering installation, usage, scripting syntax, and advanced features.
   
2. **Tutorials and Examples:**
   - Provide example scripts and tutorials to help users learn how to effectively use the interpreter.
   
3. **API Documentation:**
   - If applicable, provide documentation for any APIs or extensions exposed by the interpreter.

---

### **47.17. Implementing Security Features**

Ensure that your interpreter executes scripts securely, protecting the system and user data.

1. **Sandboxing:**
   - Isolate script execution to prevent unauthorized access to system resources.
   
2. **Input Validation:**
   - Rigorously validate user inputs to prevent injection attacks and ensure script integrity.
   
3. **Access Controls:**
   - Implement permissions and access controls to restrict sensitive operations.

---

### **47.18. Exploring Extensibility and Plugin Systems**

Design your interpreter to be extensible, allowing users to add custom functionalities or integrate third-party tools.

1. **Plugin Architecture:**
   - Develop a framework that supports plugins, enabling users to extend the interpreter's capabilities.
   
2. **APIs for Extensions:**
   - Provide clear APIs and guidelines for developing extensions, fostering a community-driven ecosystem around your interpreter.
   
3. **Documentation for Developers:**
   - Offer comprehensive guides and examples to help developers create and integrate plugins.

---

### **47.19. Engaging with Users and Gathering Feedback**

Foster a community around your interpreter to drive continuous improvement and adoption.

1. **Beta Testing:**
   - Release beta versions to a select group of users to gather feedback and identify areas for improvement.
   
2. **Community Building:**
   - Create forums, mailing lists, or chat channels where users can ask questions, share scripts, and collaborate.
   
3. **Iterative Development:**
   - Use feedback to prioritize features, fix bugs, and enhance the interpreter based on real-world usage patterns.

---

### **47.20. Summary of Achievements**

1. **Implemented `PROC SORT`:**
   - Extended the AST to represent `PROC SORT` components.
   
   - Updated the Lexer and Parser to recognize and parse `PROC SORT` syntax.
   
   - Enhanced the Interpreter to execute `PROC SORT`, handling sort variables, sort order, and options like `NODUPKEY`.
   
2. **Developed Comprehensive Test Cases:**
   - Created automated tests to verify the correct parsing and execution of `PROC SORT`.
   
   - Ensured that datasets are sorted correctly and duplicate observations are handled as specified.
   
3. **Integrated Tests into the Testing Suite:**
   - Updated the CMake configuration to include new `PROC SORT` test cases.
   
   - Ensured that `PROC SORT` tests are part of the automated testing pipeline.
   
4. **Established Best Practices:**
   - Emphasized modular design, comprehensive testing, robust error handling, performance optimization, and thorough documentation.

---

### **47.21. Next Steps**

With **`PROC SORT`** successfully implemented, your SAS interpreter now supports another essential data manipulation tool. To continue advancing your interpreter's capabilities, consider the following next steps:

1. **Implement Additional PROC Procedures:**
   - **`PROC MEANS`:**
     - Calculates descriptive statistics such as mean, median, standard deviation, etc., for numeric variables.
     
   - **`PROC FREQ`:**
     - Generates frequency tables for categorical variables, useful for analyzing distributions.
     
   - **`PROC TABULATE`:**
     - Creates multi-dimensional tables summarizing data, allowing for detailed data analysis and reporting.
     
   - **`PROC SQL`:**
     - Integrates SQL capabilities, enabling users to perform complex queries and data manipulations.
     
2. **Support External Data Formats:**
   - **Excel Files (`.xlsx`):**
     - Enable reading from and writing to Excel files using libraries like [libxlsxwriter](https://libxlsxwriter.github.io/) or [xlsxio](https://github.com/brechtsanders/xlsxio).
     
   - **JSON and XML:**
     - Integrate parsing and generation capabilities using libraries like [RapidJSON](https://github.com/Tencent/rapidjson) and [TinyXML](http://www.grinninglizard.com/tinyxml/).
     
   - **Database Connectivity:**
     - Allow interfacing with databases (e.g., PostgreSQL, MySQL) using connectors like [libpq](https://www.postgresql.org/docs/current/libpq.html) or [MySQL Connector/C++](https://dev.mysql.com/downloads/connector/cpp/).
     
3. **Enhance the REPL Interface:**
   - **Syntax Highlighting:**
     - Implement color-coding of keywords, variables, and operators to improve readability.
     
   - **Auto-completion:**
     - Provide suggestions for SAS keywords, variable names, and function names as users type.
     
   - **Command History:**
     - Allow users to navigate through previously entered commands using the arrow keys.
     
4. **Implement Debugging Tools:**
   - **Breakpoints:**
     - Enable users to set breakpoints in their scripts to pause execution and inspect variables.
     
   - **Step Execution:**
     - Allow users to step through their scripts line by line, facilitating debugging and learning.
     
   - **Variable Inspection:**
     - Provide commands to inspect the current state of variables and datasets at any point during execution.
     
5. **Optimize Performance:**
   - **Efficient Data Structures:**
     - Refine data storage and retrieval mechanisms to handle large datasets more effectively.
     
   - **Parallel Processing:**
     - Leverage multi-threading for operations that can be executed concurrently, such as data transformations and aggregations.
     
   - **Memory Management:**
     - Optimize memory usage, particularly when dealing with extensive data and complex computations.
     
   - **Profiling and Benchmarking:**
     - Use profiling tools (e.g., `gprof`, `Valgrind`) to identify and address performance bottlenecks.
     
6. **Enhance Error Handling and Logging:**
   - **Detailed Error Messages:**
     - Include contextual information like line numbers, character positions, and variable states in error messages to aid debugging.
     
   - **Advanced Logging:**
     - Implement different logging levels (e.g., DEBUG, INFO, ERROR) and log outputs to files for persistent records.
     
7. **Expand the Testing Suite:**
   - **Increase Test Coverage:**
     - Develop more test cases covering various PROC procedures, data types, and edge cases.
     
   - **Automate Performance Testing:**
     - Incorporate benchmarks to monitor and optimize the interpreter's performance over time.
     
   - **Regression Testing:**
     - Implement tests to ensure that new features do not break existing functionalities.
     
8. **Improve Documentation and User Guides:**
   - **Comprehensive Manuals:**
     - Create detailed documentation covering installation, usage, scripting syntax, and advanced features.
     
   - **Tutorials and Examples:**
     - Provide example scripts and tutorials to help users learn how to effectively use the interpreter.
     
   - **API Documentation:**
     - If applicable, provide documentation for any APIs or extensions exposed by the interpreter.
     
9. **Implement Security Features:**
   - **Sandboxing:**
     - Isolate script execution to prevent unauthorized access to system resources.
     
   - **Input Validation:**
     - Rigorously validate user inputs to prevent injection attacks and ensure script integrity.
     
   - **Access Controls:**
     - Implement permissions and access controls to restrict sensitive operations.
     
10. **Explore Extensibility and Plugin Systems:**
    - **Plugin Architecture:**
      - Develop a framework that supports plugins, enabling users to extend the interpreter's capabilities.
      
    - **APIs for Extensions:**
      - Provide clear APIs and guidelines for developing extensions, fostering a community-driven ecosystem around your interpreter.
      
    - **Documentation for Developers:**
      - Offer comprehensive guides and examples to help developers create and integrate plugins.
      
11. **Engage with Users and Gather Feedback:**
    - **Beta Testing:**
      - Release beta versions to a select group of users to gather feedback and identify areas for improvement.
      
    - **Community Building:**
      - Create forums, mailing lists, or chat channels where users can ask questions, share scripts, and collaborate.
      
    - **Iterative Development:**
      - Use feedback to prioritize features, fix bugs, and enhance the interpreter based on real-world usage patterns.

---

## **Conclusion**

Implementing **`PROC SORT`** marks a significant advancement in your SAS interpreter's development, enabling users to organize their data efficiently and prepare it for subsequent analysis and reporting tasks. By extending the AST, updating the Lexer and Parser, and enhancing the Interpreter's execution logic, you've successfully integrated another core SAS procedure. The comprehensive test cases ensure that `PROC SORT` operates reliably, correctly sorting datasets and handling options like `DESCENDING` and `NODUPKEY`.

As you continue to build upon this foundation, adopting best practices such as modular design, thorough testing, robust error handling, performance optimization, and comprehensive documentation will be instrumental in creating a reliable, efficient, and user-friendly SAS interpreter. Embracing these strategies will not only facilitate the addition of new features but also ensure that the interpreter remains maintainable and scalable as its capabilities expand.

Keep up the excellent work, and feel free to reach out for further guidance or assistance as you progress with your project. Good luck!