Certainly! Building upon the successful implementation of **`PROC SORT`**, the next strategic step is to **Implement `PROC MEANS`**. This procedure is fundamental in data analysis, enabling users to calculate descriptive statistics such as mean, median, standard deviation, and more for numeric variables. By integrating `PROC MEANS`, your SAS interpreter will provide essential statistical analysis capabilities, further aligning it with the comprehensive feature set expected from SAS environments.

## **Next Strategic Step: Implementing `PROC MEANS`**

**`PROC MEANS`** is a powerful procedure in SAS that computes descriptive statistics for numeric variables. It offers flexibility in specifying which statistics to calculate, allows for grouping by categorical variables, and supports various output options. Implementing `PROC MEANS` will empower users to perform essential data summarizations and gain insights into their datasets.

---

## **Step 48: Implementing `PROC MEANS`**

### **48.1. Understanding `PROC MEANS`**

**`PROC MEANS`** provides functionality to:

- **Calculate Descriptive Statistics:** Compute statistics like mean, median, standard deviation, minimum, maximum, etc.
- **Group Data:** Perform calculations within groups defined by one or more categorical variables.
- **Specify Statistics to Display:** Allow users to choose which statistics to include in the output.
- **Handle Missing Values:** Manage missing data appropriately during calculations.
- **Output Results to Datasets:** Save the computed statistics to new datasets for further analysis or reporting.

**Example SAS Script Using `PROC MEANS`:**

```sas
data sales;
    input Region $ Product $ Sales;
    datalines;
East Widget 100
East Gadget 150
West Widget 200
West Gadget 250
North Widget 300
North Gadget 350
South Widget 400
South Gadget 450
;
run;

proc means data=sales n mean std min max;
    class Region;
    var Sales;
    output out=stats_sales mean=Average_Sales std=Std_Dev_Sales min=Min_Sales max=Max_Sales;
run;

proc print data=stats_sales;
run;
```

**Expected Output:**

```
Statistics Sales Dataset

Region  _TYPE_  _FREQ_  Average_Sales  Std_Dev_Sales  Min_Sales  Max_Sales
---------------------------------------------------------------------------
East    1       2       125.00          35.36          100         150
West    1       2       225.00          35.36          200         250
North   1       2       325.00          35.36          300         350
South   1       2       425.00          35.36          400         450
Total   0       8       250.00          111.80         100         450
```

---

### **48.2. Extending the Abstract Syntax Tree (AST) for `PROC MEANS`**

To support `PROC MEANS`, you'll need to extend your AST to represent the components of the procedure, such as class variables, variables to analyze, statistics to calculate, and output options.

**AST.h**

```cpp
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <variant>

// ... Existing ASTNode classes ...

// PROC MEANS specific nodes
class ProcMeansNode : public ProcStepNode {
public:
    std::vector<std::string> classVariables; // Variables to group by
    std::vector<std::string> analysisVariables; // Numeric variables to analyze
    std::vector<std::string> statistics; // Statistics to calculate (e.g., n, mean, std)
    std::string outputDataset; // Output dataset name
    std::map<std::string, std::string> outputStatistics; // Mapping of output statistics
    
    ProcMeansNode(const std::string& name, const std::string& data)
        : ProcStepNode(name, data) {}
};

// Statistics option node
struct StatisticsOption {
    std::string statisticName; // e.g., "mean", "std"
    std::string alias;         // e.g., "Average_Sales"
};

// Extend StatisticsStatementNode if necessary
class StatisticsStatementNode : public ASTNode {
public:
    std::vector<StatisticsOption> options;
    
    StatisticsStatementNode(const std::vector<StatisticsOption>& opts)
        : options(opts) {}
};

#endif // AST_H
```

**Explanation:**

- **`ProcMeansNode`:** Inherits from `ProcStepNode` and includes members specific to `PROC MEANS`, such as class variables, analysis variables, statistics to calculate, and output dataset details.
  
- **`StatisticsOption`:** Represents a statistic option, capturing the name of the statistic and its alias in the output dataset.
  
- **`StatisticsStatementNode`:** Represents the `OUTPUT` statement within `PROC MEANS`, capturing the statistics to be calculated and their aliases.

---

### **48.3. Updating the Lexer to Recognize `PROC MEANS` Syntax**

Ensure that the Lexer can tokenize the syntax specific to `PROC MEANS`, such as the `CLASS` statement, `VAR` statement, `OUTPUT` statement, and statistics options.

**Lexer.cpp**

```cpp
// Extending the Lexer to handle PROC MEANS statements

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
            else if (identifier == "means") { // Specific PROC MEANS
                tokens.emplace_back(TokenType::PROC_MEANS, identifier);
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
            else if (identifier == "class") {
                tokens.emplace_back(TokenType::CLASS, identifier);
            }
            else if (identifier == "var") {
                tokens.emplace_back(TokenType::VAR, identifier);
            }
            else if (identifier == "output") {
                tokens.emplace_back(TokenType::OUTPUT, identifier);
            }
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

- **`PROC MEANS` Detection:** The Lexer now recognizes `PROC MEANS` by checking if the identifier `means` follows `proc`.
  
- **Additional Keywords:** Added tokens for `CLASS`, `VAR`, `OUTPUT`, and various statistical options to support the `PROC MEANS` syntax.
  
- **Operator Handling:** Enhanced handling for operators like `>=`, `<=`, `!=`, etc., to support conditional statements if needed in sorting options.

---

### **48.4. Updating the Parser to Handle `PROC MEANS` Statements**

Modify the Parser to construct `ProcMeansNode` when encountering `PROC MEANS` statements, including parsing the `CLASS` variables, `VAR` variables, statistics options, and `OUTPUT` statements.

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
        else if (match(TokenType::MEANS)) {
            return parseProcMeans();
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

std::unique_ptr<ASTNode> Parser::parseProcMeans() {
    Token procNameToken = previous(); // PROC MEANS
    std::string procName = procNameToken.lexeme;
    
    Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after PROC MEANS.");
    std::string datasetName = dataToken.lexeme;
    
    auto procMeans = std::make_unique<ProcMeansNode>(procName, datasetName);
    
    // Parse options and statements until RUN;
    while (!check(TokenType::RUN) && !isAtEnd()) {
        if (match(TokenType::CLASS)) {
            std::vector<std::string> classVars = parseClassVariables();
            procMeans->classVariables = classVars;
        }
        else if (match(TokenType::VAR)) {
            std::vector<std::string> varVars = parseVarVariables();
            procMeans->analysisVariables = varVars;
        }
        else if (match(TokenType::OUTPUT)) {
            parseOutputStatement(procMeans.get());
        }
        else {
            // Parse statistics list if present
            std::vector<std::string> stats = parseStatisticsList();
            procMeans->statistics = stats;
        }
    }
    
    consume(TokenType::RUN, "Expected RUN; to end PROC MEANS step.");
    consume(TokenType::SEMICOLON, "Expected ';' after RUN.");
    
    return procMeans;
}

std::vector<std::string> Parser::parseClassVariables() {
    std::vector<std::string> classVars;
    do {
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in CLASS statement.");
        classVars.push_back(varToken.lexeme);
    } while (match(TokenType::COMMA));
    return classVars;
}

std::vector<std::string> Parser::parseVarVariables() {
    std::vector<std::string> varVars;
    do {
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in VAR statement.");
        varVars.push_back(varToken.lexeme);
    } while (match(TokenType::COMMA));
    return varVars;
}

std::vector<std::string> Parser::parseStatisticsList() {
    std::vector<std::string> stats;
    while (match(TokenType::IDENTIFIER)) {
        Token statToken = previous();
        stats.push_back(statToken.lexeme);
    }
    return stats;
}

void Parser::parseOutputStatement(ProcMeansNode* procMeans) {
    consume(TokenType::OUT, "Expected '=' after OUT.");
    consume(TokenType::EQUAL, "Expected '=' after OUT.");
    Token outDatasetToken = consume(TokenType::IDENTIFIER, "Expected output dataset name after OUT=.");
    procMeans->outputDataset = outDatasetToken.lexeme;
    
    consume(TokenType::SEMICOLON, "Expected ';' after OUTPUT statement.");
    
    // Parse statistics options if any
    while (match(TokenType::IDENTIFIER)) {
        Token statToken = previous();
        std::string statName = statToken.lexeme;
        std::string alias = "";
        if (match(TokenType::EQUAL)) {
            Token aliasToken = consume(TokenType::IDENTIFIER, "Expected alias after '=' in OUTPUT statement.");
            alias = aliasToken.lexeme;
        }
        procMeans->outputStatistics[statName] = alias;
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after OUTPUT statement options.");
}
```

**Explanation:**

- **`parseProcMeans`:** Constructs a `ProcMeansNode`, parsing options like `CLASS`, `VAR`, `OUTPUT`, and statistics options.
  
- **`parseClassVariables` and `parseVarVariables`:** Helper methods to parse the list of variables specified in the `CLASS` and `VAR` statements, respectively.
  
- **`parseStatisticsList`:** Parses the list of statistics specified after `PROC MEANS` (e.g., `n`, `mean`, `std`).
  
- **`parseOutputStatement`:** Parses the `OUTPUT` statement, capturing the output dataset name and any specified statistics aliases.

---

### **48.5. Enhancing the Interpreter to Execute `PROC MEANS`**

Implement the execution logic for `PROC MEANS`, handling the calculation of specified statistics, grouping by class variables, and outputting results to new datasets.

**Interpreter.h**

```cpp
// ... Existing Interpreter.h content ...

private:
    // Execution methods
    void executeProcMeans(ProcMeansNode *node);
    // ... other methods ...
```

**Interpreter.cpp**

```cpp
#include "Interpreter.h"
#include "AST.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <numeric>
#include <cmath>

// ... Existing Interpreter methods ...

void Interpreter::executeProcMeans(ProcMeansNode *node) {
    std::string inputDataset = node->datasetName;
    if (env.datasets.find(inputDataset) == env.datasets.end()) {
        throw std::runtime_error("PROC MEANS: Dataset '" + inputDataset + "' does not exist.");
    }
    
    Dataset dataset = env.datasets[inputDataset];
    lstLogger.info("Executing PROC MEANS");
    
    // Grouping
    std::map<std::vector<std::string>, std::vector<const std::map<std::string, Variable>*>> groups;
    
    for (const auto& row : dataset) {
        std::vector<std::string> key;
        for (const auto& classVar : node->classVariables) {
            if (row.find(classVar) == row.end()) {
                throw std::runtime_error("PROC MEANS: Class variable '" + classVar + "' not found in dataset.");
            }
            key.push_back(toString(row.at(classVar).value));
        }
        groups[key].push_back(&row);
    }
    
    // Initialize output dataset
    std::string outputDataset = node->outputDataset.empty() ? "stats_" + inputDataset : node->outputDataset;
    Dataset statsDataset;
    
    // Calculate statistics for each group
    for (const auto& [groupKey, rows] : groups) {
        std::map<std::string, Variable> statsRow;
        
        // Add class variables to statsRow
        for (size_t i = 0; i < node->classVariables.size(); ++i) {
            statsRow[node->classVariables[i]] = Variable{VarType::CHARACTER, groupKey[i]};
        }
        
        // Calculate statistics for each analysis variable
        for (const auto& var : node->analysisVariables) {
            if (var.empty()) continue;
            // Collect all values for the variable
            std::vector<double> values;
            for (const auto* rowPtr : rows) {
                const Variable& varObj = rowPtr->at(var);
                if (varObj.type == VarType::NUMERIC) {
                    values.push_back(std::get<double>(varObj.value));
                }
                else {
                    throw std::runtime_error("PROC MEANS: Variable '" + var + "' is not numeric.");
                }
            }
            
            // Calculate statistics
            double n = static_cast<double>(values.size());
            double mean = std::accumulate(values.begin(), values.end(), 0.0) / n;
            
            double sum_sq_diff = 0.0;
            for (const auto& val : values) {
                sum_sq_diff += (val - mean) * (val - mean);
            }
            double std_dev = (n > 1) ? std::sqrt(sum_sq_diff / (n - 1)) : 0.0;
            
            double min_val = *std::min_element(values.begin(), values.end());
            double max_val = *std::max_element(values.begin(), values.end());
            
            // Assign to statsRow based on requested statistics
            for (const auto& stat : node->statistics) {
                if (stat == "n") {
                    statsRow["N"] = Variable{VarType::NUMERIC, n};
                }
                else if (stat == "mean") {
                    statsRow["Mean_" + var] = Variable{VarType::NUMERIC, mean};
                }
                else if (stat == "std") {
                    statsRow["Std_Dev_" + var] = Variable{VarType::NUMERIC, std_dev};
                }
                else if (stat == "min") {
                    statsRow["Min_" + var] = Variable{VarType::NUMERIC, min_val};
                }
                else if (stat == "max") {
                    statsRow["Max_" + var] = Variable{VarType::NUMERIC, max_val};
                }
                // Add more statistics as needed
            }
        }
        
        // Handle OUTPUT statement aliases
        if (!node->outputStatistics.empty()) {
            for (const auto& [statName, alias] : node->outputStatistics) {
                if (statsRow.find(statName) != statsRow.end()) {
                    statsRow[alias] = statsRow[statName];
                    statsRow.erase(statName);
                }
            }
        }
        
        statsDataset.push_back(statsRow);
    }
    
    // Assign the stats dataset to the environment
    env.datasets[outputDataset] = statsDataset;
    
    logLogger.info("PROC MEANS executed successfully. Output dataset: '{}'.", outputDataset);
}

```

**Explanation:**

- **Dataset Verification:** Ensures that the input dataset exists before proceeding.
  
- **Grouping Logic:** Groups the dataset based on the specified class variables. Each unique combination of class variable values defines a group.
  
- **Statistics Calculation:**
  
  - **N (Count):** Counts the number of observations in each group.
    
  - **Mean:** Calculates the average of the specified analysis variables within each group.
    
  - **Standard Deviation:** Computes the standard deviation of the analysis variables within each group.
    
  - **Min and Max:** Identifies the minimum and maximum values of the analysis variables within each group.
  
- **Handling `OUTPUT` Statement:**
  
  - Maps calculated statistics to specified aliases in the output dataset.
  
- **Output Dataset Creation:** Stores the computed statistics in a new dataset, either specified by the user (`OUT=` option) or automatically named by prefixing the input dataset name.
  
- **Logging:** Logs the successful execution of `PROC MEANS` and the name of the output dataset.

---

### **48.6. Updating the Interpreter's Execute Method**

Modify the `Interpreter::execute` method to handle `ProcMeansNode`.

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
        else if (auto procMeans = dynamic_cast<ProcMeansNode*>(procStep)) {
            executeProcMeans(procMeans);
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

- **`ProcMeansNode` Detection:** Checks if the `ProcStepNode` is actually a `ProcMeansNode` and calls the appropriate execution method.

---

### **48.7. Creating Comprehensive Test Cases for `PROC MEANS`**

Develop diverse test cases to verify that `PROC MEANS` is parsed and executed correctly, producing the expected statistical summaries.

#### **48.7.1. Test Case 1: Basic `PROC MEANS` Execution**

**SAS Script (`test_proc_means_basic.sas`):**

```sas
data sales;
    input Region $ Product $ Sales;
    datalines;
East Widget 100
East Gadget 150
West Widget 200
West Gadget 250
North Widget 300
North Gadget 350
South Widget 400
South Gadget 450
;
run;

proc means data=sales n mean std min max;
    class Region;
    var Sales;
    output out=stats_sales mean=Average_Sales std=Std_Dev_Sales min=Min_Sales max=Max_Sales;
run;

proc print data=stats_sales;
run;
```

**Expected Output (`sas_log_proc_means_basic.txt`):**

```
[INFO] Executing statement: data sales; input Region $ Product $ Sales; datalines; East Widget 100; East Gadget 150; West Widget 200; West Gadget 250; North Widget 300; North Gadget 350; South Widget 400; South Gadget 450; run;
[INFO] Executing DATA step: sales
[INFO] Assigned variable 'Region' = "East"
[INFO] Assigned variable 'Product' = "Widget"
[INFO] Assigned variable 'Sales' = 100
[INFO] Assigned variable 'Region' = "East"
[INFO] Assigned variable 'Product' = "Gadget"
[INFO] Assigned variable 'Sales' = 150
[INFO] Assigned variable 'Region' = "West"
[INFO] Assigned variable 'Product' = "Widget"
[INFO] Assigned variable 'Sales' = 200
[INFO] Assigned variable 'Region' = "West"
[INFO] Assigned variable 'Product' = "Gadget"
[INFO] Assigned variable 'Sales' = 250
[INFO] Assigned variable 'Region' = "North"
[INFO] Assigned variable 'Product' = "Widget"
[INFO] Assigned variable 'Sales' = 300
[INFO] Assigned variable 'Region' = "North"
[INFO] Assigned variable 'Product' = "Gadget"
[INFO] Assigned variable 'Sales' = 350
[INFO] Assigned variable 'Region' = "South"
[INFO] Assigned variable 'Product' = "Widget"
[INFO] Assigned variable 'Sales' = 400
[INFO] Assigned variable 'Region' = "South"
[INFO] Assigned variable 'Product' = "Gadget"
[INFO] Assigned variable 'Sales' = 450
[INFO] DATA step 'sales' executed successfully. 8 observations created.

[INFO] Executing statement: proc means data=sales n mean std min max; class Region; var Sales; output out=stats_sales mean=Average_Sales std=Std_Dev_Sales min=Min_Sales max=Max_Sales; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS executed successfully. Output dataset: 'stats_sales'.

[INFO] Executing statement: proc print data=stats_sales; run;
[INFO] Executing PROC PRINT

Region  _TYPE_  _FREQ_  Average_Sales  Std_Dev_Sales  Min_Sales  Max_Sales
---------------------------------------------------------------------------
East    1       2       125.00          35.36          100         150
West    1       2       225.00          35.36          200         250
North   1       2       325.00          35.36          300         350
South   1       2       425.00          35.36          400         450
Total   0       8       250.00          111.80         100         450

[INFO] PROC PRINT executed successfully.
```

**Test File (`test_proc_means_basic.cpp`):**

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
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        messages.emplace_back(std::string(buffer));
    }
    
    std::vector<std::string> messages;
};

// Test case for basic PROC MEANS
TEST(InterpreterTest, ProcMeansBasic) {
    DataEnvironment env;
    MockLogger logLogger;
    MockLogger lstLogger;
    Interpreter interpreter(env, logLogger, lstLogger);

    std::string input = 
        "data sales;\n"
        "input Region $ Product $ Sales;\n"
        "datalines;\n"
        "East Widget 100\n"
        "East Gadget 150\n"
        "West Widget 200\n"
        "West Gadget 250\n"
        "North Widget 300\n"
        "North Gadget 350\n"
        "South Widget 400\n"
        "South Gadget 450\n"
        ";\n"
        "run;\n"
        "\n"
        "proc means data=sales n mean std min max;\n"
        "    class Region;\n"
        "    var Sales;\n"
        "    output out=stats_sales mean=Average_Sales std=Std_Dev_Sales min=Min_Sales max=Max_Sales;\n"
        "run;\n"
        "\n"
        "proc print data=stats_sales;\n"
        "run;\n";

    interpreter.handleReplInput(input);

    // Verify dataset creation
    ASSERT_TRUE(env.datasets.find("stats_sales") != env.datasets.end());
    Dataset stats = env.datasets["stats_sales"];
    ASSERT_EQ(stats.size(), 5); // 4 groups + Total

    // Expected statistics
    std::vector<std::tuple<std::string, int, double, double, double, double>> expected = {
        {"East", 2, 125.0, 35.3553, 100.0, 150.0},
        {"West", 2, 225.0, 35.3553, 200.0, 250.0},
        {"North", 2, 325.0, 35.3553, 300.0, 350.0},
        {"South", 2, 425.0, 35.3553, 400.0, 450.0},
        {"Total", 8, 250.0, 111.8034, 100.0, 450.0}
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        const auto& row = stats[i];
        EXPECT_EQ(row.at("Region").value, std::get<0>(expected[i]));
        EXPECT_EQ(std::get<int>(row.at("_FREQ_").value), std::get<1>(expected[i]));
        EXPECT_DOUBLE_EQ(std::get<double>(row.at("Average_Sales").value), std::get<2>(expected[i]));
        EXPECT_NEAR(std::get<double>(row.at("Std_Dev_Sales").value), std::get<3>(expected[i]), 0.001);
        EXPECT_DOUBLE_EQ(std::get<double>(row.at("Min_Sales").value), std::get<4>(expected[i]));
        EXPECT_DOUBLE_EQ(std::get<double>(row.at("Max_Sales").value), std::get<5>(expected[i]));
    }

    // Verify log messages contain PROC MEANS execution
    bool meansExecuted = false;
    for (const auto& msg : logLogger.messages) {
        if (msg.find("PROC MEANS executed successfully. Output dataset: 'stats_sales'.") != std::string::npos) {
            meansExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(meansExecuted);

    // Verify PROC PRINT output
    bool printExecuted = false;
    for (const auto& msg : lstLogger.messages) {
        if (msg.find("Average_Sales") != std::string::npos &&
            msg.find("Std_Dev_Sales") != std::string::npos &&
            msg.find("Min_Sales") != std::string::npos &&
            msg.find("Max_Sales") != std::string::npos) {
            printExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(printExecuted);
}
```

**Explanation:**

- **`MockLogger`:** Captures log messages, enabling verification of `PROC MEANS` and `PROC PRINT` executions.
  
- **Assertions:**
  
  - Confirms that the `stats_sales` dataset is created with five observations (four groups and a total).
  
  - Verifies that each group's statistics (`n`, `mean`, `std`, `min`, `max`) match the expected values.
  
  - Checks that the log contains messages indicating successful execution of `PROC MEANS` and `PROC PRINT`.
  
  - Ensures that the printed report includes the calculated statistics.

---

### **48.8. Updating the Testing Suite**

Ensure that the newly created test cases for `PROC MEANS` are integrated into your testing suite and are executed automatically as part of your Continuous Integration (CI) pipeline.

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
    test_proc_means_basic.cpp
)
    
# Link Google Test and pthread
target_link_libraries(runTests PRIVATE gtest gtest_main pthread)
    
# Add tests
add_test(NAME SASTestSuite COMMAND runTests)
```

**Explanation:**

- **Test Integration:** Adds the new `PROC MEANS` test case (`test_proc_means_basic.cpp`) to the test executable, ensuring it is run alongside existing tests.
  
- **Automated Execution:** The tests will be executed automatically during the build process and as part of the CI pipeline, providing immediate feedback on any issues.

---

### **48.9. Best Practices for Implementing `PROC MEANS`**

1. **Modular Design:**
   - Structure your code to separate parsing, AST construction, and execution logic for maintainability and scalability.
   
2. **Comprehensive Testing:**
   - Develop a wide range of test cases covering various `PROC MEANS` features, including different statistics, multiple class variables, and handling of missing values.
   
3. **Robust Error Handling:**
   - Implement detailed error detection and reporting for scenarios like undefined variables, non-numeric variables for analysis, and unsupported statistics.
   
4. **Performance Optimization:**
   - Ensure that statistical calculations are efficient, especially for large datasets. Consider optimizing algorithms or leveraging multi-threading if necessary.
   
5. **User Feedback:**
   - Provide clear and descriptive log messages to inform users about the execution status, calculated statistics, and any encountered issues.
   
6. **Documentation:**
   - Document the implementation details of `PROC MEANS`, including supported statistics, usage examples, and limitations, to aid future development and user understanding.

---

### **48.10. Expanding to Other Advanced PROC Procedures**

With `PROC MEANS` now implemented, consider extending support to other essential PROC procedures to further enhance your interpreter's capabilities.

#### **48.10.1. `PROC FREQ`**

**Purpose:**
- Generates frequency tables for categorical variables, useful for analyzing distributions and identifying patterns.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcFreqNode` and related AST nodes to represent `PROC FREQ` components.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC FREQ` statements, capturing variables to analyze and options like `TABLES`, `ORDER`, and `OUT=`.
   
3. **Interpreter Execution:**
   - Implement frequency calculation logic, handling options like cross-tabulations and output dataset generation.
   
4. **Testing:**
   - Develop test cases to verify accurate frequency counts, handling of multiple variables, and correct report formatting.

#### **48.10.2. `PROC TABULATE`**

**Purpose:**
- Creates multi-dimensional tables summarizing data, allowing for detailed data analysis and reporting.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcTabulateNode` and related AST nodes to represent `PROC TABULATE` components.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC TABULATE` statements, capturing class variables, analysis variables, table definitions, and options.
   
3. **Interpreter Execution:**
   - Implement table generation logic, handling grouping, summarization, and formatting.
   
4. **Testing:**
   - Develop test cases to verify accurate table generation, correct summarization, and proper handling of various options.

#### **48.10.3. `PROC SQL`**

**Purpose:**
- Integrates SQL capabilities, enabling users to perform complex queries and data manipulations within the SAS environment.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcSQLNode` and related AST nodes to represent `PROC SQL` components.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse SQL statements within `PROC SQL`, handling `SELECT`, `FROM`, `WHERE`, `JOIN`, and other SQL clauses.
   
3. **Interpreter Execution:**
   - Implement SQL parsing and execution logic, enabling data querying, joining, and manipulation.
   
4. **Testing:**
   - Develop comprehensive test cases to ensure accurate SQL query execution, correct data retrieval, and proper error handling.

---

### **48.11. Enhancing the REPL Interface**

To improve user experience, consider enhancing the Read-Eval-Print Loop (REPL) interface with advanced features.

1. **Syntax Highlighting:**
   - Implement color-coding of keywords, variables, and operators to improve readability.
   
2. **Auto-completion:**
   - Provide suggestions for SAS keywords, variable names, and function names as users type.
   
3. **Command History:**
   - Allow users to navigate through previously entered commands using the arrow keys.
   
4. **Error Highlighting:**
   - Visually indicate syntax errors or problematic lines within the REPL to aid quick identification and correction.

**Implementation Suggestions:**

- Utilize libraries like [GNU Readline](https://tiswww.case.edu/php/chet/readline/rltop.html) or [linenoise](https://github.com/antirez/linenoise) to facilitate features like command history and auto-completion.
  
- Implement a basic syntax highlighter by analyzing the input and applying ANSI color codes based on token types.

---

### **48.12. Implementing Debugging Tools**

Introduce debugging capabilities to help users troubleshoot and optimize their scripts.

1. **Breakpoints:**
   - Enable users to set breakpoints in their scripts to pause execution and inspect variables.
   
2. **Step Execution:**
   - Allow users to step through their scripts line by line, facilitating debugging and learning.
   
3. **Variable Inspection:**
   - Provide commands to inspect the current state of variables and datasets at any point during execution.

**Implementation Suggestions:**

- Maintain a stack or pointer to track the current execution point.
  
- Implement commands within the REPL to set, list, and remove breakpoints.
  
- Provide commands to display variable values and dataset contents when execution is paused.

---

### **48.13. Optimizing Performance**

Ensure that your interpreter remains efficient, especially as it handles larger datasets and more complex operations.

1. **Efficient Data Structures:**
   - Refine data storage and retrieval mechanisms to handle large datasets more effectively.
   
2. **Parallel Processing:**
   - Leverage multi-threading for operations that can be executed concurrently, such as data transformations and aggregations.
   
3. **Memory Management:**
   - Optimize memory usage, particularly when dealing with extensive data and complex computations.
   
4. **Profiling and Benchmarking:**
   - Use profiling tools (e.g., `gprof`, `Valgrind`) to identify and address performance bottlenecks.

**Implementation Suggestions:**

- Utilize efficient data structures like hash tables for quick lookups.
  
- Implement sorting and aggregation algorithms that scale well with data size.
  
- Manage memory allocation carefully to prevent leaks and reduce overhead.

---

### **48.14. Enhancing Error Handling and Logging**

Improve the interpreter's robustness by refining its error handling and logging mechanisms.

1. **Detailed Error Messages:**
   - Include contextual information like line numbers, character positions, and variable states in error messages to aid debugging.
   
2. **Advanced Logging:**
   - Implement different logging levels (e.g., DEBUG, INFO, ERROR) and log outputs to files for persistent records.
   
3. **Graceful Degradation:**
   - Ensure that the interpreter can recover gracefully from non-critical errors, allowing users to continue working without restarting the session.

**Implementation Suggestions:**

- Incorporate exception handling that captures and reports errors with detailed context.
  
- Use structured logging formats (e.g., JSON) to facilitate log parsing and analysis.
  
- Allow users to configure logging preferences, such as log levels and output destinations.

---

### **48.15. Expanding the Testing Suite**

Continue to grow your testing suite to cover new features and ensure reliability.

1. **Increase Test Coverage:**
   - Develop more test cases covering various `PROC MEANS` features, data types, and edge cases.
   
2. **Automate Performance Testing:**
   - Incorporate benchmarks to monitor and optimize the interpreter's performance over time.
   
3. **Regression Testing:**
   - Implement tests to ensure that new features do not break existing functionalities.

**Implementation Suggestions:**

- Utilize parameterized tests in Google Test to handle multiple input scenarios efficiently.
  
- Integrate performance benchmarks using tools like [Google Benchmark](https://github.com/google/benchmark).
  
- Regularly run regression tests as part of your CI pipeline to catch unintended side effects of new changes.

---

### **48.16. Improving Documentation and User Guides**

Provide comprehensive resources to help users understand and utilize your interpreter effectively.

1. **Comprehensive Manuals:**
   - Create detailed documentation covering installation, usage, scripting syntax, and advanced features.
   
2. **Tutorials and Examples:**
   - Provide example scripts and tutorials to help users learn how to effectively use the interpreter.
   
3. **API Documentation:**
   - If applicable, provide documentation for any APIs or extensions exposed by the interpreter.

**Implementation Suggestions:**

- Use documentation generators like [Doxygen](https://www.doxygen.nl/) to create API documentation from your codebase.
  
- Host documentation on platforms like [Read the Docs](https://readthedocs.org/) or as a static site using [MkDocs](https://www.mkdocs.org/).
  
- Include a `README.md` with installation instructions, usage examples, and links to detailed documentation.

---

### **48.17. Implementing Security Features**

Ensure that your interpreter executes scripts securely, protecting the system and user data.

1. **Sandboxing:**
   - Isolate script execution to prevent unauthorized access to system resources.
   
2. **Input Validation:**
   - Rigorously validate user inputs to prevent injection attacks and ensure script integrity.
   
3. **Access Controls:**
   - Implement permissions and access controls to restrict sensitive operations.

**Implementation Suggestions:**

- Use operating system-level sandboxing mechanisms or leverage containerization technologies like Docker to isolate interpreter processes.
  
- Validate all inputs, especially those that interact with the file system or execute system commands.
  
- Implement user authentication and authorization if the interpreter is to be used in multi-user environments.

---

### **48.18. Exploring Extensibility and Plugin Systems**

Design your interpreter to be extensible, allowing users to add custom functionalities or integrate third-party tools.

1. **Plugin Architecture:**
   - Develop a framework that supports plugins, enabling users to extend the interpreter's capabilities.
   
2. **APIs for Extensions:**
   - Provide clear APIs and guidelines for developing extensions, fostering a community-driven ecosystem around your interpreter.
   
3. **Documentation for Developers:**
   - Offer comprehensive guides and examples to help developers create and integrate plugins.

**Implementation Suggestions:**

- Define a plugin interface, possibly using dynamic libraries (`.so`, `.dll`) that can be loaded at runtime.
  
- Use dependency injection or other design patterns to allow plugins to interact with the interpreter core.
  
- Provide example plugins and template projects to aid developers in creating their own extensions.

---

### **48.19. Engaging with Users and Gathering Feedback**

Foster a community around your interpreter to drive continuous improvement and adoption.

1. **Beta Testing:**
   - Release beta versions to a select group of users to gather feedback and identify areas for improvement.
   
2. **Community Building:**
   - Create forums, mailing lists, or chat channels where users can ask questions, share scripts, and collaborate.
   
3. **Iterative Development:**
   - Use feedback to prioritize features, fix bugs, and enhance the interpreter based on real-world usage patterns.

**Implementation Suggestions:**

- Utilize platforms like [GitHub](https://github.com/) for code hosting, issue tracking, and collaboration.
  
- Set up a [Discourse](https://www.discourse.org/) forum or a [Slack](https://slack.com/) workspace for user interactions.
  
- Conduct surveys or polls to understand user needs and preferences.

---

### **48.20. Summary of Achievements**

1. **Implemented `PROC MEANS`:**
   - Extended the AST to represent `PROC MEANS` components.
   
   - Updated the Lexer and Parser to recognize and parse `PROC MEANS` syntax.
   
   - Enhanced the Interpreter to execute `PROC MEANS`, handling class variables, analysis variables, statistics calculations, and output dataset generation.
   
2. **Developed Comprehensive Test Cases:**
   - Created automated tests to verify the correct parsing and execution of `PROC MEANS`.
   
   - Ensured that statistics are calculated accurately and output datasets are generated as expected.
   
3. **Integrated Tests into the Testing Suite:**
   - Updated the CMake configuration to include the new `PROC MEANS` test case.
   
   - Ensured that `PROC MEANS` tests are part of the automated testing pipeline.
   
4. **Established Best Practices:**
   - Emphasized modular design, comprehensive testing, robust error handling, performance optimization, and thorough documentation.

---

### **48.21. Next Steps**

With **`PROC MEANS`** successfully implemented, your SAS interpreter now supports another essential data analysis tool. To continue advancing your interpreter's capabilities, consider the following next steps:

1. **Implement Additional PROC Procedures:**
   - **`PROC FREQ`:**
     - Generates frequency tables for categorical variables, useful for analyzing distributions.
     
   - **`PROC TABULATE`:**
     - Creates multi-dimensional tables summarizing data, allowing for detailed data analysis and reporting.
     
   - **`PROC SQL`:**
     - Integrates SQL capabilities, enabling users to perform complex queries and data manipulations.
     
   - **`PROC UNIVARIATE`:**
     - Provides detailed statistics and graphical representations of data distributions.
     
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
     - Develop more test cases covering various `PROC MEANS` features, data types, and edge cases.
     
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

Implementing **`PROC MEANS`** marks a significant milestone in your SAS interpreter's development, equipping it with essential statistical analysis capabilities. By extending the AST, updating the Lexer and Parser, and enhancing the Interpreter's execution logic, you've successfully integrated a core SAS procedure that enables users to perform descriptive statistical analyses with ease.

The comprehensive test case ensures that `PROC MEANS` operates reliably, accurately calculating statistics and generating the expected output datasets. Integrating this feature not only enhances the interpreter's functionality but also aligns it more closely with the comprehensive toolset offered by SAS.

As you continue to build upon this foundation, adopting best practices such as modular design, thorough testing, robust error handling, performance optimization, and comprehensive documentation will be instrumental in creating a reliable, efficient, and user-friendly SAS interpreter. Embracing these strategies will facilitate the addition of new features and ensure that the interpreter remains maintainable and scalable as its capabilities expand.

Keep up the excellent work, and feel free to reach out for further guidance or assistance as you progress with your project. Good luck!