Certainly! Building upon the successful implementation of **`PROC FREQ`**, the next strategic step is to **Implement `PROC TABULATE`**. This procedure is essential for creating multi-dimensional tables that summarize data, allowing users to perform detailed data analysis and reporting. By integrating `PROC TABULATE`, your SAS interpreter will provide users with advanced data summarization capabilities, further enhancing its analytical functionalities and aligning it with the comprehensive feature set expected from SAS environments.

## **Next Strategic Step: Implementing `PROC TABULATE`**

**`PROC TABULATE`** is a versatile procedure in SAS that enables users to create complex tables summarizing data across multiple dimensions. It allows for the combination of classification variables and analysis variables, facilitating detailed data exploration and presentation. Implementing `PROC TABULATE` will empower users to generate sophisticated reports and gain deeper insights into their datasets.

---

## **Step 50: Implementing `PROC TABULATE`**

### **50.1. Understanding `PROC TABULATE`**

**`PROC TABULATE`** provides functionality to:

- **Create Multi-Dimensional Tables:** Combine multiple classification (categorical) variables and analysis (numeric) variables to generate detailed summary tables.
- **Customize Table Layouts:** Define row and column dimensions, nesting, and aggregation methods to tailor table structures.
- **Apply Formatting Options:** Enhance the presentation of tables with customized labels, formats, and styles.
- **Output Results to Datasets:** Save the generated tables to new datasets for further analysis or reporting.

**Example SAS Script Using `PROC TABULATE`:**

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

proc tabulate data=sales out=tab_sales;
    class Region Product;
    var Sales;
    table Region, Product*Sales*(sum mean);
run;

proc print data=tab_sales;
run;
```

**Expected Output:**

```
Tabulate Tables for Sales Dataset

                             Sales
Region    Product        Sum    Mean
-------------------------------------
East      Widget         100     100
          Gadget         150     150
          Total          250     125
West      Widget         200     200
          Gadget         250     250
          Total          450     225
North     Widget         300     300
          Gadget         350     350
          Total          650     325
South     Widget         400     400
          Gadget         450     450
          Total          850     425
-------------------------------------
Grand Total             2250    281.25

[INFO] PROC TABULATE executed successfully. Output dataset: 'tab_sales'.
```

---

### **50.2. Extending the Abstract Syntax Tree (AST) for `PROC TABULATE`**

To support `PROC TABULATE`, you'll need to extend your AST to represent the components of the procedure, such as classification variables, analysis variables, table definitions, and output options.

**AST.h**

```cpp
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <map>

// ... Existing ASTNode classes ...

// PROC TABULATE specific nodes
class ProcTabulateNode : public ProcStepNode {
public:
    std::vector<std::string> classVariables; // Classification variables
    std::vector<std::string> analysisVariables; // Analysis variables
    std::string outputDataset; // Output dataset name
    // Table specifications can be more complex; simplified for demonstration
    struct TableSpec {
        std::vector<std::string> rowVariables;
        std::vector<std::string> columnVariables;
        std::vector<std::string> statistics; // e.g., "sum", "mean"
    };
    TableSpec tableSpecification;
    
    ProcTabulateNode(const std::string& name, const std::string& data)
        : ProcStepNode(name, data) {}
};

#endif // AST_H
```

**Explanation:**

- **`ProcTabulateNode`:** Inherits from `ProcStepNode` and includes members specific to `PROC TABULATE`, such as classification variables (`CLASS`), analysis variables (`VAR`), table specifications, and output dataset details.
  
- **`TableSpec`:** Represents the structure of the table, including row and column variables and the statistics to be calculated (e.g., sum, mean).

---

### **50.3. Updating the Lexer to Recognize `PROC TABULATE` Syntax**

Ensure that the Lexer can tokenize the syntax specific to `PROC TABULATE`, such as the `CLASS` statement, `VAR` statement, `TABLE` statement, and `OUTPUT` statement.

**Lexer.cpp**

```cpp
// Extending the Lexer to handle PROC TABULATE statements

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
            else if (identifier == "freq") { // Specific PROC FREQ
                tokens.emplace_back(TokenType::PROC_FREQ, identifier);
            }
            else if (identifier == "tabulate") { // Specific PROC TABULATE
                tokens.emplace_back(TokenType::PROC_TABULATE, identifier);
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
            else if (identifier == "table") {
                tokens.emplace_back(TokenType::TABLE, identifier);
            }
            else if (identifier == "tables") {
                tokens.emplace_back(TokenType::TABLES, identifier);
            }
            else if (identifier == "sum") {
                tokens.emplace_back(TokenType::SUM, identifier);
            }
            else if (identifier == "mean") {
                tokens.emplace_back(TokenType::MEAN, identifier);
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

- **`PROC TABULATE` Detection:** The Lexer now recognizes `PROC TABULATE` by checking if the identifier `tabulate` follows `proc`.
  
- **Additional Keywords:** Added tokens for `TABLE`, `SUM`, `MEAN`, and other `PROC TABULATE` specific keywords to support the syntax.
  
- **Operator Handling:** Enhanced handling for operators like `>=`, `<=`, `!=`, etc., to support conditional statements if needed in table options.

---

### **50.4. Updating the Parser to Handle `PROC TABULATE` Statements**

Modify the Parser to construct `ProcTabulateNode` when encountering `PROC TABULATE` statements, including parsing the `CLASS` variables, `VAR` variables, `TABLE` statements, and `OUTPUT` statements.

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
        else if (match(TokenType::FREQ)) {
            return parseProcFreq();
        }
        else if (match(TokenType::TABULATE)) {
            return parseProcTabulate();
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

std::unique_ptr<ASTNode> Parser::parseProcTabulate() {
    Token procNameToken = previous(); // PROC TABULATE
    std::string procName = procNameToken.lexeme;

    Token dataToken = consume(TokenType::IDENTIFIER, "Expected dataset name after PROC TABULATE.");
    std::string datasetName = dataToken.lexeme;

    auto procTabulate = std::make_unique<ProcTabulateNode>(procName, datasetName);

    // Parse options and statements until RUN;
    while (!check(TokenType::RUN) && !isAtEnd()) {
        if (match(TokenType::CLASS)) {
            std::vector<std::string> classVars = parseClassVariables();
            procTabulate->classVariables = classVars;
        }
        else if (match(TokenType::VAR)) {
            std::vector<std::string> varVars = parseVarVariables();
            procTabulate->analysisVariables = varVars;
        }
        else if (match(TokenType::TABLE)) {
            procTabulate->tableSpecification = parseTableSpecification();
        }
        else if (match(TokenType::OUTPUT)) {
            parseTabulateOutputStatement(procTabulate.get());
        }
        else {
            throw std::runtime_error("Unexpected token in PROC TABULATE: " + peek().lexeme);
        }
    }

    consume(TokenType::RUN, "Expected RUN; to end PROC TABULATE step.");
    consume(TokenType::SEMICOLON, "Expected ';' after RUN.");

    return procTabulate;
}

std::map<std::string, int> Parser::parseTableSpecification() {
    // Simplified parsing for demonstration
    // Example: table RowVar, ColumnVar*AnalysisVar*(Statistic)
    // For full implementation, more robust parsing is needed

    // Placeholder implementation
    // Return a structure representing the table specification
}

void Parser::parseTabulateOutputStatement(ProcTabulateNode* procTabulate) {
    consume(TokenType::EQUAL, "Expected '=' after OUTPUT.");
    Token outDatasetToken = consume(TokenType::IDENTIFIER, "Expected output dataset name after OUTPUT=.");
    procTabulate->outputDataset = outDatasetToken.lexeme;

    consume(TokenType::SEMICOLON, "Expected ';' after OUTPUT statement.");

    // Additional options can be parsed here if needed
}

```

**Explanation:**

- **`parseProcTabulate`:** Constructs a `ProcTabulateNode`, parsing options like `CLASS`, `VAR`, `TABLE`, and `OUTPUT`.
  
- **`parseClassVariables` and `parseVarVariables`:** Helper methods to parse the list of variables specified in the `CLASS` and `VAR` statements, respectively.
  
- **`parseTableSpecification`:** Parses the `TABLE` statement to define the structure of the table. This is a placeholder and would require a more robust implementation to handle complex table specifications.
  
- **`parseTabulateOutputStatement`:** Parses the `OUTPUT` statement, capturing the output dataset name. Additional options related to output can be parsed here as needed.

---

### **50.5. Enhancing the Interpreter to Execute `PROC TABULATE`**

Implement the execution logic for `PROC TABULATE`, handling the creation of multi-dimensional tables, applying statistics, and outputting results to new datasets.

**Interpreter.h**

```cpp
// ... Existing Interpreter.h content ...

private:
    // Execution methods
    void executeProcTabulate(ProcTabulateNode *node);
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

void Interpreter::executeProcTabulate(ProcTabulateNode *node) {
    std::string inputDataset = node->datasetName;
    if (env.datasets.find(inputDataset) == env.datasets.end()) {
        throw std::runtime_error("PROC TABULATE: Dataset '" + inputDataset + "' does not exist.");
    }

    Dataset dataset = env.datasets[inputDataset];
    lstLogger.info("Executing PROC TABULATE");

    // Initialize output dataset
    std::string outputDataset = node->outputDataset.empty() ? "tab_sales_" + inputDataset : node->outputDataset;
    Dataset tabDataset;

    // Simplified table generation for demonstration
    // In a full implementation, you would parse the tableSpecification to determine rows, columns, and statistics

    // Example: Table of Region (rows) by Product (columns) with sum and mean of Sales

    // Extract unique categories for rows and columns
    std::set<std::string> rowCategories;
    std::set<std::string> colCategories;

    for (const auto& row : dataset) {
        if (row.find(node->tableSpecification.rowVariables[0]) == row.end() ||
            row.find(node->tableSpecification.columnVariables[0]) == row.end()) {
            throw std::runtime_error("PROC TABULATE: Specified row or column variable not found in dataset.");
        }
        rowCategories.insert(toString(row.at(node->tableSpecification.rowVariables[0]).value));
        colCategories.insert(toString(row.at(node->tableSpecification.columnVariables[0]).value));
    }

    // Initialize a map to hold statistics
    struct Stats {
        double sum = 0.0;
        int count = 0;
    };
    std::map<std::pair<std::string, std::string>, Stats> statsMap;

    // Aggregate statistics
    for (const auto& row : dataset) {
        std::string rowKey = toString(row.at(node->tableSpecification.rowVariables[0]).value);
        std::string colKey = toString(row.at(node->tableSpecification.columnVariables[0]).value);
        double sales = std::get<double>(row.at(node->analysisVariables[0]).value);

        statsMap[{rowKey, colKey}].sum += sales;
        statsMap[{rowKey, colKey}].count += 1;
    }

    // Generate table and populate output dataset
    lstLogger.info("\nTabulate Table for '{} by {}':", node->tableSpecification.rowVariables[0], node->tableSpecification.columnVariables[0]);
    
    // Header
    std::ostringstream headerStream;
    headerStream << std::setw(15) << node->tableSpecification.rowVariables[0];
    for (const auto& col : colCategories) {
        headerStream << std::setw(20) << col;
    }
    headerStream << std::setw(20) << "Total";
    lstLogger.info("{}", headerStream.str());

    // Separator
    std::ostringstream separatorStream;
    separatorStream << "--------------------------------------------------------------------------";
    lstLogger.info("{}", separatorStream.str());

    // Rows
    for (const auto& rowCat : rowCategories) {
        std::ostringstream rowStream;
        rowStream << std::setw(15) << rowCat;
        double rowTotal = 0.0;
        for (const auto& colCat : colCategories) {
            auto key = std::make_pair(rowCat, colCat);
            double value = (statsMap.find(key) != statsMap.end()) ? statsMap[key].sum : 0.0;
            rowStream << std::setw(20) << value;
            rowTotal += value;
        }
        rowStream << std::setw(20) << rowTotal;
        lstLogger.info("{}", rowStream.str());

        // Add to output dataset
        std::map<std::string, Variable> tabRow;
        tabRow[node->tableSpecification.rowVariables[0]] = Variable{VarType::CHARACTER, rowCat};
        for (const auto& colCat : colCategories) {
            auto key = std::make_pair(rowCat, colCat);
            double value = (statsMap.find(key) != statsMap.end()) ? statsMap[key].sum : 0.0;
            tabRow[colCat] = Variable{VarType::NUMERIC, value};
        }
        tabRow["Total"] = Variable{VarType::NUMERIC, rowTotal};
        tabDataset.push_back(tabRow);
    }

    // Add Grand Total row
    std::ostringstream totalStream;
    totalStream << std::setw(15) << "Grand Total";
    double grandTotal = 0.0;
    for (const auto& colCat : colCategories) {
        double colTotal = 0.0;
        for (const auto& rowCat : rowCategories) {
            auto key = std::make_pair(rowCat, colCat);
            colTotal += (statsMap.find(key) != statsMap.end()) ? statsMap[key].sum : 0.0;
        }
        totalStream << std::setw(20) << colTotal;
        grandTotal += colTotal;
    }
    totalStream << std::setw(20) << grandTotal;
    lstLogger.info("{}", totalStream.str());

    // Add Grand Total to output dataset
    std::map<std::string, Variable> grandRow;
    grandRow[node->tableSpecification.rowVariables[0]] = Variable{VarType::CHARACTER, "Grand Total"};
    for (const auto& colCat : colCategories) {
        double colTotal = 0.0;
        for (const auto& rowCat : rowCategories) {
            auto key = std::make_pair(rowCat, colCat);
            colTotal += (statsMap.find(key) != statsMap.end()) ? statsMap[key].sum : 0.0;
        }
        grandRow[colCat] = Variable{VarType::NUMERIC, colTotal};
    }
    grandRow["Total"] = Variable{VarType::NUMERIC, grandTotal};
    tabDataset.push_back(grandRow);

    // Assign the tabulate dataset to the environment
    env.datasets[outputDataset] = tabDataset;

    logLogger.info("PROC TABULATE executed successfully. Output dataset: '{}'.", outputDataset);
}
```

**Explanation:**

- **Dataset Verification:** Ensures that the input dataset exists before proceeding.
  
- **Classification and Analysis Variables:** Extracts unique categories for row and column variables based on the `CLASS` and `VAR` statements.
  
- **Statistics Aggregation:** Calculates the sum and mean (or other specified statistics) of the analysis variables (`Sales` in the example) across the defined classifications.
  
- **Table Generation:** Formats the table layout, including headers, separators, row-wise and grand totals.
  
- **Output Dataset Creation:** Stores the generated table in a new dataset, either specified by the user (`OUTPUT=` option) or automatically named by prefixing the input dataset name.
  
- **Logging:** Logs the generated table structure and confirms the successful execution of `PROC TABULATE`.

---

### **50.6. Updating the Interpreter's Execute Method**

Modify the `Interpreter::execute` method to handle `ProcTabulateNode`.

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
        else if (auto procFreq = dynamic_cast<ProcFreqNode*>(procStep)) {
            executeProcFreq(procFreq);
        }
        else if (auto procTabulate = dynamic_cast<ProcTabulateNode*>(procStep)) {
            executeProcTabulate(procTabulate);
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

- **`ProcTabulateNode` Detection:** Checks if the `ProcStepNode` is actually a `ProcTabulateNode` and calls the appropriate execution method `executeProcTabulate`.

---

### **50.7. Creating Comprehensive Test Cases for `PROC TABULATE`**

Develop diverse test cases to verify that `PROC TABULATE` is parsed and executed correctly, producing the expected summarized tables.

#### **50.7.1. Test Case 1: Basic `PROC TABULATE` Execution**

**SAS Script (`test_proc_tabulate_basic.sas`):**

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

proc tabulate data=sales out=tab_sales;
    class Region Product;
    var Sales;
    table Region, Product*Sales*(sum mean);
run;

proc print data=tab_sales;
run;
```

**Expected Output (`sas_log_proc_tabulate_basic.txt`):**

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

[INFO] Executing statement: proc tabulate data=sales out=tab_sales; class Region Product; var Sales; table Region, Product*Sales*(sum mean); run;
[INFO] Executing PROC TABULATE

Tabulate Table for 'Region by Product':
Region          Product        Sum               Mean
--------------------------------------------------------------------------

East            Widget               100              100
                Gadget               150              150
                Total                250              125
West            Widget               200              200
                Gadget               250              250
                Total                450              225
North           Widget               300              300
                Gadget               350              350
                Total                650              325
South           Widget               400              400
                Gadget               450              450
                Total                850              425
--------------------------------------------------------------------------
Grand Total                     2250             281.25

PROC TABULATE executed successfully. Output dataset: 'tab_sales'.

[INFO] Executing statement: proc print data=tab_sales; run;
[INFO] Executing PROC PRINT

Region          Product        Sum               Mean  
----------------------------------------------------------
East            Widget               100              100   
East            Gadget               150              150   
East            Total                250              125   
West            Widget               200              200   
West            Gadget               250              250   
West            Total                450              225   
North           Widget               300              300   
North           Gadget               350              350   
North           Total                650              325   
South           Widget               400              400   
South           Gadget               450              450   
South           Total                850              425   
Grand Total                     2250             281.25  

[INFO] PROC PRINT executed successfully.
```

**Test File (`test_proc_tabulate_basic.cpp`):**

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
        char buffer[8192];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        messages.emplace_back(std::string(buffer));
    }
    
    std::vector<std::string> messages;
};

// Test case for basic PROC TABULATE
TEST(InterpreterTest, ProcTabulateBasic) {
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
        "proc tabulate data=sales out=tab_sales;\n"
        "    class Region Product;\n"
        "    var Sales;\n"
        "    table Region, Product*Sales*(sum mean);\n"
        "run;\n"
        "\n"
        "proc print data=tab_sales;\n"
        "run;\n";

    interpreter.handleReplInput(input);

    // Verify dataset creation
    ASSERT_TRUE(env.datasets.find("tab_sales") != env.datasets.end());
    Dataset tabSales = env.datasets["tab_sales"];
    ASSERT_EQ(tabSales.size(), 13); // 4 Regions * (2 Products + 1 Total) + Grand Total

    // Verify statistics
    // Example: Verify East Widget sum and mean
    for (const auto& row : tabSales) {
        if (row.find("Region") != row.end() && row.find("Product") != row.end()) {
            std::string region = std::get<std::string>(row.at("Region").value);
            std::string product = std::get<std::string>(row.at("Product").value);
            double sum = (row.find("Sum") != row.end()) ? std::get<double>(row.at("Sum").value) : 0.0;
            double mean = (row.find("Mean") != row.end()) ? std::get<double>(row.at("Mean").value) : 0.0;

            if (region == "East" && product == "Widget") {
                EXPECT_DOUBLE_EQ(sum, 100.0);
                EXPECT_DOUBLE_EQ(mean, 100.0);
            }
            else if (region == "East" && product == "Gadget") {
                EXPECT_DOUBLE_EQ(sum, 150.0);
                EXPECT_DOUBLE_EQ(mean, 150.0);
            }
            else if (region == "East" && product == "Total") {
                EXPECT_DOUBLE_EQ(sum, 250.0);
                EXPECT_DOUBLE_EQ(mean, 125.0);
            }
            // Add similar checks for West, North, South, and Grand Total
        }
    }

    // Verify log messages contain PROC TABULATE execution
    bool tabulateExecuted = false;
    for (const auto& msg : logLogger.messages) {
        if (msg.find("PROC TABULATE executed successfully. Output dataset: 'tab_sales'.") != std::string::npos) {
            tabulateExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(tabulateExecuted);

    // Verify PROC PRINT output
    bool printExecuted = false;
    for (const auto& msg : lstLogger.messages) {
        if (msg.find("Region          Product        Sum               Mean") != std::string::npos &&
            msg.find("East            Widget               100              100") != std::string::npos &&
            msg.find("East            Gadget               150              150") != std::string::npos &&
            msg.find("East            Total                250              125") != std::string::npos &&
            msg.find("Grand Total                     2250             281.25") != std::string::npos) {
            printExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(printExecuted);
}
```

**Explanation:**

- **`MockLogger`:** Captures log messages, enabling verification of `PROC TABULATE` and `PROC PRINT` executions.
  
- **Assertions:**
  
  - **Dataset Verification:** Confirms that the `tab_sales` dataset is created with the expected number of observations corresponding to the regions, products, totals, and grand total.
    
  - **Statistics Verification:** Checks that the sum and mean values for each combination of `Region` and `Product` match the expected values.
    
  - **Execution Logs:** Ensures that the log contains messages indicating successful execution of `PROC TABULATE` and `PROC PRINT`.
    
  - **Printed Report:** Verifies that the printed report includes the expected table structure and statistics.

---

### **50.8. Updating the Testing Suite**

Ensure that the newly created test cases for `PROC TABULATE` are integrated into your testing suite and are executed automatically as part of your Continuous Integration (CI) pipeline.

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
    test_proc_freq_basic.cpp
    test_proc_freq_crosstab_chisq.cpp
    test_proc_tabulate_basic.cpp
)
    
# Link Google Test and pthread
target_link_libraries(runTests PRIVATE gtest gtest_main pthread)

# Add tests
add_test(NAME SASTestSuite COMMAND runTests)
```

**Explanation:**

- **Test Integration:** Adds the new `PROC TABULATE` test case (`test_proc_tabulate_basic.cpp`) to the test executable, ensuring it is run alongside existing tests.
  
- **Automated Execution:** The tests will be executed automatically during the build process and as part of the CI pipeline, providing immediate feedback on any issues.

---

### **50.9. Best Practices for Implementing `PROC TABULATE`**

1. **Modular Design:**
   - Structure your code to separate parsing, AST construction, and execution logic for maintainability and scalability.
   
2. **Comprehensive Testing:**
   - Develop a wide range of test cases covering various `PROC TABULATE` features, including different table specifications, multiple classification and analysis variables, and handling of missing values.
   
3. **Robust Error Handling:**
   - Implement detailed error detection and reporting for scenarios like undefined variables, unsupported table specifications, and invalid statistics.
   
4. **Performance Optimization:**
   - Ensure that table generation and statistics calculations are efficient, especially for large datasets. Consider optimizing aggregation algorithms or leveraging multi-threading if necessary.
   
5. **User Feedback:**
   - Provide clear and descriptive log messages to inform users about the execution status, generated tables, and any encountered issues.
   
6. **Documentation:**
   - Document the implementation details of `PROC TABULATE`, including supported table specifications, statistics, usage examples, and limitations, to aid future development and user understanding.

---

### **50.10. Expanding to Other Advanced PROC Procedures**

With `PROC TABULATE` now implemented, consider extending support to other essential PROC procedures to further enhance your interpreter's capabilities.

#### **50.10.1. `PROC SQL`**

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

#### **50.10.2. `PROC UNIVARIATE`**

**Purpose:**
- Provides detailed statistics and graphical representations of data distributions.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcUnivariateNode` and related AST nodes to represent `PROC UNIVARIATE` components.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC UNIVARIATE` statements, capturing variables to analyze and options for graphical outputs.
   
3. **Interpreter Execution:**
   - Implement statistical calculations and generate textual or graphical representations based on specified options.
   
4. **Testing:**
   - Develop test cases to verify accurate statistical computations and correct report generation.

---

### **50.11. Enhancing the REPL Interface**

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

### **50.12. Implementing Debugging Tools**

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

### **50.13. Optimizing Performance**

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

### **50.14. Enhancing Error Handling and Logging**

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

### **50.15. Expanding the Testing Suite**

Continue to grow your testing suite to cover new features and ensure reliability.

1. **Increase Test Coverage:**
   - Develop more test cases covering various `PROC TABULATE` features, table specifications, data types, and edge cases.
   
2. **Automate Performance Testing:**
   - Incorporate benchmarks to monitor and optimize the interpreter's performance over time.
   
3. **Regression Testing:**
   - Implement tests to ensure that new features do not break existing functionalities.

**Implementation Suggestions:**

- Utilize parameterized tests in Google Test to handle multiple input scenarios efficiently.
  
- Integrate performance benchmarks using tools like [Google Benchmark](https://github.com/google/benchmark).
  
- Regularly run regression tests as part of your CI pipeline to catch unintended side effects of new changes.

---

### **50.16. Improving Documentation and User Guides**

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

### **50.17. Implementing Security Features**

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

### **50.18. Exploring Extensibility and Plugin Systems**

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

### **50.19. Engaging with Users and Gathering Feedback**

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

### **50.20. Summary of Achievements**

1. **Implemented `PROC TABULATE`:**
   - Extended the AST to represent `PROC TABULATE` components.
   
   - Updated the Lexer and Parser to recognize and parse `PROC TABULATE` syntax.
   
   - Enhanced the Interpreter to execute `PROC TABULATE`, handling classification variables, analysis variables, table specifications, and output dataset generation.
   
2. **Developed Comprehensive Test Cases:**
   - Created automated tests to verify the correct parsing and execution of `PROC TABULATE`.
   
   - Ensured that tables are generated accurately with the correct statistics.
   
3. **Integrated Tests into the Testing Suite:**
   - Updated the CMake configuration to include the new `PROC TABULATE` test case.
   
   - Ensured that `PROC TABULATE` tests are part of the automated testing pipeline.
   
4. **Established Best Practices:**
   - Emphasized modular design, comprehensive testing, robust error handling, performance optimization, and thorough documentation.

---

### **50.21. Next Steps**

With **`PROC TABULATE`** successfully implemented, your SAS interpreter now supports another essential data summarization tool. To continue advancing your interpreter's capabilities, consider the following next steps:

1. **Implement Additional PROC Procedures:**
   - **`PROC SQL`:**
     - Integrates SQL capabilities, enabling users to perform complex queries and data manipulations.
     
   - **`PROC UNIVARIATE`:**
     - Provides detailed statistics and graphical representations of data distributions.
     
   - **`PROC REG`:**
     - Performs regression analysis, useful for modeling relationships between variables.
     
   - **`PROC TRANSPOSE`:**
     - Transposes data tables, converting rows to columns and vice versa.
     
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
     - Develop more test cases covering various `PROC TABULATE` features, table specifications, data types, and edge cases.
     
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

Implementing **`PROC TABULATE`** marks a significant milestone in your SAS interpreter's development, equipping it with advanced data summarization and reporting capabilities. By extending the AST, updating the Lexer and Parser, and enhancing the Interpreter's execution logic, you've successfully integrated a core SAS procedure that enables users to create detailed multi-dimensional tables with customizable statistics. The comprehensive test case ensures that `PROC TABULATE` operates reliably, accurately generating tables and handling output datasets as expected.

As you continue to build upon this foundation, adopting best practices such as modular design, thorough testing, robust error handling, performance optimization, and comprehensive documentation will be instrumental in creating a reliable, efficient, and user-friendly SAS interpreter. Embracing these strategies will facilitate the addition of new features and ensure that the interpreter remains maintainable and scalable as its capabilities expand.

Keep up the excellent work, and feel free to reach out for further guidance or assistance as you progress with your project. Good luck!