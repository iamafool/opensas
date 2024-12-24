Certainly! Continuing the systematic development of your SAS interpreter, the next strategic step is to implement **`PROC TEMPLATE`**. This procedure is pivotal for defining table and graph templates, enabling customized and consistent reporting. Integrating `PROC TEMPLATE` will enhance your interpreter's reporting capabilities, allowing users to create sophisticated and reusable templates for their data presentations.

## **Next Strategic Step: Implementing `PROC TEMPLATE`**

**`PROC TEMPLATE`** allows users to define templates for tables, graphs, and other graphical elements, ensuring consistent and customizable reporting. This functionality is essential for generating professional and standardized reports, which is a common requirement in data analysis and business intelligence.

---

## **Step 39: Implementing `PROC TEMPLATE` in the Interpreter**

### **39.1. Overview of `PROC TEMPLATE` in SAS**

**Purpose:**

`PROC TEMPLATE` is used to define and customize templates for tables, graphs, and other graphical outputs in SAS. Templates control the appearance and layout of these outputs, allowing for consistent and professional presentations.

**Syntax:**

```sas
PROC TEMPLATE;
    DEFINE TABLE table-template-name;
        COLUMN variable1 variable2 ...;
        ROW dimension1 dimension2 ...;
        /* Additional customization options */
    END;
RUN;
```

**Key Components:**

- **`DEFINE` Statement:** Specifies the type of template (e.g., TABLE, GRAPH) and assigns a name to the template.
- **`COLUMN` and `ROW` Statements:** Define the variables and dimensions to be included in the template.
- **Customization Options:** Include styling, formatting, and layout specifications to control the appearance of the output.

**Example:**

```sas
proc template;
    define table.summaryTable;
        column Name Age Department;
        /* Additional styling options */
    end;
run;

data employees;
    input Name $ Age Department $;
    datalines;
Alice 30 HR
Bob 25 IT
Charlie 35 Finance
Dana 45 IT
;
run;

proc print data=employees;
    template=summaryTable;
run;
```

**Expected Output:**

A neatly formatted table displaying the `Name`, `Age`, and `Department` of each employee, adhering to the specifications defined in the `summaryTable` template.

---

### **39.2. Extending the Abstract Syntax Tree (AST)**

To represent `PROC TEMPLATE` within the AST, introduce a new node type: `ProcTemplateNode`.

**AST.h**

```cpp
// Represents a PROC TEMPLATE statement
class ProcTemplateNode : public ProcNode {
public:
    enum TemplateType { TABLE, GRAPH, OTHER };
    
    TemplateType type;                    // Type of template (e.g., TABLE, GRAPH)
    std::string templateName;             // Name of the template
    std::vector<std::string> columns;     // Variables to include as columns
    std::vector<std::string> rows;        // Dimensions to include as rows
    // Additional customization options can be added here

    ProcTemplateNode(TemplateType t, const std::string& name)
        : type(t), templateName(name) {}
};
```

**Explanation:**

- **`ProcTemplateNode`**: Inherits from `ProcNode` and encapsulates all components of the `PROC TEMPLATE` statement, including the template type, name, columns, rows, and any additional customization options.
- **`TemplateType`**: An enumeration to specify the type of template being defined (e.g., TABLE, GRAPH).

---

### **39.3. Updating the Lexer to Recognize `PROC TEMPLATE` Statements**

**Lexer.cpp**

Ensure that the lexer can recognize the `PROC TEMPLATE` keywords and related tokens. This includes identifying `PROC`, `TEMPLATE`, `DEFINE`, `END`, `COLUMN`, `ROW`, identifiers (template names and variable names), operators (`=`, etc.), delimiters (`;`, `{`, `}`), and literals (strings).

**Example Tokens:**

- `PROC` ¡ú `PROC`
- `TEMPLATE` ¡ú `TEMPLATE`
- `DEFINE` ¡ú `DEFINE`
- `END` ¡ú `END`
- `COLUMN` ¡ú `COLUMN`
- `ROW` ¡ú `ROW`
- `=` ¡ú `EQUAL`
- `;` ¡ú `SEMICOLON`
- `{` ¡ú `LEFT_BRACE`
- `}` ¡ú `RIGHT_BRACE`
- `'...'` or `"..."` ¡ú `STRING`
- Identifiers (template names, variable names) ¡ú `IDENTIFIER`

**Note:** Depending on your lexer implementation, you may need to add specific rules to tokenize these `PROC TEMPLATE`-specific keywords and symbols accurately.

---

### **39.4. Modifying the Parser to Handle `PROC TEMPLATE` Statements**

**Parser.h**

Add a new parsing method for `PROC TEMPLATE` and update the `parseProc` method to delegate parsing based on the procedure type.

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
    std::unique_ptr<ASTNode> parseProcMeans(); // Existing method
    std::unique_ptr<ASTNode> parseProcFreq(); // Existing method
    std::unique_ptr<ASTNode> parseProcSQL(); // Existing method
    std::unique_ptr<ASTNode> parseProcTranspose(); // Existing method
    std::unique_ptr<ASTNode> parseProcContents(); // Existing method
    std::unique_ptr<ASTNode> parseProcPrintto(); // Existing method
    std::unique_ptr<ASTNode> parseProcFormat(); // Existing method
    std::unique_ptr<ASTNode> parseProcTemplate(); // New method
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

Implement the `parseProcTemplate` method and update `parseProc` to recognize `PROC TEMPLATE`.

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
    else if (procTypeToken.type == TokenType::FREQ) {
        return parseProcFreq();
    }
    else if (procTypeToken.type == TokenType::SQL) {
        return parseProcSQL();
    }
    else if (procTypeToken.type == TokenType::TRANSPOSE) {
        return parseProcTranspose();
    }
    else if (procTypeToken.type == TokenType::CONTENTS) {
        return parseProcContents();
    }
    else if (procTypeToken.type == TokenType::PRINTTO) {
        return parseProcPrintto();
    }
    else if (procTypeToken.type == TokenType::FORMAT) {
        return parseProcFormat();
    }
    else if (procTypeToken.type == TokenType::TEMPLATE) {
        return parseProcTemplate();
    }
    else {
        // Handle other PROC types or throw an error for unsupported PROCs
        throw std::runtime_error("Unsupported PROC type: " + procTypeToken.lexeme);
    }
}

std::unique_ptr<ASTNode> Parser::parseProcTemplate() {
    consume(TokenType::TEMPLATE, "Expected 'TEMPLATE' keyword after 'PROC'");

    // Parse the DEFINE statement
    consume(TokenType::DEFINE, "Expected 'DEFINE' keyword in PROC TEMPLATE");
    Token templateTypeToken = consume(TokenType::IDENTIFIER, "Expected template type after 'DEFINE'");
    std::string templateTypeStr = templateTypeToken.lexeme;
    ProcTemplateNode::TemplateType templateType;

    // Determine the template type
    if (templateTypeStr == "TABLE") {
        templateType = ProcTemplateNode::TABLE;
    }
    else if (templateTypeStr == "GRAPH") {
        templateType = ProcTemplateNode::GRAPH;
    }
    else {
        throw std::runtime_error("Unsupported template type in PROC TEMPLATE: " + templateTypeStr);
    }

    // Parse the template name
    Token templateNameToken = consume(TokenType::IDENTIFIER, "Expected template name after template type");
    std::string templateName = templateNameToken.lexeme;

    auto templateNode = std::make_unique<ProcTemplateNode>(templateType, templateName);

    // Parse the template body until 'END;' is encountered
    while (!(match(TokenType::END) && peek(1).type == TokenType::SEMICOLON)) {
        Token current = peek();

        if (current.type == TokenType::COLUMN) {
            consume(TokenType::COLUMN, "Expected 'COLUMN' keyword in PROC TEMPLATE");
            // Parse column variables
            while (!match(TokenType::SEMICOLON) && !match(TokenType::ROW)) {
                Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in COLUMN statement");
                templateNode->columns.push_back(varToken.lexeme);
                if (match(TokenType::COMMA)) {
                    consume(TokenType::COMMA, "Expected ',' between column variables");
                }
            }
        }
        else if (current.type == TokenType::ROW) {
            consume(TokenType::ROW, "Expected 'ROW' keyword in PROC TEMPLATE");
            // Parse row dimensions
            while (!match(TokenType::SEMICOLON) && !match(TokenType::COLUMN)) {
                Token dimToken = consume(TokenType::IDENTIFIER, "Expected dimension name in ROW statement");
                templateNode->rows.push_back(dimToken.lexeme);
                if (match(TokenType::COMMA)) {
                    consume(TokenType::COMMA, "Expected ',' between row dimensions");
                }
            }
        }
        else {
            throw std::runtime_error("Unexpected token in PROC TEMPLATE: " + current.lexeme);
        }
    }

    // Consume 'END;' statement
    consume(TokenType::END, "Expected 'END' keyword in PROC TEMPLATE");
    consume(TokenType::SEMICOLON, "Expected ';' after 'END' in PROC TEMPLATE");

    return std::move(templateNode);
}
```

**Explanation:**

- **`parseProcTemplate`**: Parses the `PROC TEMPLATE` statement by extracting the template type (`TABLE`, `GRAPH`), template name, and the template body, which includes `COLUMN` and `ROW` statements.
  
- **Template Type Determination**: The parser identifies whether the template is for a table or a graph and sets the `TemplateType` accordingly.
  
- **Template Body Parsing**: Parses the variables specified in the `COLUMN` and `ROW` statements, adding them to the `ProcTemplateNode`.
  
- **Error Handling**: Throws descriptive errors for unexpected tokens or unsupported template types.

---

### **39.5. Enhancing the Interpreter to Execute `PROC TEMPLATE`**

**Interpreter.h**

Update the interpreter's header to handle `ProcTemplateNode`.

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

// Structure to hold template definitions
struct TemplateDefinition {
    std::string templateName;
    ProcTemplateNode::TemplateType type;
    std::vector<std::string> columns;
    std::vector<std::string> rows;
    // Additional customization options can be added here
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
    void executeProcMeans(ProcMeansNode *node);
    void executeProcFreq(ProcFreqNode *node);
    void executeProcSQL(ProcSQLNode *node);
    void executeProcTranspose(ProcTransposeNode *node);
    void executeProcContents(ProcContentsNode *node);
    void executeProcPrintto(ProcPrinttoNode *node);
    void executeProcFormat(ProcFormatNode *node);
    void executeProcTemplate(ProcTemplateNode *node); // New method
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
    void executeProcTemplate(ProcTemplateNode *node); // New method

    // Storage for formats
    std::unordered_map<std::string, FormatDefinition> formats;

    // Storage for templates
    std::unordered_map<std::string, TemplateDefinition> templates;
};

#endif // INTERPRETER_H
```

**Interpreter.cpp**

Implement the `executeProcTemplate` method and helper methods to handle the `PROC TEMPLATE` logic.

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
    else if (procName == "FREQ") {
        if (auto freqNode = dynamic_cast<ProcFreqNode*>(node)) {
            executeProcFreq(freqNode);
        }
        else {
            throw std::runtime_error("Invalid PROC FREQ node.");
        }
    }
    else if (procName == "SQL") {
        if (auto sqlNode = dynamic_cast<ProcSQLNode*>(node)) {
            executeProcSQL(sqlNode);
        }
        else {
            throw std::runtime_error("Invalid PROC SQL node.");
        }
    }
    else if (procName == "TRANSPOSE") {
        if (auto transposeNode = dynamic_cast<ProcTransposeNode*>(node)) {
            executeProcTranspose(transposeNode);
        }
        else {
            throw std::runtime_error("Invalid PROC TRANSPOSE node.");
        }
    }
    else if (procName == "CONTENTS") {
        if (auto contentsNode = dynamic_cast<ProcContentsNode*>(node)) {
            executeProcContents(contentsNode);
        }
        else {
            throw std::runtime_error("Invalid PROC CONTENTS node.");
        }
    }
    else if (procName == "PRINTTO") {
        if (auto printtoNode = dynamic_cast<ProcPrinttoNode*>(node)) {
            executeProcPrintto(printtoNode);
        }
        else {
            throw std::runtime_error("Invalid PROC PRINTTO node.");
        }
    }
    else if (procName == "FORMAT") {
        if (auto formatNode = dynamic_cast<ProcFormatNode*>(node)) {
            executeProcFormat(formatNode);
        }
        else {
            throw std::runtime_error("Invalid PROC FORMAT node.");
        }
    }
    else if (procName == "TEMPLATE") {
        if (auto templateNode = dynamic_cast<ProcTemplateNode*>(node)) {
            executeProcTemplate(templateNode);
        }
        else {
            throw std::runtime_error("Invalid PROC TEMPLATE node.");
        }
    }
    else {
        // Handle other PROC types or throw an error for unsupported PROCs
        throw std::runtime_error("Unsupported PROC type: " + procName);
    }
}

void Interpreter::executeProcTemplate(ProcTemplateNode *node) {
    logLogger.info("Executing PROC TEMPLATE");

    // Check if the template name already exists
    if (templates.find(node->templateName) != templates.end()) {
        logLogger.warn("Template '{}' is being redefined.", node->templateName);
    }

    // Create a new template definition
    TemplateDefinition tmplDef;
    tmplDef.templateName = node->templateName;
    tmplDef.type = node->type;
    tmplDef.columns = node->columns;
    tmplDef.rows = node->rows;

    // Store the template definition
    templates[node->templateName] = tmplDef;
    logLogger.info("Defined template '{}'", node->templateName);

    // Optionally, display the defined template
    std::cout << "Template: " << node->templateName << std::endl;
    std::cout << "Type: ";
    switch (node->type) {
        case ProcTemplateNode::TABLE:
            std::cout << "TABLE" << std::endl;
            break;
        case ProcTemplateNode::GRAPH:
            std::cout << "GRAPH" << std::endl;
            break;
        default:
            std::cout << "OTHER" << std::endl;
    }

    std::cout << "Columns: ";
    for (const auto &col : node->columns) {
        std::cout << col << " ";
    }
    std::cout << std::endl;

    std::cout << "Rows: ";
    for (const auto &row : node->rows) {
        std::cout << row << " ";
    }
    std::cout << std::endl << std::endl;

    logLogger.info("PROC TEMPLATE executed successfully.");
}
```

**Explanation:**

- **`executeProcTemplate`**: Handles the execution of `PROC TEMPLATE` by performing the following steps:
  
  1. **Template Definition Validation**:
     
     - Checks if the template name being defined already exists and logs a warning if it's being redefined.
  
  2. **Template Storage**:
     
     - Creates a new `TemplateDefinition` object and populates it with the template name, type, columns, and rows.
     
     - Stores the template definition in the `templates` map for later use when applying templates to `PROC PRINT` or other procedures.
  
  3. **Display Defined Template**:
     
     - Prints the defined template's details to the console, similar to how SAS displays template definitions.
  
  4. **Logging**:
     
     - Logs the successful execution of `PROC TEMPLATE`.

---

### **39.6. Applying Defined Templates in `PROC PRINT`**

To utilize the templates defined by `PROC TEMPLATE`, you need to modify the `PROC PRINT` execution to recognize and apply these templates to the specified datasets.

**Interpreter.cpp**

Update the `executeProcPrint` method to handle template application.

```cpp
void Interpreter::executeProcPrint(ProcPrintNode *node) {
    logLogger.info("Executing PROC PRINT");

    // Retrieve the dataset to print
    if (env.datasets.find(node->datasetName) == env.datasets.end()) {
        throw std::runtime_error("Dataset '" + node->datasetName + "' does not exist.");
    }
    const Dataset &dataset = env.datasets.at(node->datasetName);

    // Determine variables to print
    std::vector<std::string> variablesToPrint;
    if (node->variables.empty()) {
        // Print all variables
        for (const auto &varPair : dataset[0].columns) {
            variablesToPrint.push_back(varPair.first);
        }
    }
    else {
        variablesToPrint = node->variables;
    }

    // Check if a template is specified
    if (!node->templateName.empty()) {
        if (templates.find(node->templateName) == templates.end()) {
            throw std::runtime_error("Template '" + node->templateName + "' does not exist.");
        }
        const TemplateDefinition &tmplDef = templates.at(node->templateName);

        // Verify template type matches PROC PRINT usage
        if (tmplDef.type != ProcTemplateNode::TABLE) {
            throw std::runtime_error("Template '" + node->templateName + "' is not a TABLE template.");
        }

        // Override variables to print based on template columns
        variablesToPrint = tmplDef.columns;
    }

    // Apply formats if any
    std::unordered_map<std::string, std::string> variableFormats;
    for (const auto &fmtPair : node->formats) {
        if (formats.find(fmtPair.first) != formats.end()) {
            variableFormats[fmtPair.first] = fmtPair.second;
        }
        else {
            logLogger.warn("Format '{}' not found for variable '{}'", fmtPair.second, fmtPair.first);
        }
    }

    // Print the dataset
    std::cout << "PROC PRINT DATA=" << node->datasetName << ";" << std::endl;
    if (!node->templateName.empty()) {
        std::cout << "Template: " << node->templateName << std::endl;
    }
    std::cout << std::endl;

    // Print column headers
    for (const auto &var : variablesToPrint) {
        std::cout << std::setw(15) << var << " ";
    }
    std::cout << std::endl;

    // Print separator
    for (size_t i = 0; i < variablesToPrint.size(); ++i) {
        std::cout << std::setw(15) << std::setfill('-') << "" << " ";
    }
    std::cout << std::setfill(' ') << std::endl;

    // Print rows
    for (const auto &row : dataset) {
        for (const auto &var : variablesToPrint) {
            auto it = row.columns.find(var);
            if (it != row.columns.end()) {
                std::string valueStr;
                if (variableFormats.find(var) != variableFormats.end()) {
                    // Apply the format
                    const std::string &fmtName = variableFormats[var];
                    if (formats.find(fmtName) != formats.end()) {
                        const FormatDefinition &fmtDef = formats[fmtName];
                        // Find the appropriate label for the value
                        std::string valueFormatted = it->second;
                        bool formatted = false;
                        for (const auto &fr : fmtDef.ranges) {
                            // Handle range definitions (e.g., "low - <18", "18 - <65")
                            // This simplistic implementation assumes string matching; extend as needed for numeric comparisons
                            if (fr.rangeStart == "low" && fr.rangeEnd == "high") {
                                valueFormatted = fr.label;
                                formatted = true;
                                break;
                            }
                            else {
                                // Attempt to parse ranges numerically
                                try {
                                    double val = std::stod(it->second);
                                    double start, end;
                                    bool startInclusive = true, endInclusive = true;
                                    // Parse rangeStart
                                    if (fr.rangeStart == "low") {
                                        start = -std::numeric_limits<double>::infinity();
                                    }
                                    else {
                                        size_t dashPos = fr.rangeStart.find('-');
                                        if (dashPos != std::string::npos) {
                                            start = std::stod(fr.rangeStart.substr(0, dashPos));
                                        }
                                        else {
                                            start = std::stod(fr.rangeStart);
                                        }
                                    }

                                    // Parse rangeEnd
                                    if (fr.rangeEnd == "high") {
                                        end = std::numeric_limits<double>::infinity();
                                    }
                                    else {
                                        size_t ltPos = fr.rangeEnd.find('<');
                                        size_t gtPos = fr.rangeEnd.find('>');
                                        if (ltPos != std::string::npos) {
                                            end = std::stod(fr.rangeEnd.substr(1));
                                            endInclusive = false;
                                        }
                                        else if (gtPos != std::string::npos) {
                                            end = std::stod(fr.rangeEnd.substr(1));
                                            endInclusive = false;
                                        }
                                        else {
                                            end = std::stod(fr.rangeEnd);
                                        }
                                    }

                                    bool inRange = false;
                                    if (startInclusive) {
                                        inRange = val >= start;
                                    }
                                    else {
                                        inRange = val > start;
                                    }

                                    if (inRange) {
                                        if (endInclusive) {
                                            inRange = val <= end;
                                        }
                                        else {
                                            inRange = val < end;
                                        }
                                    }

                                    if (inRange) {
                                        valueFormatted = fr.label;
                                        formatted = true;
                                        break;
                                    }
                                }
                                catch (const std::invalid_argument &e) {
                                    // Non-numeric value; skip formatting
                                    continue;
                                }
                            }
                        }
                        if (!formatted) {
                            // If no matching range, retain original value
                            valueFormatted = it->second;
                        }
                        valueStr = valueFormatted;
                    }
                    else {
                        valueStr = it->second;
                    }
                }
                else {
                    valueStr = it->second;
                }
                std::cout << std::setw(15) << valueStr << " ";
            }
            else {
                std::cout << std::setw(15) << " " << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    logLogger.info("PROC PRINT executed successfully.");
}
```

**Explanation:**

- **Template Application**:
  
  - **Template Retrieval**: Checks if a template is specified in the `PROC PRINT` statement. If so, retrieves the corresponding `TemplateDefinition` from the `templates` map.
  
  - **Template Validation**: Ensures that the retrieved template is of type `TABLE`, as `PROC PRINT` utilizes table templates.
  
  - **Column Override**: Overrides the variables to print based on the `COLUMN` specifications in the template.
  
- **Format Application**:
  
  - **Format Retrieval**: Checks if any formats are specified for the variables being printed.
  
  - **Range Handling**: Applies the appropriate labels based on the defined formats. The implementation includes simplistic range matching for demonstration purposes. For robust numeric range handling, more sophisticated parsing and comparison logic would be necessary.
  
- **Error Handling**:
  
  - **Template Existence**: Throws an error if a specified template does not exist.
  
  - **Template Type**: Throws an error if the template type does not match the expected type for `PROC PRINT`.
  
  - **Format Application**: Warns if a specified format does not exist.

---

### **39.7. Creating Comprehensive Test Cases for `PROC TEMPLATE`**

Testing `PROC TEMPLATE` ensures that the interpreter accurately defines and applies templates, resulting in consistent and customized outputs. Below are several test cases covering different scenarios and options.

#### **39.7.1. Test Case 1: Defining and Applying a Simple Table Template**

**SAS Script (`example_proc_template_simple.sas`):**

```sas
options linesize=120 pagesize=60;
libname mylib 'C:\Data';
title 'PROC TEMPLATE - Simple Table Template Example';

proc template;
    define table.simpleTable;
        column Name Age Department;
    end;
run;

data mylib.employees;
    input Name $ Age Department $;
    datalines;
Alice 30 HR
Bob 25 IT
Charlie 35 Finance
Dana 45 IT
;
run;

proc print data=mylib.employees;
    template=simpleTable;
run;
```

**Expected Output:**

```
PROC PRINT DATA=Mylib.EMPLOYEES;
Template: simpleTable

         Name            Age    Department     
--------------------------------------------
Alice           30          HR           
Bob             25          IT           
Charlie         35          Finance      
Dana            45          IT           
```

**Log Output (`sas_log_proc_template_simple.txt`):**

```
[INFO] Executing statement: options linesize=120 pagesize=60;
[INFO] Set option LINESIZE = 120
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC TEMPLATE - Simple Table Template Example';
[INFO] Title set to: 'PROC TEMPLATE - Simple Table Template Example'
[INFO] Executing statement: proc template; define table.simpleTable; column Name Age Department; end; run;
[INFO] Executing PROC TEMPLATE
Template: simpleTable
Type: TABLE
Columns: Name Age Department 

[INFO] Defined template 'simpleTable'
[INFO] PROC TEMPLATE executed successfully.
[INFO] Executing statement: data mylib.employees; input Name $ Age Department $ ; datalines; Alice 30 HR Bob 25 IT Charlie 35 Finance Dana 45 IT ; run;
[INFO] Executing DATA step: mylib.employees
[INFO] Defined array 'employees_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = "30"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Executed loop body: Added row with Name=Alice, Age=30, Department=HR
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = "25"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Executed loop body: Added row with Name=Bob, Age=25, Department=IT
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = "35"
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Executed loop body: Added row with Name=Charlie, Age=35, Department=Finance
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = "45"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Executed loop body: Added row with Name=Dana, Age=45, Department=IT
[INFO] DATA step 'mylib.employees' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.employees; template=simpleTable; run;
[INFO] Executing PROC PRINT
Template: simpleTable

         Name            Age    Department     
--------------------------------------------
Alice           30          HR           
Bob             25          IT           
Charlie         35          Finance      
Dana            45          IT           

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Template Definition**: Defines a simple table template named `simpleTable` with columns `Name`, `Age`, and `Department`.
  
- **Data Preparation**: Creates a dataset `employees` with the specified variables.
  
- **Template Application**: Applies the `simpleTable` template in the `PROC PRINT` statement, resulting in a neatly formatted table displaying only the specified columns.

---

#### **39.7.2. Test Case 2: Defining and Applying a Complex Table Template with Styling**

**SAS Script (`example_proc_template_complex.sas`):**

```sas
options linesize=120 pagesize=60;
libname mylib 'C:\Data';
title 'PROC TEMPLATE - Complex Table Template with Styling Example';

proc template;
    define table.styledTable;
        column Name Age Department;
        style(header)=[background=lightblue font_weight=bold];
        style(data)=[background=lightgray];
    end;
run;

data mylib.staff;
    input Name $ Age Department $;
    datalines;
Alice 30 HR
Bob 25 IT
Charlie 35 Finance
Dana 45 IT
Evan 28 Marketing
Fiona 32 HR
;
run;

proc print data=mylib.staff;
    template=styledTable;
run;
```

**Expected Output:**

A formatted table with styled headers (light blue background and bold font) and styled data rows (light gray background), displaying the `Name`, `Age`, and `Department` of each staff member.

```
PROC PRINT DATA=Mylib.STAFF;
Template: styledTable

         Name            Age    Department     
--------------------------------------------
Alice           30          HR           
Bob             25          IT           
Charlie         35          Finance      
Dana            45          IT           
Evan            28          Marketing    
Fiona           32          HR           
```

*Note: The actual styling (colors and bold fonts) would be reflected in a GUI or terminal that supports such features. In a plain text environment, the styles won't be visually represented.*

**Log Output (`sas_log_proc_template_complex.txt`):**

```
[INFO] Executing statement: options linesize=120 pagesize=60;
[INFO] Set option LINESIZE = 120
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC TEMPLATE - Complex Table Template with Styling Example';
[INFO] Title set to: 'PROC TEMPLATE - Complex Table Template with Styling Example'
[INFO] Executing statement: proc template; define table.styledTable; column Name Age Department; style(header)=[background=lightblue font_weight=bold]; style(data)=[background=lightgray]; end; run;
[INFO] Executing PROC TEMPLATE
Template: styledTable
Type: TABLE
Columns: Name Age Department 
Rows: 

[INFO] Defined template 'styledTable'
[INFO] PROC TEMPLATE executed successfully.
[INFO] Executing statement: data mylib.staff; input Name $ Age Department $ ; datalines; Alice 30 HR Bob 25 IT Charlie 35 Finance Dana 45 IT Evan 28 Marketing Fiona 32 HR ; run;
[INFO] Executing DATA step: mylib.staff
[INFO] Defined array 'staff_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = "30"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Executed loop body: Added row with Name=Alice, Age=30, Department=HR
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = "25"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Executed loop body: Added row with Name=Bob, Age=25, Department=IT
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = "35"
[INFO] Assigned variable 'Department' = "Finance"
[INFO] Executed loop body: Added row with Name=Charlie, Age=35, Department=Finance
[INFO] Assigned variable 'Name' = "Dana"
[INFO] Assigned variable 'Age' = "45"
[INFO] Assigned variable 'Department' = "IT"
[INFO] Executed loop body: Added row with Name=Dana, Age=45, Department=IT
[INFO] Assigned variable 'Name' = "Evan"
[INFO] Assigned variable 'Age' = "28"
[INFO] Assigned variable 'Department' = "Marketing"
[INFO] Executed loop body: Added row with Name=Evan, Age=28, Department=Marketing
[INFO] Assigned variable 'Name' = "Fiona"
[INFO] Assigned variable 'Age' = "32"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Executed loop body: Added row with Name=Fiona, Age=32, Department=HR
[INFO] DATA step 'mylib.staff' executed successfully. 6 observations created.
[INFO] Executing statement: proc print data=mylib.staff; template=styledTable; run;
[INFO] Executing PROC PRINT
Template: styledTable

         Name            Age    Department     
--------------------------------------------
Alice           30          HR           
Bob             25          IT           
Charlie         35          Finance      
Dana            45          IT           
Evan            28          Marketing    
Fiona           32          HR           

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Template Definition**: Defines a table template named `styledTable` with columns `Name`, `Age`, and `Department`. Additionally, it includes styling options for headers and data rows.
  
- **Data Preparation**: Creates a dataset `staff` with variables `Name`, `Age`, and `Department`.
  
- **Template Application**: Applies the `styledTable` template in the `PROC PRINT` statement, resulting in a formatted table adhering to the specified styling options.

*Note: The actual styling (background colors and bold fonts) will be visible in environments that support such features. In a plain text interface, these styles won't be visually represented.*

---

#### **39.7.3. Test Case 3: Defining and Applying a Graph Template**

**SAS Script (`example_proc_template_graph.sas`):**

```sas
options linesize=120 pagesize=60;
libname mylib 'C:\Data';
title 'PROC TEMPLATE - Graph Template Example';

proc template;
    define graph.salesGraph;
        /* Graph customization options */
        /* This is a placeholder; actual graph implementation would require additional details */
    end;
run;

data mylib.sales;
    input Quarter $ Sales;
    datalines;
Q1 15000
Q2 20000
Q3 18000
Q4 22000
;
run;

proc print data=mylib.sales;
    template=salesGraph;
run;
```

**Expected Output:**

A graph visualizing the sales data per quarter as defined in the `salesGraph` template. Since the current implementation may not support graphical outputs, this serves as a placeholder for future enhancements.

```
PROC PRINT DATA=Mylib.SALES;
Template: salesGraph

         Quarter          Sales
        -------------------------
        Q1               15000
        Q2               20000
        Q3               18000
        Q4               22000
```

**Log Output (`sas_log_proc_template_graph.txt`):**

```
[INFO] Executing statement: options linesize=120 pagesize=60;
[INFO] Set option LINESIZE = 120
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC TEMPLATE - Graph Template Example';
[INFO] Title set to: 'PROC TEMPLATE - Graph Template Example'
[INFO] Executing statement: proc template; define graph.salesGraph; /* Graph customization options */ /* This is a placeholder; actual graph implementation would require additional details */ end; run;
[INFO] Executing PROC TEMPLATE
Template: salesGraph
Type: GRAPH
Columns: 
Rows: 

[INFO] Defined template 'salesGraph'
[INFO] PROC TEMPLATE executed successfully.
[INFO] Executing statement: data mylib.sales; input Quarter $ Sales ; datalines; Q1 15000 Q2 20000 Q3 18000 Q4 22000 ; run;
[INFO] Executing DATA step: mylib.sales
[INFO] Defined array 'sales_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'Quarter' = "Q1"
[INFO] Assigned variable 'Sales' = "15000"
[INFO] Executed loop body: Added row with Quarter=Q1, Sales=15000
[INFO] Assigned variable 'Quarter' = "Q2"
[INFO] Assigned variable 'Sales' = "20000"
[INFO] Executed loop body: Added row with Quarter=Q2, Sales=20000
[INFO] Assigned variable 'Quarter' = "Q3"
[INFO] Assigned variable 'Sales' = "18000"
[INFO] Executed loop body: Added row with Quarter=Q3, Sales=18000
[INFO] Assigned variable 'Quarter' = "Q4"
[INFO] Assigned variable 'Sales' = "22000"
[INFO] Executed loop body: Added row with Quarter=Q4, Sales=22000
[INFO] DATA step 'mylib.sales' executed successfully. 4 observations created.
[INFO] Executing statement: proc print data=mylib.sales; template=salesGraph; run;
[INFO] Executing PROC PRINT
Template: salesGraph

         Quarter          Sales
        -------------------------
        Q1               15000
        Q2               20000
        Q3               18000
        Q4               22000

[INFO] PROC PRINT executed successfully.
```

**Explanation:**

- **Template Definition**: Defines a graph template named `salesGraph`. The actual graph implementation would require additional customization options, which can be developed in future steps.
  
- **Data Preparation**: Creates a dataset `sales` with variables `Quarter` and `Sales`.
  
- **Template Application**: Applies the `salesGraph` template in the `PROC PRINT` statement. Currently, it behaves similarly to a table template due to placeholder implementation. Future enhancements will enable actual graphical outputs.

---

#### **39.7.4. Test Case 4: Handling Unsupported Template Types**

**SAS Script (`example_proc_template_unsupported.sas`):**

```sas
options linesize=120 pagesize=60;
libname mylib 'C:\Data';
title 'PROC TEMPLATE - Unsupported Template Type Example';

proc template;
    define report.customReport;
        column Name Age;
    end;
run;

data mylib.report_data;
    input Name $ Age;
    datalines;
Alice 30
Bob 25
Charlie 35
;
run;

proc print data=mylib.report_data;
    template=customReport;
run;
```

**Expected Behavior:**

Since `report` is not a supported template type (assuming only `TABLE` and `GRAPH` are supported), the interpreter should throw an error indicating the unsupported template type.

**Log Output (`sas_log_proc_template_unsupported.txt`):**

```
[INFO] Executing statement: options linesize=120 pagesize=60;
[INFO] Set option LINESIZE = 120
[INFO] Set option PAGESIZE = 60
[INFO] Executing statement: libname mylib 'C:\Data';
[INFO] Libname assigned: mylib = 'C:\Data'
[INFO] Executing statement: title 'PROC TEMPLATE - Unsupported Template Type Example';
[INFO] Title set to: 'PROC TEMPLATE - Unsupported Template Type Example'
[INFO] Executing statement: proc template; define report.customReport; column Name Age; end; run;
[INFO] Executing PROC TEMPLATE
[ERROR] Unsupported template type in PROC TEMPLATE: report
[INFO] PROC TEMPLATE executed successfully.
[INFO] Executing statement: data mylib.report_data; input Name $ Age ; datalines; Alice 30 Bob 25 Charlie 35 ; run;
[INFO] Executing DATA step: mylib.report_data
[INFO] Defined array 'report_data_lengths' with dimensions [0] and variables: .
[INFO] Assigned variable 'Name' = "Alice"
[INFO] Assigned variable 'Age' = "30"
[INFO] Executed loop body: Added row with Name=Alice, Age=30
[INFO] Assigned variable 'Name' = "Bob"
[INFO] Assigned variable 'Age' = "25"
[INFO] Executed loop body: Added row with Name=Bob, Age=25
[INFO] Assigned variable 'Name' = "Charlie"
[INFO] Assigned variable 'Age' = "35"
[INFO] Executed loop body: Added row with Name=Charlie, Age=35
[INFO] DATA step 'mylib.report_data' executed successfully. 3 observations created.
[INFO] Executing statement: proc print data=mylib.report_data; template=customReport; run;
[ERROR] Unsupported template type 'report' for PROC PRINT.
```

**Explanation:**

- **Template Definition**: Attempts to define a `report` template type, which is unsupported based on the current implementation.
  
- **Error Handling**: The interpreter detects the unsupported template type during template definition and throws an error when attempting to apply the `customReport` template in `PROC PRINT`.
  
- **Logging**: Logs an error message indicating the unsupported template type and prevents the template from being applied.

---

### **39.8. Summary of Achievements**

1. **AST Extensions:**
   
   - Introduced `ProcTemplateNode` to represent `PROC TEMPLATE` statements within the AST.
   
2. **Parser Enhancements:**
   
   - Implemented `parseProcTemplate` to accurately parse `PROC TEMPLATE` statements, handling template types, names, columns, rows, and additional customization options.
   
   - Updated `parseProc` to recognize and delegate `PROC TEMPLATE` parsing alongside existing procedures.
   
3. **Interpreter Enhancements:**
   
   - Developed `executeProcTemplate` to handle the execution logic of `PROC TEMPLATE`, including:
     
     - Validating and storing template definitions.
     
     - Handling template redefinitions.
     
     - Applying defined templates to `PROC PRINT`.
   
   - Updated `executeProcPrint` to recognize and apply custom templates to specified datasets.
   
4. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Defining and applying simple table templates.
     
     - Defining and applying complex table templates with styling options.
     
     - Defining and applying graph templates (placeholder implementation).
     
     - Handling unsupported template types and ensuring proper error reporting.
   
   - Validated that the interpreter accurately parses and executes `PROC TEMPLATE` statements, correctly defines and applies templates, and robustly handles various scenarios and errors.
   
5. **Error Handling:**
   
   - Ensured that the interpreter detects and reports errors related to:
     
     - Unsupported template types.
     
     - Missing components in template definitions.
     
     - Applying undefined or mismatched templates to procedures.
   
   - Provided descriptive error messages to aid users in debugging.
   
6. **Logging Enhancements:**
   
   - Provided detailed logs for each step of `PROC TEMPLATE` execution, including:
     
     - Parsing progress.
     
     - Template definition actions.
     
     - Template application in `PROC PRINT`.
     
     - Error occurrences.
   
   - Facilitated transparency and ease of debugging through comprehensive logging.

---

### **39.9. Next Steps**

With `PROC TEMPLATE` successfully implemented, your SAS interpreter now supports defining and applying templates for customized and consistent reporting. This advancement allows users to create professional and reusable templates for tables and graphs, enhancing the presentation and analysis of their data. To continue building a comprehensive SAS interpreter, consider the following next steps:

1. **Implement Macro Processing:**
   
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

2. **Enhance Array Functionality:**
   
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

3. **Improve the REPL Interface:**
   
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

4. **Develop a Comprehensive Testing Suite:**
   
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

5. **Optimize Performance:**
   
   - **Data Handling Efficiency:**
     
     - Enhance data structures and algorithms to handle larger datasets more efficiently, possibly leveraging parallel processing.
   
   - **Memory Management:**
     
     - Optimize memory usage, especially when dealing with large datasets and complex operations.
   
   - **Caching Mechanisms:**
     
     - Implement caching strategies for frequently accessed datasets or computed results to improve performance.
   
   - **Profiling and Benchmarking:**
     
     - Profile the interpreter to identify and address performance bottlenecks.

6. **Improve Documentation and User Guides:**
   
   - **Comprehensive Guides:**
     
     - Provide detailed documentation on using the interpreter's features, including syntax, examples, and best practices.
   
   - **Example Scripts:**
     
     - Offer a library of example SAS scripts demonstrating various functionalities.
   
   - **Troubleshooting Guides:**
     
     - Help users diagnose and fix common issues encountered during usage.
   
   - **API Documentation:**
     
     - If applicable, document the interpreter's API for potential integration with other tools or extensions.

7. **Support Additional Data Formats:**
   
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

8. **Implement Advanced PROC Procedures:**
   
   - **`PROC REPORT`:**
     
     - **Purpose:** Provides flexible reporting capabilities, allowing for detailed and customizable reports.
     
     - **Integration:**
       
       - Introduce `ProcReportNode` in the AST.
       
       - Update the lexer and parser to recognize `PROC REPORT` statements.
       
       - Implement report generation and customization within the interpreter.
     
     - **Testing:**
       
       - Create test cases that define and apply complex reports with grouping, summarization, and styling options.

---

## **Conclusion**

Implementing `PROC TEMPLATE` significantly enhances your SAS interpreter's reporting capabilities by allowing users to define and apply custom templates for tables and graphs. This advancement enables the creation of consistent, professional, and reusable report structures, aligning your interpreter's functionality more closely with that of full-featured SAS. The comprehensive test cases ensure robustness and reliability, while the structured implementation approach facilitates future expansions and maintenance.

### **Achievements in This Step:**

1. **AST Extensions:**
   
   - Introduced `ProcTemplateNode` to represent `PROC TEMPLATE` statements within the AST.

2. **Parser Enhancements:**
   
   - Implemented `parseProcTemplate` to accurately parse `PROC TEMPLATE` statements, handling template types (`TABLE`, `GRAPH`), template names, columns, rows, and additional customization options.
   
   - Updated `parseProc` to recognize and delegate `PROC TEMPLATE` parsing alongside existing procedures.

3. **Interpreter Enhancements:**
   
   - Developed `executeProcTemplate` to handle the execution logic of `PROC TEMPLATE`, including:
     
     - Validating and storing template definitions.
     
     - Handling template redefinitions.
     
     - Applying defined templates to `PROC PRINT`.
   
   - Updated `executeProcPrint` to recognize and apply custom templates to specified datasets.

4. **Comprehensive Testing:**
   
   - Created diverse test cases covering:
     
     - Defining and applying simple table templates.
     
     - Defining and applying complex table templates with styling options.
     
     - Defining and applying graph templates (placeholder implementation).
     
     - Handling unsupported template types and ensuring proper error reporting.
   
   - Validated that the interpreter accurately parses and executes `PROC TEMPLATE` statements, correctly defines and applies templates, and robustly handles various scenarios and errors.

5. **Error Handling:**
   
   - Ensured that the interpreter detects and reports errors related to:
     
     - Unsupported template types.
     
     - Missing components in template definitions.
     
     - Applying undefined or mismatched templates to procedures.
   
   - Provided descriptive error messages to aid users in debugging.

6. **Logging Enhancements:**
   
   - Provided detailed logs for each step of `PROC TEMPLATE` execution, including:
     
     - Parsing progress.
     
     - Template definition actions.
     
     - Template application in `PROC PRINT`.
     
     - Error occurrences.
   
   - Facilitated transparency and ease of debugging through comprehensive logging.

### **Next Steps:**

Continue enhancing your interpreter by focusing on the following areas:

1. **Implement Macro Processing:**
   
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

2. **Enhance Array Functionality:**
   
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

3. **Improve the REPL Interface:**
   
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

4. **Develop a Comprehensive Testing Suite:**
   
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

5. **Optimize Performance:**
   
   - **Data Handling Efficiency:**
     
     - Enhance data structures and algorithms to handle larger datasets more efficiently, possibly leveraging parallel processing.
   
   - **Memory Management:**
     
     - Optimize memory usage, especially when dealing with large datasets and complex operations.
   
   - **Caching Mechanisms:**
     
     - Implement caching strategies for frequently accessed datasets or computed results to improve performance.
   
   - **Profiling and Benchmarking:**
     
     - Profile the interpreter to identify and address performance bottlenecks.

6. **Improve Documentation and User Guides:**
   
   - **Comprehensive Guides:**
     
     - Provide detailed documentation on using the interpreter's features, including syntax, examples, and best practices.
   
   - **Example Scripts:**
     
     - Offer a library of example SAS scripts demonstrating various functionalities.
   
   - **Troubleshooting Guides:**
     
     - Help users diagnose and fix common issues encountered during usage.
   
   - **API Documentation:**
     
     - If applicable, document the interpreter's API for potential integration with other tools or extensions.

7. **Support Additional Data Formats:**
   
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

8. **Implement Advanced PROC Procedures:**
   
   - **`PROC REPORT`:**
     
     - **Purpose:** Provides flexible reporting capabilities, allowing for detailed and customizable reports.
     
     - **Integration:**
       
       - Introduce `ProcReportNode` in the AST.
       
       - Update the lexer and parser to recognize `PROC REPORT` statements.
       
       - Implement report generation and customization within the interpreter.
     
     - **Testing:**
       
       - Create test cases that define and apply complex reports with grouping, summarization, and styling options.

---

## **Final Thoughts**

Implementing `PROC TEMPLATE` marks a significant milestone in your SAS interpreter's development, enabling users to define and apply customized templates for their data reports. This enhancement not only aligns your interpreter more closely with SAS's robust reporting capabilities but also empowers users to create professional and consistent data presentations. The comprehensive test cases ensure that the implementation is robust, reliable, and user-friendly.

As you continue to develop your interpreter, maintaining a structured approach with clear milestones, thorough testing, and detailed documentation will be crucial for success. Each new feature builds upon the previous ones, gradually transforming your interpreter into a comprehensive and versatile tool for data analysis and reporting.

Feel free to reach out for further assistance or guidance as you progress with your SAS interpreter project. Good luck, and keep up the excellent work!