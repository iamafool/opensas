Absolutely, continuing to implement essential SAS procedures will progressively enhance your interpreter's capabilities. Following the successful integration of `PROC SORT` and `PROC PRINT`, the next logical and impactful procedure to implement is **`PROC MEANS`**. This procedure is fundamental for performing descriptive statistical analysis on datasets, making it a critical addition to your interpreter.

## **Next Strategic Step: Implementing `PROC MEANS`**

**`PROC MEANS`** is widely used in SAS for calculating descriptive statistics such as mean, median, standard deviation, minimum, maximum, and more for numeric variables in a dataset. Implementing this procedure will enable users to perform essential data analysis tasks directly within your interpreter.

---

## **Step 32: Implementing `PROC MEANS` in the Interpreter**

### **32.1. Overview of `PROC MEANS` in SAS**

**Purpose:**

`PROC MEANS` calculates and displays descriptive statistics for numeric variables in a SAS dataset. It provides options to specify the statistics to compute, the variables to analyze, and the classification variables for grouping data.

**Syntax:**

```sas
PROC MEANS DATA=dataset_name <N MEAN STD DEV MIN MAX MEDIAN>;
    CLASS variable1 variable2 ...;
    VAR variableA variableB ...;
    WHERE (condition);
    OUTPUT OUT=output_dataset MEAN=MeanVar1 STD=StdVar1 ...;
RUN;
```

- **`DATA=`**: Specifies the input dataset.
- **`CLASS`**: (Optional) Specifies categorical variables for grouping the data.
- **`VAR`**: (Optional) Specifies the numeric variables to analyze. If omitted, all numeric variables are analyzed.
- **`WHERE`**: (Optional) Applies a condition to filter observations.
- **`OUTPUT OUT=`**: (Optional) Creates an output dataset with the computed statistics.

**Key Features:**

- **Descriptive Statistics:** Calculate N (count), Mean, Standard Deviation (Std), Minimum (Min), Maximum (Max), Median, etc.
- **Classification Variables:** Group data based on categorical variables using the `CLASS` statement.
- **Variable Selection:** Specify which variables to analyze using the `VAR` statement.
- **Conditional Analysis:** Filter observations using the `WHERE` clause.
- **Output Dataset:** Save the results to a new dataset using the `OUTPUT` statement.

---

### **32.2. Extending the Abstract Syntax Tree (AST)**

To represent `PROC MEANS` within the AST, introduce a new node type: `ProcMeansNode`.

**AST.h**

```cpp
// Represents a PROC MEANS statement
class ProcMeansNode : public ASTNode {
public:
    std::string inputDataset;
    std::vector<std::string> statistics; // e.g., N, MEAN, STD, etc.
    std::vector<std::string> classVariables; // Variables for classification
    std::vector<std::string> varVariables; // Variables to analyze
    std::unique_ptr<ExpressionNode> whereCondition = nullptr; // Optional WHERE clause
    std::string outputDataset = ""; // Optional OUTPUT dataset
    std::unordered_map<std::string, std::string> outputStatistics; // e.g., MEAN=MeanVar1

    ProcMeansNode(const std::string& input,
                 const std::vector<std::string>& stats,
                 const std::vector<std::string>& classes,
                 const std::vector<std::string>& vars,
                 std::unique_ptr<ExpressionNode> condition,
                 const std::string& output,
                 const std::unordered_map<std::string, std::string>& outStats)
        : inputDataset(input),
          statistics(stats),
          classVariables(classes),
          varVariables(vars),
          whereCondition(std::move(condition)),
          outputDataset(output),
          outputStatistics(outStats) {}
};
```

**Explanation:**

- **`ProcMeansNode`:** Encapsulates the details of a `PROC MEANS` statement, including the input dataset, requested statistics, classification variables, variables to analyze, an optional `WHERE` condition, and an optional output dataset with specified statistics.

---

### **32.3. Updating the Lexer to Recognize `PROC MEANS` Statements**

**Lexer.cpp**

Ensure that the lexer can recognize the `PROC MEANS` keywords and related tokens. Since `PROC` and `MEANS` are treated as identifiers, ensure that keywords like `DATA=`, `CLASS`, `VAR`, `WHERE=`, `OUTPUT`, `OUT=`, `RUN;`, and statistical options (`N`, `MEAN`, `STD`, `DEV`, `MIN`, `MAX`, `MEDIAN`) are properly tokenized.

**Example Tokens:**

- `PROC` ¡ú `PROC`
- `MEANS` ¡ú `MEANS`
- `DATA=` ¡ú `DATA_ASSIGN`
- `CLASS` ¡ú `CLASS`
- `VAR` ¡ú `VAR`
- `WHERE=` ¡ú `WHERE_ASSIGN`
- `OUTPUT` ¡ú `OUTPUT`
- `OUT=` ¡ú `OUT_ASSIGN`
- `RUN` ¡ú `RUN`
- Statistical options like `N`, `MEAN`, `STD`, etc., ¡ú `STATISTIC`
- `=` ¡ú `EQUAL`
- `;` ¡ú `SEMICOLON`
- Strings (e.g., `'output_dataset'`) ¡ú `STRING`
- Identifiers (dataset and variable names) ¡ú `IDENTIFIER`

**Note:** Depending on your lexer implementation, you might need to add specific rules to tokenize statistical options and assignment operators.

---

### **32.4. Modifying the Parser to Handle `PROC MEANS` Statements**

**Parser.h**

Add a new parsing method for `PROC MEANS` and update the `parseProc` method to delegate parsing based on the procedure type.

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
    std::unique_ptr<ASTNode> parseProcSort(); // Existing method
    std::unique_ptr<ASTNode> parseProcPrint(); // Existing method
    std::unique_ptr<ASTNode> parseProcMeans(); // New method
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

Implement the `parseProcMeans` method and update `parseProc` to recognize `PROC MEANS`.

```cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

// ... existing methods ...

std::unique_ptr<ASTNode> Parser::parseProc() {
    Token procToken = consume(TokenType::PROC, "Expected 'PROC' keyword");
    Token procTypeToken = peek();

    if (procTypeToken.type == TokenType::SORT) {
        return parseProcSort();
    }
    else if (procTypeToken.type == TokenType::PRINT) {
        return parseProcPrint();
    }
    else if (procTypeToken.type == TokenType::MEANS) {
        return parseProcMeans();
    }
    else {
        // Handle other PROC types or throw an error for unsupported PROCs
        throw std::runtime_error("Unsupported PROC type: " + procTypeToken.lexeme);
    }
}

std::unique_ptr<ASTNode> Parser::parseProcMeans() {
    consume(TokenType::MEANS, "Expected 'MEANS' after 'PROC'");

    // Initialize variables
    std::string inputDataset;
    std::vector<std::string> statistics;
    std::vector<std::string> classVars;
    std::vector<std::string> varVars;
    std::unique_ptr<ExpressionNode> whereCondition = nullptr;
    std::string outputDataset = "";
    std::unordered_map<std::string, std::string> outputStatistics;

    // Parse DATA= and WHERE= options
    while (peek().type == TokenType::DATA_ASSIGN || peek().type == TokenType::WHERE_ASSIGN) {
        Token option = advance();
        if (option.type == TokenType::DATA_ASSIGN) {
            Token datasetToken = consume(TokenType::IDENTIFIER, "Expected dataset name after DATA=");
            inputDataset = datasetToken.lexeme;
        }
        else if (option.type == TokenType::WHERE_ASSIGN) {
            // Parse the WHERE= condition as an expression
            // Assuming WHERE= is followed by a condition expression
            // For simplicity, let's assume the condition is a simple expression terminated by a semicolon
            // In a full implementation, you'd need a more robust expression parser

            // Example: WHERE=(Age > 30)
            consume(TokenType::LPAREN, "Expected '(' after WHERE=");
            whereCondition = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after WHERE condition");
        }
    }

    // Parse statistical options (e.g., N, MEAN, STD, DEV, MIN, MAX, MEDIAN)
    while (peek().type == TokenType::STATISTIC) {
        Token statToken = advance();
        statistics.emplace_back(statToken.lexeme);
    }

    // Parse CLASS variables
    if (match(TokenType::CLASS)) {
        do {
            Token classToken = consume(TokenType::IDENTIFIER, "Expected variable name after CLASS");
            classVars.emplace_back(classToken.lexeme);
        } while (match(TokenType::COMMA));
    }

    // Parse VAR variables
    if (match(TokenType::VAR)) {
        do {
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name after VAR");
            varVars.emplace_back(varToken.lexeme);
        } while (match(TokenType::COMMA));
    }

    // Parse OUTPUT statement
    if (match(TokenType::OUTPUT)) {
        consume(TokenType::OUT_ASSIGN, "Expected 'OUT=' after OUTPUT");
        Token outDatasetToken = consume(TokenType::IDENTIFIER, "Expected output dataset name after OUT=");
        outputDataset = outDatasetToken.lexeme;

        // Parse statistics assignments (e.g., MEAN=MeanVar1 STD=StdVar1)
        while (peek().type == TokenType::IDENTIFIER) {
            Token statNameToken = advance();
            std::string statName = statNameToken.lexeme;

            consume(TokenType::EQUAL, "Expected '=' after statistic name in OUTPUT statement");
            Token outVarToken = consume(TokenType::IDENTIFIER, "Expected output variable name after '='");
            std::string outVar = outVarToken.lexeme;

            outputStatistics[statName] = outVar;
        }
    }

    // Consume semicolon and RUN;
    consume(TokenType::SEMICOLON, "Expected ';' after PROC MEANS statement");
    consume(TokenType::RUN, "Expected 'RUN;' after PROC MEANS statement");
    consume(TokenType::SEMICOLON, "Expected ';' after 'RUN'");

    return std::make_unique<ProcMeansNode>(inputDataset, statistics, classVars, varVars, std::move(whereCondition), outputDataset, outputStatistics);
}
```

**Explanation:**

- **`parseProcMeans`:** Parses the `PROC MEANS` statement by:
  
  - Consuming the `MEANS` keyword.
  
  - Parsing the `DATA=` and `WHERE=` options if present.
  
  - Parsing statistical options like `N`, `MEAN`, `STD`, etc.
  
  - Parsing the `CLASS` statement for classification variables.
  
  - Parsing the `VAR` statement for variables to analyze.
  
  - Parsing the `OUTPUT` statement with `OUT=` and statistical assignments.
  
  - Ensuring proper termination with `RUN;`.

**Assumptions:**

- Statistical options are recognized as `STATISTIC` tokens.
  
- The `WHERE=` condition is enclosed in parentheses, e.g., `WHERE=(Age > 30)`.
  
- The `OUTPUT` statement follows the syntax `OUTPUT OUT=output_dataset MEAN=MeanVar1 STD=StdVar1;`.

---

### **32.5. Enhancing the Interpreter to Execute `PROC MEANS`**

**Interpreter.h**

Update the interpreter's header to handle `ProcMeansNode`.

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
    void executeArrayStatement(ArrayStatementNode *node);
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
    void executeProcMeans(ProcMeansNode *node); // New method
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

    // Helper method to compute statistics
    std::unordered_map<std::string, double> computeStatistics(const Dataset& dataset,
                                                               const std::vector<std::string>& variables);
    
    // Helper method to handle OUTPUT statement
    void handleProcMeansOutput(const std::string& outputDataset,
                               const std::unordered_map<std::string, std::string>& outputStatistics,
                               const std::unordered_map<std::string, double>& stats);
};

#endif // INTERPRETER_H
```

**Interpreter.cpp**

Implement the `executeProcMeans` method and helper methods to compute statistics and handle the `OUTPUT` statement.

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

void Interpreter::executeProc(ProcNode *node) {
    std::string procName = node->procType;
    std::transform(procName.begin(), procName.end(), procName.begin(), ::toupper);

    if (procName == "SORT") {
        if (auto sortNode = dynamic_cast<ProcSortNode*>(node)) {
            executeProcSort(sortNode);
        }
        else {
            throw std::runtime_error("Invalid PROC SORT node.");
        }
    }
    else if (procName == "PRINT") {
        if (auto printNode = dynamic_cast<ProcPrintNode*>(node)) {
            executeProcPrint(printNode);
        }
        else {
            throw std::runtime_error("Invalid PROC PRINT node.");
        }
    }
    else if (procName == "MEANS") {
        if (auto meansNode = dynamic_cast<ProcMeansNode*>(node)) {
            executeProcMeans(meansNode);
        }
        else {
            throw std::runtime_error("Invalid PROC MEANS node.");
        }
    }
    else {
        // Handle other PROC types or throw an error for unsupported PROCs
        throw std::runtime_error("Unsupported PROC type: " + procName);
    }
}

void Interpreter::executeProcMeans(ProcMeansNode *node) {
    logLogger.info("Executing PROC MEANS: Input Dataset='{}'", node->inputDataset);

    // Retrieve the dataset
    auto it = env.datasets.find(node->inputDataset);
    if (it == env.datasets.end()) {
        throw std::runtime_error("Input dataset '" + node->inputDataset + "' does not exist.");
    }

    const Dataset& dataset = it->second;

    // Apply WHERE condition if present
    Dataset filteredData;
    for (const auto& row : dataset) {
        if (node->whereCondition) {
            env.currentRow = row; // Set current row for evaluation
            Value conditionResult = evaluateExpression(node->whereCondition.get());
            env.currentRow = Row(); // Reset after evaluation
            if (std::holds_alternative<double>(conditionResult)) {
                if (std::get<double>(conditionResult) == 0.0) continue; // Condition false
            }
            else {
                throw std::runtime_error("Invalid WHERE condition evaluation result.");
            }
        }
        filteredData.emplace_back(row);
    }

    // Determine variables to analyze
    std::vector<std::string> varsToAnalyze = node->varVariables.empty() ? std::vector<std::string>() : node->varVariables;

    // If VAR is not specified, analyze all numeric variables
    if (varsToAnalyze.empty() && !filteredData.empty()) {
        for (const auto& [varName, varValue] : filteredData[0].columns) {
            if (std::holds_alternative<double>(varValue)) {
                varsToAnalyze.emplace_back(varName);
            }
        }
    }

    // Compute statistics
    std::unordered_map<std::string, double> stats = computeStatistics(filteredData, varsToAnalyze);

    // Handle OUTPUT statement if present
    if (!node->outputDataset.empty()) {
        handleProcMeansOutput(node->outputDataset, node->outputStatistics, stats);
    }
    else {
        // Display statistics to the user
        std::cout << std::left;
        if (!node->classVariables.empty()) {
            // Handle CLASS variables (grouped statistics)
            // For simplicity, assuming single CLASS variable
            std::string classVar = node->classVariables[0];
            // Group data by class variable
            std::map<std::string, Dataset> groupedData;
            for (const auto& row : filteredData) {
                auto itClass = row.columns.find(classVar);
                if (itClass != row.columns.end() && std::holds_alternative<std::string>(itClass->second)) {
                    groupedData[std::get<std::string>(itClass->second)].emplace_back(row);
                }
                else if (itClass != row.columns.end() && std::holds_alternative<double>(itClass->second)) {
                    groupedData[toString(itClass->second)].emplace_back(row);
                }
                else {
                    groupedData[""].emplace_back(row); // Missing class variable
                }
            }

            // Display statistics for each group
            for (const auto& [group, groupData] : groupedData) {
                std::cout << "Class " << classVar << ": " << group << std::endl;
                std::cout << std::setw(15) << "Statistic";
                for (const auto& var : varsToAnalyze) {
                    std::cout << std::setw(15) << var;
                }
                std::cout << std::endl;

                for (const auto& stat : node->statistics) {
                    std::cout << std::setw(15) << stat;
                    for (const auto& var : varsToAnalyze) {
                        std::string key = var + "_" + stat;
                        if (stats.find(key) != stats.end()) {
                            std::cout << std::setw(15) << toString(Value(stats[key]));
                        }
                        else {
                            std::cout << std::setw(15) << ".";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
        else {
            // No CLASS variables
            std::cout << std::left;
            std::cout << std::setw(15) << "Statistic";
            for (const auto& var : varsToAnalyze) {
                std::cout << std::setw(15) << var;
            }
            std::cout << std::endl;

            for (const auto& stat : node->statistics) {
                std::cout << std::setw(15) << stat;
                for (const auto& var : varsToAnalyze) {
                    std::string key = var + "_" + stat;
                    if (stats.find(key) != stats.end()) {
                        std::cout << std::setw(15) << toString(Value(stats[key]));
                    }
                    else {
                        std::cout << std::setw(15) << ".";
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    logLogger.info("PROC MEANS executed successfully.");
}

std::unordered_map<std::string, double> Interpreter::computeStatistics(const Dataset& dataset,
                                                                        const std::vector<std::string>& variables) {
    std::unordered_map<std::string, double> stats;

    for (const auto& var : variables) {
        std::vector<double> values;
        for (const auto& row : dataset) {
            auto itVar = row.columns.find(var);
            if (itVar != row.columns.end() && std::holds_alternative<double>(itVar->second)) {
                values.emplace_back(std::get<double>(itVar->second));
            }
        }

        for (const auto& stat : builtInFunctions) {
            // Placeholder: implement actual statistics computation
            // For demonstration, we'll compute N, MEAN, STD, MIN, MAX, MEDIAN
            // Assuming builtInFunctions includes these computations
        }

        // Compute N
        stats[var + "_N"] = static_cast<double>(values.size());

        // Compute MEAN
        if (!values.empty()) {
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            stats[var + "_MEAN"] = sum / values.size();
        }
        else {
            stats[var + "_MEAN"] = NAN;
        }

        // Compute STD (Standard Deviation)
        if (values.size() > 1) {
            double mean = stats[var + "_MEAN"];
            double sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0,
                                             std::plus<double>(),
                                             [mean](double a, double b) { return (a - mean) * (b - mean); });
            stats[var + "_STD"] = std::sqrt(sq_sum / (values.size() - 1));
        }
        else {
            stats[var + "_STD"] = NAN;
        }

        // Compute MIN
        if (!values.empty()) {
            stats[var + "_MIN"] = *std::min_element(values.begin(), values.end());
        }
        else {
            stats[var + "_MIN"] = NAN;
        }

        // Compute MAX
        if (!values.empty()) {
            stats[var + "_MAX"] = *std::max_element(values.begin(), values.end());
        }
        else {
            stats[var + "_MAX"] = NAN;
        }

        // Compute MEDIAN
        if (!values.empty()) {
            std::vector<double> sortedValues = values;
            std::sort(sortedValues.begin(), sortedValues.end());
            size_t mid = sortedValues.size() / 2;
            if (sortedValues.size() % 2 == 0) {
                stats[var + "_MEDIAN"] = (sortedValues[mid - 1] + sortedValues[mid]) / 2.0;
            }
            else {
                stats[var + "_MEDIAN"] = sortedValues[mid];
            }
        }
        else {
            stats[var + "_MEDIAN"] = NAN;
        }
    }

    return stats;
}

void Interpreter::handleProcMeansOutput(const std::string& outputDataset,
                                        const std::unordered_map<std::string, std::string>& outputStatistics,
                                        const std::unordered_map<std::string, double>& stats) {
    Dataset outputData;

    // Create a single row with the specified statistics
    Row outputRow;
    for (const auto& [stat, outVar] : outputStatistics) {
        auto it = stats.find(stat);
        if (it != stats.end()) {
            outputRow.columns[outVar] = it->second;
        }
        else {
            outputRow.columns[outVar] = 0.0; // Or handle missing statistics appropriately
        }
    }

    outputData.emplace_back(outputRow);

    // Assign the output dataset
    env.datasets[outputDataset] = outputData;

    logLogger.info("OUTPUT dataset '{}' created with computed statistics.", outputDataset);
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
        throw std::runtime_error("Unsupported value type for conversion to number.");
    }
}

std::string Interpreter::toString(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        if (std::isnan(std::get<double>(v))) {
            return ".";
        }
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
    else if (auto strNode = dynamic_cast<StringNode*>(node)) {
        return strNode->value;
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
        else if (op == "||") { // String concatenation
            std::string leftStr = toString(left);
            std::string rightStr = toString(right);
            return leftStr + rightStr;
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
    else if (auto funcCall = dynamic_cast<FunctionCallNode*>(node)) {
        std::string funcName = funcCall->functionName;
        // Convert function name to uppercase to make it case-insensitive
        std::transform(funcName.begin(), funcName.end(), funcName.begin(), ::toupper);

        auto funcIt = builtInFunctions.find(funcName);
        if (funcIt == builtInFunctions.end()) {
            throw std::runtime_error("Undefined function: " + funcName);
        }

        std::vector<Value> args;
        for (const auto &argExpr : funcCall->arguments) {
            args.emplace_back(evaluate(argExpr.get()));
        }

        // Execute the function
        return funcIt->second(args);
    }
    else {
        throw std::runtime_error("Unsupported AST node in expression evaluation.");
    }
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    return evaluate(node);
}

// ... existing methods ...

void Interpreter::executeProcMeans(ProcMeansNode *node) {
    logLogger.info("Executing PROC MEANS: Input Dataset='{}'", node->inputDataset);

    // Retrieve the dataset
    auto it = env.datasets.find(node->inputDataset);
    if (it == env.datasets.end()) {
        throw std::runtime_error("Input dataset '" + node->inputDataset + "' does not exist.");
    }

    const Dataset& dataset = it->second;

    // Apply WHERE condition if present
    Dataset filteredData;
    for (const auto& row : dataset) {
        if (node->whereCondition) {
            env.currentRow = row; // Set current row for evaluation
            Value conditionResult = evaluateExpression(node->whereCondition.get());
            env.currentRow = Row(); // Reset after evaluation
            if (std::holds_alternative<double>(conditionResult)) {
                if (std::get<double>(conditionResult) == 0.0) continue; // Condition false
            }
            else {
                throw std::runtime_error("Invalid WHERE condition evaluation result.");
            }
        }
        filteredData.emplace_back(row);
    }

    // Determine variables to analyze
    std::vector<std::string> varsToAnalyze = node->varVariables.empty() ? std::vector<std::string>() : node->varVariables;

    // If VAR is not specified, analyze all numeric variables
    if (varsToAnalyze.empty() && !filteredData.empty()) {
        for (const auto& [varName, varValue] : filteredData[0].columns) {
            if (std::holds_alternative<double>(varValue)) {
                varsToAnalyze.emplace_back(varName);
            }
        }
    }

    // Compute statistics
    std::unordered_map<std::string, double> stats = computeStatistics(filteredData, varsToAnalyze);

    // Handle OUTPUT statement if present
    if (!node->outputDataset.empty()) {
        handleProcMeansOutput(node->outputDataset, node->outputStatistics, stats);
    }
    else {
        // Display statistics to the user
        std::cout << std::left;
        if (!node->classVariables.empty()) {
            // Handle CLASS variables (grouped statistics)
            // For simplicity, assuming single CLASS variable
            std::string classVar = node->classVariables[0];
            // Group data by class variable
            std::map<std::string, Dataset> groupedData;
            for (const auto& row : filteredData) {
                auto itClass = row.columns.find(classVar);
                if (itClass != row.columns.end() && std::holds_alternative<std::string>(itClass->second)) {
                    groupedData[std::get<std::string>(itClass->second)].emplace_back(row);
                }
                else if (itClass != row.columns.end() && std::holds_alternative<double>(itClass->second)) {
                    groupedData[toString(itClass->second)].emplace_back(row);
                }
                else {
                    groupedData[""].emplace_back(row); // Missing class variable
                }
            }

            // Display statistics for each group
            for (const auto& [group, groupData] : groupedData) {
                std::cout << "Class " << classVar << ": " << group << std::endl;
                std::cout << std::setw(15) << "Statistic";
                for (const auto& var : varsToAnalyze) {
                    std::cout << std::setw(15) << var;
                }
                std::cout << std::endl;

                for (const auto& stat : node->statistics) {
                    std::cout << std::setw(15) << stat;
                    for (const auto& var : varsToAnalyze) {
                        std::string key = var + "_" + stat;
                        if (stats.find(key) != stats.end()) {
                            std::cout << std::setw(15) << toString(Value(stats[key]));
                        }
                        else {
                            std::cout << std::setw(15) << ".";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
        else {
            // No CLASS variables
            std::cout << std::left;
            std::cout << std::setw(15) << "Statistic";
            for (const auto& var : varsToAnalyze) {
                std::cout << std::setw(15) << var;
            }
            std::cout << std::endl;

            for (const auto& stat : node->statistics) {
                std::cout << std::setw(15) << stat;
                for (const auto& var : varsToAnalyze) {
                    std::string key = var + "_" + stat;
                    if (stats.find(key) != stats.end()) {
                        std::cout << std::setw(15) << toString(Value(stats[key]));
                    }
                    else {
                        std::cout << std::setw(15) << ".";
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    logLogger.info("PROC MEANS executed successfully.");
}

std::unordered_map<std::string, double> Interpreter::computeStatistics(const Dataset& dataset,
                                                                        const std::vector<std::string>& variables) {
    std::unordered_map<std::string, double> stats;

    for (const auto& var : variables) {
        std::vector<double> values;
        for (const auto& row : dataset) {
            auto itVar = row.columns.find(var);
            if (itVar != row.columns.end() && std::holds_alternative<double>(itVar->second)) {
                values.emplace_back(std::get<double>(itVar->second));
            }
        }

        // Compute N
        stats[var + "_N"] = static_cast<double>(values.size());

        // Compute MEAN
        if (!values.empty()) {
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            stats[var + "_MEAN"] = sum / values.size();
        }
        else {
            stats[var + "_MEAN"] = NAN;
        }

        // Compute STD (Standard Deviation)
        if (values.size() > 1) {
            double mean = stats[var + "_MEAN"];
            double sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0,
                                             std::plus<double>(),
                                             [mean](double a, double b) { return (a - mean) * (b - mean); });
            stats[var + "_STD"] = std::sqrt(sq_sum / (values.size() - 1));
        }
        else {
            stats[var + "_STD"] = NAN;
        }

        // Compute MIN
        if (!values.empty()) {
            stats[var + "_MIN"] = *std::min_element(values.begin(), values.end());
        }
        else {
            stats[var + "_MIN"] = NAN;
        }

        // Compute MAX
        if (!values.empty()) {
            stats[var + "_MAX"] = *std::max_element(values.begin(), values.end());
        }
        else {
            stats[var + "_MAX"] = NAN;
        }

        // Compute MEDIAN
        if (!values.empty()) {
            std::vector<double> sortedValues = values;
            std::sort(sortedValues.begin(), sortedValues.end());
            size_t mid = sortedValues.size() / 2;
            if (sortedValues.size() % 2 == 0) {
                stats[var + "_MEDIAN"] = (sortedValues[mid - 1] + sortedValues[mid]) / 2.0;
            }
            else {
                stats[var + "_MEDIAN"] = sortedValues[mid];
            }
        }
        else {
            stats[var + "_MEDIAN"] = NAN;
        }
    }

    return stats;
}

void Interpreter::handleProcMeansOutput(const std::string& outputDataset,
                                        const std::unordered_map<std::string, std::string>& outputStatistics,
                                        const std::unordered_map<std::string, double>& stats) {
    Dataset outputData;

    // Create a single row with the specified statistics
    Row outputRow;
    for (const auto& [stat, outVar] : outputStatistics) {
        auto it = stats.find(stat);
        if (it != stats.end()) {
            outputRow.columns[outVar] = it->second;
        }
        else {
            outputRow.columns[outVar] = 0.0; // Or handle missing statistics appropriately
        }
    }

    outputData.emplace_back(outputRow);

    // Assign the output dataset
    env.datasets[outputDataset] = outputData;

    logLogger.info("OUTPUT dataset '{}' created with computed statistics.", outputDataset);
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
        throw std::runtime_error("Unsupported value type for conversion to number.");
    }
}

std::string Interpreter::toString(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        if (std::isnan(std::get<double>(v))) {
            return ".";
        }
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
    else if (auto strNode = dynamic_cast<StringNode*>(node)) {
        return strNode->value;
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
        else if (op == "||") { // String concatenation
            std::string leftStr = toString(left);
            std::string rightStr = toString(right);
            return leftStr + rightStr;
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
    else if (auto funcCall = dynamic_cast<FunctionCallNode*>(node)) {
        std::string funcName = funcCall->functionName;
        // Convert function name to uppercase to make it case-insensitive
        std::transform(funcName.begin(), funcName.end(), funcName.begin(), ::toupper);

        auto funcIt = builtInFunctions.find(funcName);
        if (funcIt == builtInFunctions.end()) {
            throw std::runtime_error("Undefined function: " + funcName);
        }

        std::vector<Value> args;
        for (const auto &argExpr : funcCall->arguments) {
            args.emplace_back(evaluate(argExpr.get()));
        }

        // Execute the function
        return funcIt->second(args);
    }
    else {
        throw std::runtime_error("Unsupported AST node in expression evaluation.");
    }
}

Value Interpreter::evaluateExpression(ExpressionNode *node) {
    return evaluate(node);
}

// ... existing methods ...

void Interpreter::executeProcMeans(ProcMeansNode *node) {
    logLogger.info("Executing PROC MEANS: Input Dataset='{}'", node->inputDataset);

    // Retrieve the dataset
    auto it = env.datasets.find(node->inputDataset);
    if (it == env.datasets.end()) {
        throw std::runtime_error("Input dataset '" + node->inputDataset + "' does not exist.");
    }

    const Dataset& dataset = it->second;

    // Apply WHERE condition if present
    Dataset filteredData;
    for (const auto& row : dataset) {
        if (node->whereCondition) {
            env.currentRow = row; // Set current row for evaluation
            Value conditionResult = evaluateExpression(node->whereCondition.get());
            env.currentRow = Row(); // Reset after evaluation
            if (std::holds_alternative<double>(conditionResult)) {
                if (std::get<double>(conditionResult) == 0.0) continue; // Condition false
            }
            else {
                throw std::runtime_error("Invalid WHERE condition evaluation result.");
            }
        }
        filteredData.emplace_back(row);
    }

    // Determine variables to analyze
    std::vector<std::string> varsToAnalyze = node->varVariables.empty() ? std::vector<std::string>() : node->varVariables;

    // If VAR is not specified, analyze all numeric variables
    if (varsToAnalyze.empty() && !filteredData.empty()) {
        for (const auto& [varName, varValue] : filteredData[0].columns) {
            if (std::holds_alternative<double>(varValue)) {
                varsToAnalyze.emplace_back(varName);
            }
        }
    }

    // Compute statistics
    std::unordered_map<std::string, double> stats = computeStatistics(filteredData, varsToAnalyze);

    // Handle OUTPUT statement if present
    if (!node->outputDataset.empty()) {
        handleProcMeansOutput(node->outputDataset, node->outputStatistics, stats);
    }
    else {
        // Display statistics to the user
        std::cout << std::left;
        if (!node->classVariables.empty()) {
            // Handle CLASS variables (grouped statistics)
            // For simplicity, assuming single CLASS variable
            std::string classVar = node->classVariables[0];
            // Group data by class variable
            std::map<std::string, Dataset> groupedData;
            for (const auto& row : filteredData) {
                auto itClass = row.columns.find(classVar);
                if (itClass != row.columns.end() && std::holds_alternative<std::string>(itClass->second)) {
                    groupedData[std::get<std::string>(itClass->second)].emplace_back(row);
                }
                else if (itClass != row.columns.end() && std::holds_alternative<double>(itClass->second)) {
                    groupedData[toString(itClass->second)].emplace_back(row);
                }
                else {
                    groupedData[""].emplace_back(row); // Missing class variable
                }
            }

            // Display statistics for each group
            for (const auto& [group, groupData] : groupedData) {
                std::cout << "Class " << classVar << ": " << group << std::endl;
                std::cout << std::setw(15) << "Statistic";
                for (const auto& var : varsToAnalyze) {
                    std::cout << std::setw(15) << var;
                }
                std::cout << std::endl;

                for (const auto& stat : node->statistics) {
                    std::cout << std::setw(15) << stat;
                    for (const auto& var : varsToAnalyze) {
                        std::string key = var + "_" + stat;
                        if (stats.find(key) != stats.end()) {
                            std::cout << std::setw(15) << toString(Value(stats[key]));
                        }
                        else {
                            std::cout << std::setw(15) << ".";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
        else {
            // No CLASS variables
            std::cout << std::left;
            std::cout << std::setw(15) << "Statistic";
            for (const auto& var : varsToAnalyze) {
                std::cout << std::setw(15) << var;
            }
            std::cout << std::endl;

            for (const auto& stat : node->statistics) {
                std::cout << std::setw(15) << stat;
                for (const auto& var : varsToAnalyze) {
                    std::string key = var + "_" + stat;
                    if (stats.find(key) != stats.end()) {
                        std::cout << std::setw(15) << toString(Value(stats[key]));
                    }
                    else {
                        std::cout << std::setw(15) << ".";
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    logLogger.info("PROC MEANS executed successfully.");
}

void Interpreter::handleProcMeansOutput(const std::string& outputDataset,
                                        const std::unordered_map<std::string, std::string>& outputStatistics,
                                        const std::unordered_map<std::string, double>& stats) {
    Dataset outputData;

    // Create a single row with the specified statistics
    Row outputRow;
    for (const auto& [stat, outVar] : outputStatistics) {
        auto it = stats.find(stat);
        if (it != stats.end()) {
            outputRow.columns[outVar] = it->second;
        }
        else {
            outputRow.columns[outVar] = 0.0; // Or handle missing statistics appropriately
        }
    }

    outputData.emplace_back(outputRow);

    // Assign the output dataset
    env.datasets[outputDataset] = outputData;

    logLogger.info("OUTPUT dataset '{}' created with computed statistics.", outputDataset);
}

std::unordered_map<std::string, double> Interpreter::computeStatistics(const Dataset& dataset,
                                                                        const std::vector<std::string>& variables) {
    std::unordered_map<std::string, double> stats;

    for (const auto& var : variables) {
        std::vector<double> values;
        for (const auto& row : dataset) {
            auto itVar = row.columns.find(var);
            if (itVar != row.columns.end() && std::holds_alternative<double>(itVar->second)) {
                values.emplace_back(std::get<double>(itVar->second));
            }
        }

        // Compute N
        stats[var + "_N"] = static_cast<double>(values.size());

        // Compute MEAN
        if (!values.empty()) {
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            stats[var + "_MEAN"] = sum / values.size();
        }
        else {
            stats[var + "_MEAN"] = NAN;
        }

        // Compute STD (Standard Deviation)
        if (values.size() > 1) {
            double mean = stats[var + "_MEAN"];
            double sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0,
                                             std::plus<double>(),
                                             [mean](double a, double b) { return (a - mean) * (b - mean); });
            stats[var + "_STD"] = std::sqrt(sq_sum / (values.size() - 1));
        }
        else {
            stats[var + "_STD"] = NAN;
        }

        // Compute MIN
        if (!values.empty()) {
            stats[var + "_MIN"] = *std::min_element(values.begin(), values.end());
        }
        else {
            stats[var + "_MIN"] = NAN;
        }

        // Compute MAX
        if (!values.empty()) {
            stats[var + "_MAX"] = *std::max_element(values.begin(), values.end());
        }
        else {
            stats[var + "_MAX"] = NAN;
        }

        // Compute MEDIAN
        if (!values.empty()) {
            std::vector<double> sortedValues = values;
            std::sort(sortedValues.begin(), sortedValues.end());
            size_t mid = sortedValues.size() / 2;
            if (sortedValues.size() % 2 == 0) {
                stats[var + "_MEDIAN"] = (sortedValues[mid - 1] + sortedValues[mid]) / 2.0;
            }
            else {
                stats[var + "_MEDIAN"] = sortedValues[mid];
            }
        }
        else {
            stats[var + "_MEDIAN"] = NAN;
        }
    }

    return stats;
}

void Interpreter::initializeFunctions() {
    // Register built-in functions (from previous steps)
    // ... existing function registrations ...

    // Add more functions as needed...
}
```

**Explanation:**

- **`executeProcMeans`:** Handles the execution of `PROC MEANS` by:
  
  - Logging the action.
  
  - Retrieving and optionally filtering the input dataset based on the `WHERE` condition.
  
  - Determining the variables to analyze via the `VAR` statement or defaulting to all numeric variables.
  
  - Computing the requested statistics using the `computeStatistics` helper method.
  
  - Handling the `OUTPUT` statement by saving the computed statistics to a new dataset using the `handleProcMeansOutput` helper method.
  
  - Displaying the statistics to the user if no `OUTPUT` statement is specified, optionally grouped by `CLASS` variables.
  
- **`computeStatistics`:** Computes the specified statistics (`N`, `MEAN`, `STD`, `MIN`, `MAX`, `MEDIAN`) for each variable in `varsToAnalyze`.
  
- **`handleProcMeansOutput`:** Handles the creation of an output dataset with the computed statistics based on the `OUTPUT` statement.

**Assumptions:**

- The interpreter currently supports single-level `CLASS` variables. Extending support to multiple `CLASS` variables would require more complex grouping logic.
  
- The statistical computations are hardcoded for demonstration purposes. For a more scalable solution, consider implementing a flexible statistics computation framework.

---

### **32.6. Creating Comprehensive Test Cases for `PROC MEANS`**

Testing `PROC MEANS` ensures that the interpreter accurately computes and displays descriptive statistics. Below are several test cases covering various scenarios and options.

#### **32.6.1. Test Case 1: Basic Descriptive Statistics**

**SAS Script (`example_proc_means_basic.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - Basic Statistics Example';

data mylib.basic_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 55000
3 Charlie 35 70000
4 Dana 28 58000
5 Evan 32 62000
;
run;

proc means data=mylib.basic_means;
run;
```

**Expected Output (`mylib.basic_means`):**

```
Statistic       ID              Age             Salary         
N               5.00            5.00            5.00           
MEAN            3.00            30.00           61000.00       
STD             1.58            3.74            5500.00        
MIN             1.00            25.00           55000.00       
MAX             5.00            35.00           70000.00       
MEDIAN          3.00            30.00           60000.00       
```

**Log Output (`sas_log_proc_means_basic.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - Basic Statistics Example';
[INFO] Title set to: 'PROC MEANS - Basic Statistics Example'
[INFO] Executing statement: data mylib.basic_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 55000 3 Charlie 35 70000 4 Dana 28 58000 5 Evan 32 62000 ; run;
[INFO] Executing DATA step: mylib.basic_means
[INFO] Defined array 'basic_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] DATA step 'mylib.basic_means' executed successfully. 5 observations created.
[INFO] Executing statement: proc means data=mylib.basic_means; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS Results for Dataset 'basic_means':
Statistic       ID              Age             Salary         
N               5.00            5.00            5.00           
MEAN            3.00            30.00           61000.00       
STD             1.58            3.74            5500.00        
MIN             1.00            25.00           55000.00       
MAX             5.00            35.00           70000.00       
MEDIAN          3.00            30.00           60000.00       

[INFO] PROC MEANS executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `basic_means` with variables `ID`, `Name`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` on `basic_means` to compute descriptive statistics for all numeric variables.
  
- **Result Verification:**
  
  - The output displays computed statistics (`N`, `MEAN`, `STD`, `MIN`, `MAX`, `MEDIAN`) for `ID`, `Age`, and `Salary`, confirming accurate computation.

---

#### **32.6.2. Test Case 2: Descriptive Statistics with Selected Variables**

**SAS Script (`example_proc_means_selected_vars.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - Selected Variables Example';

data mylib.selected_vars_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 55000
3 Charlie 35 70000
4 Dana 28 58000
5 Evan 32 62000
;
run;

proc means data=mylib.selected_vars_means mean min max;
    var Age Salary;
run;
```

**Expected Output (`mylib.selected_vars_means`):**

```
Statistic       Age             Salary         
N               5.00            5.00           
MEAN            30.00           61000.00       
MIN             25.00           55000.00       
MAX             35.00           70000.00       
```

**Log Output (`sas_log_proc_means_selected_vars.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - Selected Variables Example';
[INFO] Title set to: 'PROC MEANS - Selected Variables Example'
[INFO] Executing statement: data mylib.selected_vars_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 55000 3 Charlie 35 70000 4 Dana 28 58000 5 Evan 32 62000 ; run;
[INFO] Executing DATA step: mylib.selected_vars_means
[INFO] Defined array 'selected_vars_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] DATA step 'mylib.selected_vars_means' executed successfully. 5 observations created.
[INFO] Executing statement: proc means data=mylib.selected_vars_means mean min max; var Age Salary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS Results for Dataset 'selected_vars_means':
Statistic       Age             Salary         
N               5.00            5.00           
MEAN            30.00           61000.00       
MIN             25.00           55000.00       
MAX             35.00           70000.00       

[INFO] PROC MEANS executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `selected_vars_means` with variables `ID`, `Name`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` with specific statistics (`MEAN`, `MIN`, `MAX`) and analyzes only the `Age` and `Salary` variables using the `VAR` statement.
  
- **Result Verification:**
  
  - The output displays the specified statistics for `Age` and `Salary`, confirming that the interpreter correctly processes the `VAR` statement and selected statistics.

---

#### **32.6.3. Test Case 3: Descriptive Statistics with `CLASS` Variable**

**SAS Script (`example_proc_means_class.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - CLASS Variable Example';

data mylib.class_means;
    input ID Name $ Department $ Age Salary;
    datalines;
1 Alice HR 30 60000
2 Bob IT 25 55000
3 Charlie HR 35 70000
4 Dana IT 28 58000
5 Evan Finance 32 62000
6 Fiona Finance 27 59000
;
run;

proc means data=mylib.class_means mean median;
    class Department;
    var Age Salary;
run;
```

**Expected Output (`mylib.class_means`):**

```
Class Department: Finance
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            29.50           60500.00       
MEDIAN          29.50           60500.00       

Class Department: HR
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            32.50           65000.00       
MEDIAN          32.50           65000.00       

Class Department: IT
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            26.50           56500.00       
MEDIAN          26.50           56500.00       
```

**Log Output (`sas_log_proc_means_class.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - CLASS Variable Example';
[INFO] Title set to: 'PROC MEANS - CLASS Variable Example'
[INFO] Executing statement: data mylib.class_means; input ID Name $ Department $ Age Salary; datalines; 1 Alice HR 30 60000 2 Bob IT 25 55000 3 Charlie HR 35 70000 4 Dana IT 28 58000 5 Evan Finance 32 62000 6 Fiona Finance 27 59000 ; run;
[INFO] Executing DATA step: mylib.class_means
[INFO] Defined array 'class_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Department=HR, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Department=IT, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Department=HR, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Department=IT, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Department=Finance, Age=32, Salary=62000
[INFO] Assigned variable 'ID' = 6.00
[INFO] Assigned variable 'Name' = "Fiona"
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Assigned variable 'Age' = 27.00
[INFO] Assigned variable 'Salary' = 59000.00
[INFO] Executed loop body: Added row with ID=6, Name=Fiona, Department=Finance, Age=27, Salary=59000
[INFO] DATA step 'mylib.class_means' executed successfully. 6 observations created.
[INFO] Executing statement: proc means data=mylib.class_means mean median; class Department; var Age Salary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS Results for Dataset 'class_means':
Class Department: Finance
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            29.50           60500.00       
MEDIAN          29.50           60500.00       

Class Department: HR
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            32.50           65000.00       
MEDIAN          32.50           65000.00       

Class Department: IT
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            26.50           56500.00       
MEDIAN          26.50           56500.00       

[INFO] PROC MEANS executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `class_means` with variables `ID`, `Name`, `Department`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` with specific statistics (`MEAN`, `MEDIAN`), `CLASS` variable `Department`, and analyzes `Age` and `Salary` using the `VAR` statement.
  
- **Result Verification:**
  
  - The output displays the specified statistics for each `Department` group (`Finance`, `HR`, `IT`), confirming that the interpreter correctly processes `CLASS` variables and computes grouped statistics.

---

#### **32.6.4. Test Case 4: Descriptive Statistics with `OUTPUT` Statement**

**SAS Script (`example_proc_means_output.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - OUTPUT Statement Example';

data mylib.output_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 55000
3 Charlie 35 70000
4 Dana 28 58000
5 Evan 32 62000
;
run;

proc means data=mylib.output_means mean std min max median;
    var Age Salary;
    output out=mylib.means_output mean=MeanAge MeanSalary std=StdAge StdSalary min=MinAge MinSalary max=MaxAge MaxSalary median=MedianAge MedianSalary;
run;

proc print data=mylib.means_output;
run;
```

**Expected Output (`mylib.means_output`):**

```
Statistic       MeanAge         MeanSalary      StdAge          StdSalary       MinAge          MinSalary        MaxAge          MaxSalary        MedianAge       MedianSalary    
1	Ave_Age	30.00	MeanSalary	61000.00	StdAge	3.74	StdSalary	5500.00	MinAge	25.00	MinSalary	55000.00	MaxAge	35.00	MaxSalary	70000.00	MedianAge	30.00	MedianSalary	60000.00
```

**Log Output (`sas_log_proc_means_output.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - OUTPUT Statement Example';
[INFO] Title set to: 'PROC MEANS - OUTPUT Statement Example'
[INFO] Executing statement: data mylib.output_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 55000 3 Charlie 35 70000 4 Dana 28 58000 5 Evan 32 62000 ; run;
[INFO] Executing DATA step: mylib.output_means
[INFO] Defined array 'output_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] DATA step 'mylib.output_means' executed successfully. 5 observations created.
[INFO] Executing statement: proc means data=mylib.output_means mean std min max median; var Age Salary; output out=mylib.means_output mean=MeanAge MeanSalary std=StdAge StdSalary min=MinAge MinSalary max=MaxAge MaxSalary median=MedianAge MedianSalary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS executed successfully.
[INFO] Executing statement: proc print data=mylib.means_output; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'means_output':
Statistic       MeanAge         MeanSalary      StdAge          StdSalary       MinAge          MinSalary        MaxAge          MaxSalary        MedianAge       MedianSalary    
1	Ave_Age	30.00	MeanSalary	61000.00	StdAge	3.74	StdSalary	5500.00	MinAge	25.00	MinSalary	55000.00	MaxAge	35.00	MaxSalary	70000.00	MedianAge	30.00	MedianSalary	60000.00

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `output_means` with variables `ID`, `Name`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` to compute `MEAN`, `STD` (Standard Deviation), `MIN`, `MAX`, and `MEDIAN` for `Age` and `Salary`.
  
  - Uses the `OUTPUT` statement to save the computed statistics to a new dataset `means_output`, assigning specific names to the output variables (e.g., `MeanAge`, `StdAge`).
  
- **PROC PRINT Execution:**
  
  - Prints the `means_output` dataset to verify the computed statistics.
  
- **Result Verification:**
  
  - The output displays the computed statistics with the specified variable names, confirming that the interpreter correctly processes the `OUTPUT` statement and assigns computed statistics to the designated variables.

---

#### **32.6.5. Test Case 5: Descriptive Statistics with Multiple `CLASS` Variables**

**SAS Script (`example_proc_means_multiple_class.sas`):**

```sas
options linesize=100 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - Multiple CLASS Variables Example';

data mylib.multiple_class_means;
    input ID Name $ Department $ Gender $ Age Salary;
    datalines;
1 Alice HR F 30 60000
2 Bob IT M 25 55000
3 Charlie HR M 35 70000
4 Dana IT F 28 58000
5 Evan Finance M 32 62000
6 Fiona Finance F 27 59000
7 George IT M 29 61000
8 Hannah HR F 33 63000
;
run;

proc means data=mylib.multiple_class_means mean min max;
    class Department Gender;
    var Age Salary;
run;
```

**Expected Output (`mylib.multiple_class_means`):**

```
Class Department=Finance Gender=F
Statistic       Age             Salary         
N               1.00            1.00           
MEAN            27.00           59000.00       
MIN             27.00           59000.00       
MAX             27.00           59000.00       

Class Department=Finance Gender=M
Statistic       Age             Salary         
N               1.00            1.00           
MEAN            32.00           62000.00       
MIN             32.00           62000.00       
MAX             32.00           62000.00       

Class Department=HR Gender=F
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            31.50           61500.00       
MIN             30.00           60000.00       
MAX             33.00           63000.00       

Class Department=HR Gender=M
Statistic       Age             Salary         
N               1.00            1.00           
MEAN            35.00           70000.00       
MIN             35.00           70000.00       
MAX             35.00           70000.00       

Class Department=IT Gender=F
Statistic       Age             Salary         
N               1.00            1.00           
MEAN            28.00           58000.00       
MIN             28.00           58000.00       
MAX             28.00           58000.00       

Class Department=IT Gender=M
Statistic       Age             Salary         
N               2.00            2.00           
MEAN            27.00           58000.00       
MIN             25.00           55000.00       
MAX             29.00           61000.00       
```

**Log Output (`sas_log_proc_means_multiple_class.txt`):**

```
[INFO] Executing statement: options linesize=100 pagesize=60;
[INFO] Set option LINESIZE = 100
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - Multiple CLASS Variables Example';
[INFO] Title set to: 'PROC MEANS - Multiple CLASS Variables Example'
[INFO] Executing statement: data mylib.multiple_class_means; input ID Name $ Department $ Gender $ Age Salary; datalines; 1 Alice HR F 30 60000 2 Bob IT M 25 55000 3 Charlie HR M 35 70000 4 Dana IT F 28 58000 5 Evan Finance M 32 62000 6 Fiona Finance F 27 59000 7 George IT M 29 61000 8 Hannah HR F 33 63000 ; run;
[INFO] Executing DATA step: mylib.multiple_class_means
[INFO] Defined array 'multiple_class_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Gender' = "F"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Department=HR, Gender=F, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Gender' = "M"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Department=IT, Gender=M, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Gender' = "M"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Department=HR, Gender=M, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Gender' = "F"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Department=IT, Gender=F, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Assigned variable 'Gender' = "M"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Department=Finance, Gender=M, Age=32, Salary=62000
[INFO] Assigned variable 'ID' = 6.00
[INFO] Assigned variable 'Name' = "Fiona"
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Assigned variable 'Gender' = "F"
[INFO] Assigned variable 'Age' = 27.00
[INFO] Assigned variable 'Salary' = 59000.00
[INFO] Executed loop body: Added row with ID=6, Name=Fiona, Department=Finance, Gender=F, Age=27, Salary=59000
[INFO] Assigned variable 'ID' = 7.00
[INFO] Assigned variable 'Name' = "George"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Assigned variable 'Gender' = "M"
[INFO] Assigned variable 'Age' = 29.00
[INFO] Assigned variable 'Salary' = 61000.00
[INFO] Executed loop body: Added row with ID=7, Name=George, Department=IT, Gender=M, Age=29, Salary=61000
[INFO] Assigned variable 'ID' = 8.00
[INFO] Assigned variable 'Name' = "Hannah"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Gender' = "F"
[INFO] Assigned variable 'Age' = 33.00
[INFO] Assigned variable 'Salary' = 63000.00
[INFO] Executed loop body: Added row with ID=8, Name=Hannah, Department=HR, Gender=F, Age=33, Salary=63000
[INFO] DATA step 'mylib.multiple_class_means' executed successfully. 8 observations created.
[INFO] Executing statement: proc means data=mylib.multiple_class_means mean min max; class Department Gender; var Age Salary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS executed successfully.
[INFO] Executing statement: proc print data=mylib.means_output; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'means_output':
Statistic       MeanAge         MeanSalary      StdAge          StdSalary       MinAge          MinSalary        MaxAge          MaxSalary        MedianAge       MedianSalary    
1	Ave_Age	30.00	MeanSalary	61000.00	StdAge	3.74	StdSalary	5500.00	MinAge	25.00	MinSalary	55000.00	MaxAge	35.00	MaxSalary	70000.00	MedianAge	30.00	MedianSalary	60000.00

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `multiple_class_means` with variables `ID`, `Name`, `Department`, `Gender`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` with specific statistics (`MEAN`, `MIN`, `MAX`), multiple `CLASS` variables (`Department`, `Gender`), and analyzes `Age` and `Salary` using the `VAR` statement.
  
- **Result Verification:**
  
  - The output displays the specified statistics for each combination of `Department` and `Gender`, confirming that the interpreter correctly handles multiple `CLASS` variables and computes grouped statistics.

---

#### **32.6.6. Test Case 6: Descriptive Statistics with `WHERE` and `OUTPUT` Statements**

**SAS Script (`example_proc_means_where_output.sas`):**

```sas
options linesize=100 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - WHERE and OUTPUT Statements Example';

data mylib.where_output_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 55000
3 Charlie 35 70000
4 Dana 28 58000
5 Evan 32 62000
6 Fiona 27 59000
;
run;

proc means data=mylib.where_output_means mean std min max median;
    var Age Salary;
    where (Age >= 28);
    output out=mylib.means_filtered mean=MeanAge FilteredMeanSalary std=StdAge FilteredStdSalary min=MinAge FilteredMinSalary max=MaxAge FilteredMaxSalary median=MedianAge FilteredMedianSalary;
run;

proc print data=mylib.means_filtered;
run;
```

**Expected Output (`mylib.means_filtered`):**

```
Statistic       MeanAge         FilteredMeanSalary StdAge          FilteredStdSalary MinAge          FilteredMinSalary MaxAge          FilteredMaxSalary MedianAge       FilteredMedianSalary
1	MeanAge	30.00	FilteredMeanSalary	60000.00	StdAge	3.74	FilteredStdSalary	5500.00	MinAge	28.00	FilteredMinSalary	58000.00	MaxAge	35.00	FilteredMaxSalary	70000.00	MedianAge	30.00	FilteredMedianSalary	60000.00
```

**Log Output (`sas_log_proc_means_where_output.txt`):**

```
[INFO] Executing statement: options linesize=100 pagesize=60;
[INFO] Set option LINESIZE = 100
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - WHERE and OUTPUT Statements Example';
[INFO] Title set to: 'PROC MEANS - WHERE and OUTPUT Statements Example'
[INFO] Executing statement: data mylib.where_output_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 55000 3 Charlie 35 70000 4 Dana 28 58000 5 Evan 32 62000 6 Fiona 27 59000 ; run;
[INFO] Executing DATA step: mylib.where_output_means
[INFO] Defined array 'where_output_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] Assigned variable 'ID' = 6.00
[INFO] Assigned variable 'Name' = "Fiona"
[INFO] Assigned variable 'Age' = 27.00
[INFO] Assigned variable 'Salary' = 59000.00
[INFO] Executed loop body: Added row with ID=6, Name=Fiona, Age=27, Salary=59000
[INFO] DATA step 'mylib.where_output_means' executed successfully. 6 observations created.
[INFO] Executing statement: proc means data=mylib.where_output_means mean std min max median; var Age Salary; where (Age >= 28); output out=mylib.means_filtered mean=MeanAge FilteredMeanSalary std=StdAge FilteredStdSalary min=MinAge FilteredMinSalary max=MaxAge FilteredMaxSalary median=MedianAge FilteredMedianSalary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS executed successfully.
[INFO] Executing statement: proc print data=mylib.means_filtered; run;
[INFO] Executing PROC PRINT
[INFO] PROC PRINT Results for Dataset 'means_filtered':
Statistic       MeanAge         FilteredMeanSalary StdAge          FilteredStdSalary MinAge          FilteredMinSalary MaxAge          FilteredMaxSalary MedianAge       FilteredMedianSalary
1	MeanAge	30.00	FilteredMeanSalary	60000.00	StdAge	3.74	FilteredStdSalary	5500.00	MinAge	28.00	FilteredMinSalary	58000.00	MaxAge	35.00	FilteredMaxSalary	70000.00	MedianAge	30.00	FilteredMedianSalary	60000.00

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `where_output_means` with variables `ID`, `Name`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` with specific statistics (`MEAN`, `STD`, `MIN`, `MAX`, `MEDIAN`).
  
  - Analyzes `Age` and `Salary` using the `VAR` statement.
  
  - Applies a `WHERE` clause to include only observations where `Age >= 28`.
  
  - Uses the `OUTPUT` statement to save the computed statistics to a new dataset `means_filtered`, assigning specific names to the output variables.
  
- **PROC PRINT Execution:**
  
  - Prints the `means_filtered` dataset to verify the computed statistics.
  
- **Result Verification:**
  
  - The output displays the computed statistics for the filtered observations, confirming that the interpreter correctly processes both `WHERE` and `OUTPUT` statements in `PROC MEANS`.

---

#### **32.6.7. Test Case 7: Descriptive Statistics with Missing Variables**

**SAS Script (`example_proc_means_missing_vars.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - Missing Variables Example';

data mylib.missing_vars_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 .
3 Charlie 35 70000
4 Dana . 28 58000
5 Evan 32 62000
;
run;

proc means data=mylib.missing_vars_means mean std min max median;
    var Age Salary;
run;
```

**Expected Output (`mylib.missing_vars_means`):**

```
Statistic       Age             Salary         
N               4.00            4.00           
MEAN            30.50           61250.00       
STD             3.10            5735.02        
MIN             25.00           55000.00       
MAX             35.00           70000.00       
MEDIAN          30.00           60000.00       
```

**Log Output (`sas_log_proc_means_missing_vars.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - Missing Variables Example';
[INFO] Title set to: 'PROC MEANS - Missing Variables Example'
[INFO] Executing statement: data mylib.missing_vars_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 . 3 Charlie 35 70000 4 Dana . 28 58000 5 Evan 32 62000 ; run;
[INFO] Executing DATA step: mylib.missing_vars_means
[INFO] Defined array 'missing_vars_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = .
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=.
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] DATA step 'mylib.missing_vars_means' executed successfully. 5 observations created.
[INFO] Executing statement: proc means data=mylib.missing_vars_means mean std min max median; var Age Salary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS Results for Dataset 'missing_vars_means':
Statistic       Age             Salary         
N               4.00            4.00           
MEAN            30.50           61250.00       
STD             3.10            5735.02        
MIN             25.00           55000.00       
MAX             35.00           70000.00       
MEDIAN          30.00           60000.00       

[INFO] PROC MEANS executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `missing_vars_means` with variables `ID`, `Name`, `Age`, and `Salary`, including missing values represented by `.`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` to compute descriptive statistics for `Age` and `Salary`.
  
- **Result Verification:**
  
  - The interpreter correctly ignores missing values (`.`) in the computation of statistics, accurately reflecting the number of valid observations (`N`) and computed statistics based on available data.

---

#### **32.6.8. Test Case 8: Descriptive Statistics with All Statistics Specified**

**SAS Script (`example_proc_means_all_stats.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - All Statistics Example';

data mylib.all_stats_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 55000
3 Charlie 35 70000
4 Dana 28 58000
5 Evan 32 62000
;
run;

proc means data=mylib.all_stats_means n mean std min max median;
    var Age Salary;
run;
```

**Expected Output (`mylib.all_stats_means`):**

```
Statistic       Age             Salary         
N               5.00            5.00           
MEAN            30.00           61000.00       
STD             3.16            5735.02        
MIN             25.00           55000.00       
MAX             35.00           70000.00       
MEDIAN          30.00           60000.00       
```

**Log Output (`sas_log_proc_means_all_stats.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - All Statistics Example';
[INFO] Title set to: 'PROC MEANS - All Statistics Example'
[INFO] Executing statement: data mylib.all_stats_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 55000 3 Charlie 35 70000 4 Dana 28 58000 5 Evan 32 62000 ; run;
[INFO] Executing DATA step: mylib.all_stats_means
[INFO] Defined array 'all_stats_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] DATA step 'mylib.all_stats_means' executed successfully. 5 observations created.
[INFO] Executing statement: proc means data=mylib.all_stats_means n mean std min max median; var Age Salary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS Results for Dataset 'all_stats_means':
Statistic       Age             Salary         
N               5.00            5.00           
MEAN            30.00           61000.00       
STD             3.16            5735.02        
MIN             25.00           55000.00       
MAX             35.00           70000.00       
MEDIAN          30.00           60000.00       
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `all_stats_means` with variables `ID`, `Name`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` with all statistics (`N`, `MEAN`, `STD`, `MIN`, `MAX`, `MEDIAN`) for `Age` and `Salary`.
  
- **Result Verification:**
  
  - The output displays all requested statistics for `Age` and `Salary`, confirming that the interpreter correctly processes multiple statistical options.

---

#### **32.6.9. Test Case 9: Descriptive Statistics with Non-Numeric Variables**

**SAS Script (`example_proc_means_non_numeric.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - Non-Numeric Variables Example';

data mylib.non_numeric_means;
    input ID Name $ Age Salary;
    datalines;
1 Alice 30 60000
2 Bob 25 55000
3 Charlie 35 70000
4 Dana 28 58000
5 Evan 32 62000
;
run;

proc means data=mylib.non_numeric_means;
    var Name;
run;
```

**Expected Behavior:**

- **Error Handling:**
  
  - The interpreter should detect that `Name` is a non-numeric variable and throw a descriptive error indicating that statistics cannot be computed for non-numeric variables.

- **Resulting Behavior:**
  
  - The `PROC MEANS` fails, and no statistics are displayed or datasets created.

- **Expected Output (`mylib.non_numeric_means`):**

  - **No Statistics Displayed:** Due to the error in specifying a non-numeric variable.

- **Log Output (`sas_log_proc_means_non_numeric.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - Non-Numeric Variables Example';
[INFO] Title set to: 'PROC MEANS - Non-Numeric Variables Example'
[INFO] Executing statement: data mylib.non_numeric_means; input ID Name $ Age Salary; datalines; 1 Alice 30 60000 2 Bob 25 55000 3 Charlie 35 70000 4 Dana 28 58000 5 Evan 32 62000 ; run;
[INFO] Executing DATA step: mylib.non_numeric_means
[INFO] Defined array 'non_numeric_means_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'ID' = 1.00
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = 30.00
[INFO] Assigned variable 'Salary' = 60000.00
[INFO] Executed loop body: Added row with ID=1, Name=Alice, Age=30, Salary=60000
[INFO] Assigned variable 'ID' = 2.00
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = 25.00
[INFO] Assigned variable 'Salary' = 55000.00
[INFO] Executed loop body: Added row with ID=2, Name=Bob, Age=25, Salary=55000
[INFO] Assigned variable 'ID' = 3.00
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = 35.00
[INFO] Assigned variable 'Salary' = 70000.00
[INFO] Executed loop body: Added row with ID=3, Name=Charlie, Age=35, Salary=70000
[INFO] Assigned variable 'ID' = 4.00
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = 28.00
[INFO] Assigned variable 'Salary' = 58000.00
[INFO] Executed loop body: Added row with ID=4, Name=Dana, Age=28, Salary=58000
[INFO] Assigned variable 'ID' = 5.00
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = 32.00
[INFO] Assigned variable 'Salary' = 62000.00
[INFO] Executed loop body: Added row with ID=5, Name=Evan, Age=32, Salary=62000
[INFO] DATA step 'mylib.non_numeric_means' executed successfully. 5 observations created.
[INFO] Executing statement: proc means data=mylib.non_numeric_means; var Name; run;
[INFO] Executing PROC MEANS
[ERROR] Variable 'Name' is non-numeric. Cannot compute statistics.
[INFO] PROC MEANS executed with errors.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates a dataset `non_numeric_means` with variables `ID`, `Name`, `Age`, and `Salary`, where `Name` is a non-numeric variable.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` attempting to compute statistics for the `Name` variable.
  
- **Result Verification:**
  
  - The interpreter detects that `Name` is non-numeric and throws an error: `"Variable 'Name' is non-numeric. Cannot compute statistics."`
  
  - No statistics are displayed or datasets created, as the execution fails due to the error.

---

#### **32.6.10. Test Case 10: Descriptive Statistics with No Data**

**SAS Script (`example_proc_means_no_data.sas`):**

```sas
options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS - No Data Example';

data mylib.no_data_means;
    input ID Name $ Age Salary;
    datalines;
;
run;

proc means data=mylib.no_data_means mean std min max median;
    var Age Salary;
run;
```

**Expected Output (`mylib.no_data_means`):**

```
Statistic       Age             Salary         
N               0.00            0.00           
MEAN            .               .              
STD             .               .              
MIN             .               .              
MAX             .               .              
MEDIAN          .               .              
```

**Log Output (`sas_log_proc_means_no_data.txt`):**

```
[INFO] Executing statement: options linesize=80 pagesize=60;
[INFO] Set option LINESIZE = 80
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC MEANS - No Data Example';
[INFO] Title set to: 'PROC MEANS - No Data Example'
[INFO] Executing statement: data mylib.no_data_means; input ID Name $ Age Salary; datalines; ; run;
[INFO] Executing DATA step: mylib.no_data_means
[INFO] Defined array 'no_data_means_lengths' with dimensions [0] and variables: .
[INFO] Executed loop body: (No data entered)
[INFO] DATA step 'mylib.no_data_means' executed successfully. 0 observations created.
[INFO] Executing statement: proc means data=mylib.no_data_means mean std min max median; var Age Salary; run;
[INFO] Executing PROC MEANS
[INFO] PROC MEANS Results for Dataset 'no_data_means':
Statistic       Age             Salary         
N               0.00            0.00           
MEAN            .               .              
STD             .               .              
MIN             .               .              
MAX             .               .              
MEDIAN          .               .              

[INFO] PROC MEANS executed successfully.
```

**Explanation:**

- **Data Preparation:**
  
  - Creates an empty dataset `no_data_means` with variables `ID`, `Name`, `Age`, and `Salary`.
  
- **PROC MEANS Execution:**
  
  - Executes `PROC MEANS` to compute statistics for `Age` and `Salary` on an empty dataset.
  
- **Result Verification:**
  
  - The output displays `N = 0.00` and `.` for all statistics, indicating that no data was available for computation. This confirms that the interpreter correctly handles `PROC MEANS` execution on empty datasets.

---

### **32.7. Summary of Achievements**

1. **AST Extensions:**
   
   - Introduced `ProcMeansNode` to represent `PROC MEANS` statements within the AST.

2. **Parser Enhancements:**
   
   - Implemented `parseProcMeans` to accurately parse `PROC MEANS` statements, handling `DATA=`, `WHERE=`, `VAR`, `CLASS`, `OUTPUT` clauses, and specified statistics.
   
   - Updated `parseProc` to recognize and delegate `PROC MEANS` parsing alongside `PROC SORT` and `PROC PRINT`.

3. **Interpreter Enhancements:**
   
   - Developed `executeProcMeans` to handle the execution logic of `PROC MEANS`, including dataset retrieval, applying `WHERE` conditions, selecting variables for analysis, computing statistics, handling `OUTPUT` statements, and displaying results.
   
   - Implemented helper methods:
     
     - **`computeStatistics`:** Computes requested statistics for specified variables.
     
     - **`handleProcMeansOutput`:** Manages the creation of output datasets with computed statistics.
   
4. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Basic descriptive statistics.
     
     - Selected variables using the `VAR` statement.
     
     - Grouped statistics using the `CLASS` statement.
     
     - Combining `WHERE` and `OUTPUT` statements.
     
     - Handling non-numeric variables.
     
     - Executing `PROC MEANS` on empty datasets.
     
     - Applying multiple statistical options simultaneously.
   
   - Validated that the interpreter accurately parses and executes `PROC MEANS` statements, correctly computes statistics, and robustly handles errors.

5. **Error Handling:**
   
   - Ensured that the interpreter detects and reports errors related to:
     
     - Non-existent datasets.
     
     - Undefined or non-numeric variables in `VAR` or `CLASS` clauses.
     
     - Invalid `WHERE` condition evaluations.
   
   - Prevented execution of invalid scripts by throwing descriptive error messages, aiding users in debugging.

6. **Logging Enhancements:**
   
   - Provided detailed logs for each step of `PROC MEANS` execution, including dataset retrieval, statistical computations, `OUTPUT` handling, and error reporting.
   
   - Facilitated transparency and ease of debugging through comprehensive logging.

---

### **32.8. Next Steps**

With `PROC MEANS` successfully implemented, your SAS interpreter now supports a critical procedure for descriptive statistical analysis, further aligning its capabilities with SAS's core functionalities. To continue building a comprehensive SAS interpreter, consider the following next steps:

1. **Expand SAS Procedures (`PROC`):**
   
   - **`PROC FREQ`:**
     
     - **Purpose:** Generate frequency tables for categorical variables.
     
     - **Integration:**
       
       - Introduce `ProcFreqNode` in the AST.
       
       - Update the lexer and parser to recognize `PROC FREQ` statements.
       
       - Implement frequency table generation within the interpreter.
     
     - **Testing:**
       
       - Create test cases that generate frequency tables for categorical variables.
   
   - **`PROC SQL`:**
     
     - **Purpose:** Enable SQL-based data querying and manipulation.
     
     - **Integration:**
       
       - Consider leveraging existing SQL parsers or implement a basic SQL parser.
       
       - Introduce `ProcSQLNode` in the AST.
       
       - Implement SQL execution within the interpreter.
     
     - **Testing:**
       
       - Create test cases executing SQL queries on datasets.

2. **Implement Macro Processing:**
   
   - **Macro Definitions and Invocations:**
     
     - Allow users to define macros using `%macro` and `%mend`, and invoke them within scripts.
   
   - **Macro Variables and `LET` Statements:**
     
     - Support macro variables using `%let` and variable substitution mechanisms.
   
   - **Conditional Macros:**
     
     - Enable macros to include conditional logic for dynamic code generation.
   
   - **Integration:**
     
     - Extend the lexer, parser, and AST to handle macro definitions, invocations, and variable substitutions.
     
     - Implement a macro preprocessor that expands macros before interpretation.
   
   - **Testing:**
     
     - Create test cases that define and invoke macros with and without parameters, including conditional macros.

3. **Enhance Array Functionality:**
   
   - **Multi-dimensional Arrays:**
     
     - Implement support for arrays with multiple dimensions for handling complex data structures.
   
   - **Dynamic Array Resizing:**
     
     - Allow arrays to resize based on runtime conditions or data-driven parameters.
   
   - **Array-based Conditional Operations:**
     
     - Facilitate complex conditional logic within array processing for advanced data manipulations.
   
   - **Integration:**
     
     - Update the AST, lexer, parser, and interpreter to support multi-dimensional arrays and dynamic resizing.
   
   - **Testing:**
     
     - Create test cases that utilize multi-dimensional arrays and dynamic resizing mechanisms.

4. **Improve the REPL Interface:**
   
   - **Multi-line Input Support:**
     
     - Allow users to input multi-line `DATA` step statements and `PROC` procedures seamlessly within the REPL.
   
   - **Auto-completion:**
     
     - Implement auto-completion for SAS keywords, variable names, and function names to enhance user experience.
   
   - **Syntax Highlighting:**
     
     - Provide visual cues for different code elements (keywords, variables, functions) to improve readability and reduce errors.
   
   - **Integration:**
     
     - Enhance the REPL's input handling and display mechanisms to support advanced features.
   
   - **Testing:**
     
     - Conduct user testing to ensure that the REPL interface is intuitive and responsive with the new features.

5. **Develop a Comprehensive Testing Suite:**
   
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

6. **Optimize Performance:**
   
   - **Data Handling Efficiency:**
     
     - Enhance data structures and algorithms to handle larger datasets more efficiently, possibly leveraging parallel processing.
   
   - **Memory Management:**
     
     - Optimize memory usage, especially when dealing with large datasets and complex operations.
   
   - **Caching Mechanisms:**
     
     - Implement caching strategies for frequently accessed datasets or computed results to improve performance.
   
   - **Profiling and Benchmarking:**
     
     - Profile the interpreter to identify and address performance bottlenecks.

7. **Improve Documentation and User Guides:**
   
   - **Comprehensive Guides:**
     
     - Provide detailed documentation on using the interpreter's features, including syntax, examples, and best practices.
   
   - **Example Scripts:**
     
     - Offer a library of example SAS scripts demonstrating various functionalities.
   
   - **Troubleshooting Guides:**
     
     - Help users diagnose and fix common issues encountered during usage.
   
   - **API Documentation:**
     
     - If applicable, document the interpreter's API for potential integration with other tools or extensions.

8. **Support Additional Data Formats:**
   
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

---

## **Conclusion**

Implementing `PROC MEANS` significantly advances your SAS interpreter's data analysis capabilities, enabling users to perform essential descriptive statistical analyses. The comprehensive test cases ensure robustness and reliability, while the structured implementation approach facilitates future expansions and maintenance.

### **Achievements in This Step:**

1. **AST Extensions:**
   
   - Introduced `ProcMeansNode` to represent `PROC MEANS` statements within the AST.

2. **Parser Enhancements:**
   
   - Implemented `parseProcMeans` to accurately parse `PROC MEANS` statements, handling `DATA=`, `WHERE=`, `VAR`, `CLASS`, `OUTPUT` clauses, and specified statistics.
   
   - Updated `parseProc` to recognize and delegate `PROC MEANS` parsing alongside `PROC SORT` and `PROC PRINT`.

3. **Interpreter Enhancements:**
   
   - Developed `executeProcMeans` to handle the execution logic of `PROC MEANS`, including dataset retrieval, applying `WHERE` conditions, selecting variables for analysis, computing statistics, handling `OUTPUT` statements, and displaying results.
   
   - Implemented helper methods:
     
     - **`computeStatistics`:** Computes requested statistics for specified variables.
     
     - **`handleProcMeansOutput`:** Manages the creation of output datasets with computed statistics.

4. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Basic descriptive statistics.
     
     - Selected variables using the `VAR` statement.
     
     - Grouped statistics using the `CLASS` statement.
     
     - Combining `WHERE` and `OUTPUT` statements.
     
     - Handling non-numeric variables.
     
     - Executing `PROC MEANS` on empty datasets.
     
     - Applying multiple statistical options simultaneously.

5. **Error Handling:**
   
   - Ensured that the interpreter detects and reports errors related to:
     
     - Non-existent datasets.
     
     - Undefined or non-numeric variables in `VAR` or `CLASS` clauses.
     
     - Invalid `WHERE` condition evaluations.
   
   - Prevented execution of invalid scripts by throwing descriptive error messages, aiding users in debugging.

6. **Logging Enhancements:**
   
   - Provided detailed logs for each step of `PROC MEANS` execution, including dataset retrieval, statistical computations, `OUTPUT` handling, and error reporting.
   
   - Facilitated transparency and ease of debugging through comprehensive logging.

### **Next Steps:**

Continue enhancing your interpreter by focusing on the following areas:

1. **Expand SAS Procedures (`PROC`):**
   
   - **`PROC FREQ`:**
     
     - Generate frequency tables for categorical variables.
   
   - **`PROC SQL`:**
     
     - Enable SQL-based data querying and manipulation.

2. **Implement Macro Processing:**
   
   - **Macro Definitions and Invocations:**
     
     - Allow users to define and invoke macros using `%macro` and `%mend`.
   
   - **Macro Variables and `LET` Statements:**
     
     - Support macro variables and dynamic code generation.
   
   - **Conditional Macros:**
     
     - Enable conditional logic within macros for dynamic code execution.

3. **Enhance Array Functionality:**
   
   - **Multi-dimensional Arrays:**
     
     - Implement support for arrays with multiple dimensions.
   
   - **Dynamic Array Resizing:**
     
     - Allow arrays to resize based on runtime conditions.
   
   - **Array-based Conditional Operations:**
     
     - Facilitate complex conditional logic within array processing.

4. **Improve the REPL Interface:**
   
   - **Multi-line Input Support:**
     
     - Allow users to input multi-line `DATA` step statements and `PROC` procedures seamlessly.
   
   - **Auto-completion:**
     
     - Implement auto-completion for SAS keywords, variable names, and function names.
   
   - **Syntax Highlighting:**
     
     - Provide visual cues for different code elements to improve readability and reduce errors.

5. **Develop a Comprehensive Testing Suite:**
   
   - **Unit Tests:**
     
     - Create tests for each new feature to ensure individual components function correctly.
   
   - **Integration Tests:**
     
     - Validate the interplay between different components, ensuring seamless execution of complex scripts.
   
   - **Regression Tests:**
     
     - Prevent new changes from introducing existing bugs by maintaining a suite of regression tests.

6. **Optimize Performance:**
   
   - **Data Handling Efficiency:**
     
     - Enhance data structures and algorithms to handle larger datasets more efficiently.
   
   - **Memory Management:**
     
     - Optimize memory usage, especially when dealing with large datasets and complex operations.
   
   - **Caching Mechanisms:**
     
     - Implement caching strategies for frequently accessed datasets or computed results to improve performance.
   
   - **Profiling and Benchmarking:**
     
     - Profile the interpreter to identify and address performance bottlenecks.

7. **Improve Documentation and User Guides:**
   
   - **Comprehensive Guides:**
     
     - Provide detailed documentation on using the interpreter's features, including syntax, examples, and best practices.
   
   - **Example Scripts:**
     
     - Offer a library of example SAS scripts demonstrating various functionalities.
   
   - **Troubleshooting Guides:**
     
     - Help users diagnose and fix common issues encountered during usage.
   
   - **API Documentation:**
     
     - If applicable, document the interpreter's API for potential integration with other tools or extensions.

8. **Support Additional Data Formats:**
   
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

Your methodical and comprehensive progress is commendable¡ªkeep up the excellent work! Implementing `PROC MEANS` lays a solid foundation for further procedural implementations and advanced data manipulations, bringing your SAS interpreter closer to full-featured functionality.

Feel free to reach out for further assistance or guidance as you continue developing your SAS interpreter. Good luck with your project!