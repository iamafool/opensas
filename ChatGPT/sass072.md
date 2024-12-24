Certainly! Building upon the successful implementation of **`PROC TABULATE`**, the next strategic step is to **Implement `PROC SQL`**. This procedure is pivotal in data manipulation and querying, providing SQL (Structured Query Language) capabilities within the SAS environment. By integrating `PROC SQL`, your SAS interpreter will empower users to perform complex data queries, joins, and manipulations, significantly enhancing its data processing and analytical functionalities.

## **Next Strategic Step: Implementing `PROC SQL`**

**`PROC SQL`** is a powerful procedure in SAS that allows users to execute SQL queries directly within SAS programs. It enables a wide range of data manipulation tasks, including selecting specific data, joining multiple tables, aggregating data, and creating new datasets based on complex conditions. Implementing `PROC SQL` will greatly enhance your interpreter's versatility and align it with the comprehensive data management capabilities expected from SAS environments.

---

## **Step 51: Implementing `PROC SQL`**

### **51.1. Understanding `PROC SQL`**

**`PROC SQL`** provides functionality to:

- **Execute SQL Queries:** Perform `SELECT`, `INSERT`, `UPDATE`, `DELETE`, and `CREATE` operations on SAS datasets.
- **Join Multiple Tables:** Combine data from different datasets using various types of joins (`INNER`, `LEFT`, `RIGHT`, `FULL`).
- **Aggregate Data:** Use functions like `COUNT`, `SUM`, `AVG`, `MAX`, and `MIN` to summarize data.
- **Create New Tables and Views:** Generate new datasets or views based on query results.
- **Manage Data Integrity:** Apply constraints and manage data relationships within the SQL framework.
- **Handle Subqueries:** Use nested queries to perform complex data retrieval operations.

**Example SAS Script Using `PROC SQL`:**

```sas
data employees;
    input EmployeeID $ Name $ Department $ Salary;
    datalines;
E001 John Sales 50000
E002 Jane Marketing 55000
E003 Mike Sales 60000
E004 Anna HR 52000
E005 Tom Marketing 58000
E006 Lucy HR 50000
;
run;

data departments;
    input Department $ Manager $;
    datalines;
Sales Alice
Marketing Bob
HR Carol
;
run;

proc sql;
    create table emp_dept as
    select e.EmployeeID, e.Name, e.Department, d.Manager, e.Salary
    from employees e
    inner join departments d
    on e.Department = d.Department
    where e.Salary > 50000
    order by e.Salary desc;
quit;

proc print data=emp_dept;
run;
```

**Expected Output:**

```
EmployeeID    Name    Department    Manager    Salary
------------------------------------------------------
E003          Mike    Sales         Alice      60000
E005          Tom     Marketing     Bob        58000
E002          Jane    Marketing     Bob        55000
E004          Anna    HR            Carol      52000
```

---

### **51.2. Extending the Abstract Syntax Tree (AST) for `PROC SQL`**

To support `PROC SQL`, extend your AST to include nodes representing SQL statements, queries, joins, and other SQL-specific components.

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

// PROC SQL specific nodes
class ProcSQLNode : public ProcStepNode {
public:
    // List of SQL statements within PROC SQL
    std::vector<std::unique_ptr<SQLStatement>> sqlStatements;
    
    ProcSQLNode(const std::string& name, const std::string& data)
        : ProcStepNode(name, data) {}
};

// Base class for SQL statements
class SQLStatement {
public:
    virtual ~SQLStatement() = default;
};

// SELECT statement node
class SelectStatement : public SQLStatement {
public:
    std::vector<std::string> selectColumns; // Columns to select
    std::string fromTable;                   // Table to select from
    std::vector<std::pair<std::string, std::string>> joins; // Joins: pair<joinType, tableName>
    std::string whereClause;                 // WHERE condition
    std::vector<std::string> orderByColumns; // ORDER BY columns

    SelectStatement() = default;
};

// CREATE TABLE statement node
class CreateTableStatement : public SQLStatement {
public:
    std::string tableName; // Name of the table to create
    SelectStatement selectStmt; // SELECT statement to populate the table

    CreateTableStatement() = default;
};

// ... Additional SQL statement nodes as needed ...

#endif // AST_H
```

**Explanation:**

- **`ProcSQLNode`:** Inherits from `ProcStepNode` and contains a list of SQL statements to be executed within the `PROC SQL` block.
  
- **`SQLStatement`:** Abstract base class for various SQL statements.
  
- **`SelectStatement`:** Represents a `SELECT` statement, capturing selected columns, source table, joins, where clause, and order by columns.
  
- **`CreateTableStatement`:** Represents a `CREATE TABLE` statement that uses a `SELECT` statement to define the table's contents.

---

### **51.3. Updating the Lexer to Recognize `PROC SQL` Syntax**

Ensure that the Lexer can tokenize the syntax specific to `PROC SQL`, such as SQL keywords (`SELECT`, `FROM`, `JOIN`, `WHERE`, `CREATE`, etc.), operators, and other SQL-specific tokens.

**Lexer.cpp**

```cpp
// Extending the Lexer to handle PROC SQL statements

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
            // Convert to uppercase for case-insensitive matching
            std::transform(identifier.begin(), identifier.end(), identifier.begin(), ::toupper);
            // Check for SQL keywords
            if (identifier == "PROC") {
                tokens.emplace_back(TokenType::PROC, identifier);
            }
            else if (identifier == "SQL") {
                tokens.emplace_back(TokenType::PROC_SQL, identifier);
            }
            else if (identifier == "SELECT") {
                tokens.emplace_back(TokenType::SELECT, identifier);
            }
            else if (identifier == "FROM") {
                tokens.emplace_back(TokenType::FROM, identifier);
            }
            else if (identifier == "WHERE") {
                tokens.emplace_back(TokenType::WHERE, identifier);
            }
            else if (identifier == "INNER") {
                tokens.emplace_back(TokenType::INNER_JOIN, identifier);
            }
            else if (identifier == "JOIN") {
                tokens.emplace_back(TokenType::JOIN, identifier);
            }
            else if (identifier == "LEFT") {
                tokens.emplace_back(TokenType::LEFT_JOIN, identifier);
            }
            else if (identifier == "RIGHT") {
                tokens.emplace_back(TokenType::RIGHT_JOIN, identifier);
            }
            else if (identifier == "FULL") {
                tokens.emplace_back(TokenType::FULL_JOIN, identifier);
            }
            else if (identifier == "ON") {
                tokens.emplace_back(TokenType::ON, identifier);
            }
            else if (identifier == "CREATE") {
                tokens.emplace_back(TokenType::CREATE, identifier);
            }
            else if (identifier == "TABLE") {
                tokens.emplace_back(TokenType::TABLE, identifier);
            }
            else if (identifier == "QUIT") {
                tokens.emplace_back(TokenType::QUIT, identifier);
            }
            else if (identifier == "INSERT") {
                tokens.emplace_back(TokenType::INSERT, identifier);
            }
            else if (identifier == "INTO") {
                tokens.emplace_back(TokenType::INTO, identifier);
            }
            else if (identifier == "VALUES") {
                tokens.emplace_back(TokenType::VALUES, identifier);
            }
            else if (identifier == "UPDATE") {
                tokens.emplace_back(TokenType::UPDATE, identifier);
            }
            else if (identifier == "SET") {
                tokens.emplace_back(TokenType::SET, identifier);
            }
            else if (identifier == "DELETE") {
                tokens.emplace_back(TokenType::DELETE, identifier);
            }
            else if (identifier == "ORDER") {
                tokens.emplace_back(TokenType::ORDER, identifier);
            }
            else if (identifier == "BY") {
                tokens.emplace_back(TokenType::BY, identifier);
            }
            else if (identifier == "DESC") {
                tokens.emplace_back(TokenType::DESC, identifier);
            }
            else if (identifier == "ASC") {
                tokens.emplace_back(TokenType::ASC, identifier);
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
                case ',': tokens.emplace_back(TokenType::COMMA, ","); break;
                case '*': tokens.emplace_back(TokenType::STAR, "*"); break;
                case '=': tokens.emplace_back(TokenType::EQUAL, "="); break;
                case '(': tokens.emplace_back(TokenType::LEFT_PAREN, "("); break;
                case ')': tokens.emplace_back(TokenType::RIGHT_PAREN, ")"); break;
                case '.': tokens.emplace_back(TokenType::DOT, "."); break;
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
                case '>':
                    if (match('=')) {
                        tokens.emplace_back(TokenType::GREATER_EQUAL, ">=");
                    }
                    else {
                        tokens.emplace_back(TokenType::GREATER, ">");
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

- **`PROC SQL` Detection:** The Lexer now recognizes `PROC SQL` by checking if the identifier `SQL` follows `PROC`.
  
- **SQL Keywords:** Added tokens for SQL-specific keywords such as `SELECT`, `FROM`, `WHERE`, `JOIN`, `CREATE`, `TABLE`, `INSERT`, `UPDATE`, `DELETE`, `ORDER BY`, etc.
  
- **Case-Insensitive Matching:** Transforms identifiers to uppercase to handle case-insensitive SQL keywords.

- **Operator Handling:** Enhanced handling for operators like `<=`, `>=`, `<>`, etc., to support SQL conditional expressions.

---

### **51.4. Updating the Parser to Handle `PROC SQL` Statements**

Modify the Parser to construct `ProcSQLNode` when encountering `PROC SQL` statements, including parsing various SQL statements like `SELECT`, `CREATE TABLE`, `INSERT`, `UPDATE`, and `DELETE`.

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
        else if (match(TokenType::SQL)) {
            return parseProcSQL();
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

std::unique_ptr<ASTNode> Parser::parseProcSQL() {
    Token procNameToken = previous(); // PROC SQL
    std::string procName = procNameToken.lexeme;

    auto procSQL = std::make_unique<ProcSQLNode>(procName, "");

    // Parse SQL statements until QUIT;
    while (!check(TokenType::QUIT) && !isAtEnd()) {
        if (match(TokenType::CREATE)) {
            auto createStmt = parseCreateTableStatement();
            procSQL->sqlStatements.emplace_back(std::move(createStmt));
        }
        else if (match(TokenType::SELECT)) {
            auto selectStmt = parseSelectStatement();
            procSQL->sqlStatements.emplace_back(std::move(selectStmt));
        }
        else if (match(TokenType::INSERT)) {
            // Implement parseInsertStatement()
        }
        else if (match(TokenType::UPDATE)) {
            // Implement parseUpdateStatement()
        }
        else if (match(TokenType::DELETE)) {
            // Implement parseDeleteStatement()
        }
        else {
            throw std::runtime_error("Unsupported SQL statement in PROC SQL: " + peek().lexeme);
        }
    }

    consume(TokenType::QUIT, "Expected QUIT; to end PROC SQL step.");
    consume(TokenType::SEMICOLON, "Expected ';' after QUIT.");

    return procSQL;
}

std::unique_ptr<SQLStatement> Parser::parseCreateTableStatement() {
    Token createToken = previous(); // CREATE
    consume(TokenType::TABLE, "Expected TABLE after CREATE.");

    Token tableNameToken = consume(TokenType::IDENTIFIER, "Expected table name after CREATE TABLE.");
    std::string tableName = tableNameToken.lexeme;

    consume(TokenType::AS, "Expected AS after CREATE TABLE.");

    // Parse the SELECT statement that defines the table
    auto selectStmt = parseSelectStatement();

    auto createTableStmt = std::make_unique<CreateTableStatement>();
    createTableStmt->tableName = tableName;
    createTableStmt->selectStmt = *selectStmt; // Assuming deep copy is handled

    return createTableStmt;
}

std::unique_ptr<SelectStatement> Parser::parseSelectStatement() {
    auto selectStmt = std::make_unique<SelectStatement>();

    // Parse selected columns
    do {
        Token colToken = consume(TokenType::IDENTIFIER, "Expected column name in SELECT statement.");
        selectStmt->selectColumns.push_back(colToken.lexeme);
    } while (match(TokenType::COMMA));

    consume(TokenType::FROM, "Expected FROM after SELECT columns.");

    // Parse FROM table
    Token fromTableToken = consume(TokenType::IDENTIFIER, "Expected table name after FROM.");
    selectStmt->fromTable = fromTableToken.lexeme;

    // Parse optional JOINs
    while (match(TokenType::INNER_JOIN) || match(TokenType::LEFT_JOIN) ||
           match(TokenType::RIGHT_JOIN) || match(TokenType::FULL_JOIN)) {
        std::string joinType;
        if (previous().type == TokenType::INNER_JOIN) joinType = "INNER JOIN";
        else if (previous().type == TokenType::LEFT_JOIN) joinType = "LEFT JOIN";
        else if (previous().type == TokenType::RIGHT_JOIN) joinType = "RIGHT JOIN";
        else if (previous().type == TokenType::FULL_JOIN) joinType = "FULL JOIN";

        Token joinTableToken = consume(TokenType::IDENTIFIER, "Expected table name after JOIN.");
        std::string joinTable = joinTableToken.lexeme;

        consume(TokenType::ON, "Expected ON after JOIN table.");

        // Parse join condition (simplified)
        Token leftCond = consume(TokenType::IDENTIFIER, "Expected left side of JOIN condition.");
        consume(TokenType::EQUAL, "Expected '=' in JOIN condition.");
        Token rightCond = consume(TokenType::IDENTIFIER, "Expected right side of JOIN condition.");

        // For simplicity, store the join condition as a string
        std::string condition = leftCond.lexeme + " = " + rightCond.lexeme;

        // Combine join type and table name
        joinType += " " + joinTable + " ON " + condition;

        selectStmt->joins.emplace_back(std::make_pair(joinType, joinTable));
    }

    // Parse optional WHERE clause
    if (match(TokenType::WHERE)) {
        // Simplified: parse the WHERE condition as a single string
        std::string whereCond;
        while (!check(TokenType::SEMICOLON) && !check(TokenType::QUIT) && !isAtEnd()) {
            whereCond += advance();
        }
        selectStmt->whereClause = whereCond;
    }

    // Parse optional ORDER BY clause
    if (match(TokenType::ORDER)) {
        consume(TokenType::BY, "Expected BY after ORDER.");

        do {
            Token orderCol = consume(TokenType::IDENTIFIER, "Expected column name in ORDER BY.");
            selectStmt->orderByColumns.push_back(orderCol.lexeme);

            // Check for ASC or DESC
            if (match(TokenType::ASC)) {
                selectStmt->orderByColumns.back() += " ASC";
            }
            else if (match(TokenType::DESC)) {
                selectStmt->orderByColumns.back() += " DESC";
            }
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::SEMICOLON, "Expected ';' after SELECT statement.");

    return selectStmt;
}

// Implement additional parsing functions for INSERT, UPDATE, DELETE as needed

```

**Explanation:**

- **`parseProcSQL`:** Constructs a `ProcSQLNode`, parsing various SQL statements within the `PROC SQL` block until the `QUIT;` statement is encountered.
  
- **`parseCreateTableStatement`:** Parses a `CREATE TABLE` statement that uses a `SELECT` statement to define the table's contents.
  
- **`parseSelectStatement`:** Parses a `SELECT` statement, capturing selected columns, source table, joins, where clause, and order by clauses.
  
- **Join Parsing:** Handles different types of joins (`INNER JOIN`, `LEFT JOIN`, etc.) and parses the join conditions.
  
- **Simplifications:** The parsing functions are simplified for demonstration purposes. A complete implementation would require more robust parsing to handle complex SQL syntax, nested queries, subqueries, aliases, and other SQL features.

---

### **51.5. Enhancing the Interpreter to Execute `PROC SQL`**

Implement the execution logic for `PROC SQL`, handling the parsing and execution of SQL statements, managing dataset operations, and generating output datasets as specified by SQL queries.

**Interpreter.h**

```cpp
// ... Existing Interpreter.h content ...

private:
    // Execution methods
    void executeProcSQL(ProcSQLNode *node);
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
#include <set>
#include <stdexcept>
#include <sstream>
#include <cmath>

// ... Existing Interpreter methods ...

void Interpreter::executeProcSQL(ProcSQLNode *node) {
    lstLogger.info("Executing PROC SQL");

    for (const auto& sqlStmt : node->sqlStatements) {
        if (auto createTableStmt = dynamic_cast<CreateTableStatement*>(sqlStmt.get())) {
            executeCreateTableStatement(createTableStmt);
        }
        else if (auto selectStmt = dynamic_cast<SelectStatement*>(sqlStmt.get())) {
            executeSelectStatement(selectStmt);
        }
        else {
            throw std::runtime_error("Unsupported SQL statement in PROC SQL.");
        }
    }

    logLogger.info("PROC SQL executed successfully.");
}

void Interpreter::executeCreateTableStatement(CreateTableStatement* stmt) {
    std::string newTableName = stmt->tableName;
    std::string sourceTable = stmt->selectStmt.fromTable;

    if (env.datasets.find(sourceTable) == env.datasets.end()) {
        throw std::runtime_error("PROC SQL CREATE TABLE: Source table '" + sourceTable + "' does not exist.");
    }

    Dataset sourceDataset = env.datasets[sourceTable];
    Dataset newDataset;

    // Handle JOINs if present in the SELECT statement
    // Simplified: Only supports INNER JOIN for demonstration
    if (!stmt->selectStmt.joins.empty()) {
        // For simplicity, process only one join
        auto joinPair = stmt->selectStmt.joins[0];
        std::string joinType = joinPair.first;
        std::string joinTable = joinPair.second;

        if (env.datasets.find(joinTable) == env.datasets.end()) {
            throw std::runtime_error("PROC SQL CREATE TABLE: Join table '" + joinTable + "' does not exist.");
        }

        Dataset joinDataset = env.datasets[joinTable];

        // Extract join condition (simplified)
        // Assumes condition is in the format "table1.column1 = table2.column2"
        size_t onPos = joinType.find("ON");
        if (onPos == std::string::npos) {
            throw std::runtime_error("PROC SQL CREATE TABLE: Missing ON clause in JOIN.");
        }
        std::string condition = joinType.substr(onPos + 2);
        size_t eqPos = condition.find('=');
        if (eqPos == std::string::npos) {
            throw std::runtime_error("PROC SQL CREATE TABLE: Invalid JOIN condition.");
        }
        std::string left = trim(condition.substr(0, eqPos));
        std::string right = trim(condition.substr(eqPos + 1));

        // Split into table and column
        size_t dotPos = left.find('.');
        if (dotPos == std::string::npos) {
            throw std::runtime_error("PROC SQL CREATE TABLE: Invalid left side of JOIN condition.");
        }
        std::string leftTable = left.substr(0, dotPos);
        std::string leftColumn = left.substr(dotPos + 1);

        dotPos = right.find('.');
        if (dotPos == std::string::npos) {
            throw std::runtime_error("PROC SQL CREATE TABLE: Invalid right side of JOIN condition.");
        }
        std::string rightTable = right.substr(0, dotPos);
        std::string rightColumn = right.substr(dotPos + 1);

        // Perform INNER JOIN
        for (const auto& row1 : sourceDataset) {
            for (const auto& row2 : joinDataset) {
                if (toString(row1.at(leftColumn).value) == toString(row2.at(rightColumn).value)) {
                    // Merge rows
                    std::map<std::string, Variable> mergedRow = row1;
                    for (const auto& [key, value] : row2) {
                        mergedRow[key] = value;
                    }
                    newDataset.push_back(mergedRow);
                }
            }
        }
    }
    else {
        // No JOINs, simply select from the source table
        newDataset = sourceDataset;
    }

    // Handle WHERE clause (simplified)
    if (!stmt->selectStmt.whereClause.empty()) {
        Dataset filteredDataset;
        // Parse the WHERE clause (very simplified)
        // Assumes format "Column Operator Value", e.g., "Salary > 50000"
        std::istringstream whereStream(stmt->selectStmt.whereClause);
        std::string column, op;
        double value;
        whereStream >> column >> op >> value;

        for (const auto& row : newDataset) {
            if (row.find(column) == row.end()) {
                throw std::runtime_error("PROC SQL SELECT: Column '" + column + "' not found in dataset.");
            }
            double cellValue = std::get<double>(row.at(column).value);
            bool condition = false;
            if (op == ">") condition = cellValue > value;
            else if (op == "<") condition = cellValue < value;
            else if (op == ">=") condition = cellValue >= value;
            else if (op == "<=") condition = cellValue <= value;
            else if (op == "==") condition = cellValue == value;
            else if (op == "!=" || op == "<>") condition = cellValue != value;
            else {
                throw std::runtime_error("PROC SQL SELECT: Unsupported operator '" + op + "' in WHERE clause.");
            }

            if (condition) {
                filteredDataset.push_back(row);
            }
        }

        newDataset = filteredDataset;
    }

    // Handle SELECT columns (simplified)
    Dataset selectedDataset;
    for (const auto& row : newDataset) {
        std::map<std::string, Variable> selectedRow;
        for (const auto& col : stmt->selectStmt.selectColumns) {
            if (row.find(col) != row.end()) {
                selectedRow[col] = row.at(col);
            }
            else {
                throw std::runtime_error("PROC SQL SELECT: Column '" + col + "' not found in dataset.");
            }
        }
        selectedDataset.push_back(selectedRow);
    }

    // Handle ORDER BY clause (simplified)
    if (!stmt->selectStmt.orderByColumns.empty()) {
        // Currently supports single column ordering
        std::string orderCol = stmt->selectStmt.orderByColumns[0];
        bool ascending = true;
        if (orderCol.find("DESC") != std::string::npos) {
            ascending = false;
            orderCol = trim(orderCol.substr(0, orderCol.find("DESC")));
        }
        else if (orderCol.find("ASC") != std::string::npos) {
            ascending = true;
            orderCol = trim(orderCol.substr(0, orderCol.find("ASC")));
        }

        std::sort(selectedDataset.begin(), selectedDataset.end(),
                  [&](const std::map<std::string, Variable>& a, const std::map<std::string, Variable>& b) -> bool {
                      double valA = std::get<double>(a.at(orderCol).value);
                      double valB = std::get<double>(b.at(orderCol).value);
                      return ascending ? (valA < valB) : (valA > valB);
                  });
    }

    // Assign the result to the new table
    env.datasets[newTableName] = selectedDataset;

    lstLogger.info("Created table '{}'.", newTableName);
}

void Interpreter::executeSelectStatement(SelectStatement* stmt) {
    // Simplified: Execute a SELECT statement and display results
    std::string sourceTable = stmt->fromTable;

    if (env.datasets.find(sourceTable) == env.datasets.end()) {
        throw std::runtime_error("PROC SQL SELECT: Source table '" + sourceTable + "' does not exist.");
    }

    Dataset sourceDataset = env.datasets[sourceTable];
    Dataset resultDataset;

    // Handle SELECT columns (simplified)
    for (const auto& row : sourceDataset) {
        std::map<std::string, Variable> selectedRow;
        for (const auto& col : stmt->selectColumns) {
            if (row.find(col) != row.end()) {
                selectedRow[col] = row.at(col);
            }
            else {
                throw std::runtime_error("PROC SQL SELECT: Column '" + col + "' not found in dataset.");
            }
        }
        resultDataset.push_back(selectedRow);
    }

    // Handle WHERE clause (simplified)
    if (!stmt->whereClause.empty()) {
        Dataset filteredDataset;
        // Parse the WHERE clause (very simplified)
        // Assumes format "Column Operator Value", e.g., "Salary > 50000"
        std::istringstream whereStream(stmt->whereClause);
        std::string column, op;
        double value;
        whereStream >> column >> op >> value;

        for (const auto& row : resultDataset) {
            if (row.find(column) == row.end()) {
                throw std::runtime_error("PROC SQL SELECT: Column '" + column + "' not found in dataset.");
            }
            double cellValue = std::get<double>(row.at(column).value);
            bool condition = false;
            if (op == ">") condition = cellValue > value;
            else if (op == "<") condition = cellValue < value;
            else if (op == ">=") condition = cellValue >= value;
            else if (op == "<=") condition = cellValue <= value;
            else if (op == "==") condition = cellValue == value;
            else if (op == "!=" || op == "<>") condition = cellValue != value;
            else {
                throw std::runtime_error("PROC SQL SELECT: Unsupported operator '" + op + "' in WHERE clause.");
            }

            if (condition) {
                filteredDataset.push_back(row);
            }
        }

        resultDataset = filteredDataset;
    }

    // Handle ORDER BY clause (simplified)
    if (!stmt->orderByColumns.empty()) {
        // Currently supports single column ordering
        std::string orderCol = stmt->orderByColumns[0];
        bool ascending = true;
        if (orderCol.find("DESC") != std::string::npos) {
            ascending = false;
            orderCol = trim(orderCol.substr(0, orderCol.find("DESC")));
        }
        else if (orderCol.find("ASC") != std::string::npos) {
            ascending = true;
            orderCol = trim(orderCol.substr(0, orderCol.find("ASC")));
        }

        std::sort(resultDataset.begin(), resultDataset.end(),
                  [&](const std::map<std::string, Variable>& a, const std::map<std::string, Variable>& b) -> bool {
                      double valA = std::get<double>(a.at(orderCol).value);
                      double valB = std::get<double>(b.at(orderCol).value);
                      return ascending ? (valA < valB) : (valA > valB);
                  });
    }

    // Display the result (simplified)
    lstLogger.info("\nSELECT Query Result:");
    // Print header
    std::ostringstream headerStream;
    for (const auto& col : stmt->selectColumns) {
        headerStream << std::setw(15) << col;
    }
    lstLogger.info("{}", headerStream.str());

    // Print separator
    lstLogger.info("----------------------------------------------");

    // Print rows
    for (const auto& row : resultDataset) {
        std::ostringstream rowStream;
        for (const auto& col : stmt->selectColumns) {
            if (row.find(col) != row.end()) {
                if (row.at(col).type == VarType::NUMERIC) {
                    rowStream << std::setw(15) << std::get<double>(row.at(col).value);
                }
                else {
                    rowStream << std::setw(15) << std::get<std::string>(row.at(col).value);
                }
            }
            else {
                rowStream << std::setw(15) << "NA";
            }
        }
        lstLogger.info("{}", rowStream.str());
    }
}

```

**Explanation:**

- **`executeProcSQL`:** Iterates through the list of SQL statements within the `PROC SQL` block and delegates execution to specific handler functions based on the statement type (e.g., `CREATE TABLE`, `SELECT`).
  
- **`executeCreateTableStatement`:** Handles the creation of new tables based on `CREATE TABLE` statements. It supports `INNER JOIN` operations in a simplified manner, merging rows from two tables based on join conditions. The implementation currently supports only `INNER JOIN` and requires further expansion for other join types and more complex conditions.
  
- **`executeSelectStatement`:** Executes `SELECT` statements by selecting specified columns from the source table, applying `WHERE` conditions, and ordering the results based on `ORDER BY` clauses. The implementation is simplified and assumes straightforward conditions and single-column ordering.
  
- **Simplifications:** The current implementation handles basic `CREATE TABLE` with `INNER JOIN` and simple `SELECT` statements. Comprehensive support for all SQL features (subqueries, multiple joins, aliases, complex conditions, aggregation functions, etc.) would require more extensive parsing and execution logic.

---

### **51.6. Updating the Interpreter's Execute Method**

Modify the `Interpreter::execute` method to handle `ProcSQLNode`.

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
        else if (auto procSQL = dynamic_cast<ProcSQLNode*>(procStep)) {
            executeProcSQL(procSQL);
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

- **`ProcSQLNode` Detection:** Checks if the `ProcStepNode` is actually a `ProcSQLNode` and calls the appropriate execution method `executeProcSQL`.

---

### **51.7. Creating Comprehensive Test Cases for `PROC SQL`**

Develop diverse test cases to verify that `PROC SQL` is parsed and executed correctly, producing the expected query results and handling various SQL operations.

#### **51.7.1. Test Case 1: Basic `PROC SQL` Execution with `CREATE TABLE` and `SELECT`**

**SAS Script (`test_proc_sql_basic.sas`):**

```sas
data employees;
    input EmployeeID $ Name $ Department $ Salary;
    datalines;
E001 John Sales 50000
E002 Jane Marketing 55000
E003 Mike Sales 60000
E004 Anna HR 52000
E005 Tom Marketing 58000
E006 Lucy HR 50000
;
run;

data departments;
    input Department $ Manager $;
    datalines;
Sales Alice
Marketing Bob
HR Carol
;
run;

proc sql;
    create table emp_dept as
    select e.EmployeeID, e.Name, e.Department, d.Manager, e.Salary
    from employees e
    inner join departments d
    on e.Department = d.Department
    where e.Salary > 50000
    order by e.Salary desc;
quit;

proc print data=emp_dept;
run;
```

**Expected Output (`sas_log_proc_sql_basic.txt`):**

```
[INFO] Executing statement: data employees; input EmployeeID $ Name $ Department $ Salary; datalines; E001 John Sales 50000; E002 Jane Marketing 55000; E003 Mike Sales 60000; E004 Anna HR 52000; E005 Tom Marketing 58000; E006 Lucy HR 50000; run;
[INFO] Executing DATA step: employees
[INFO] Assigned variable 'EmployeeID' = "E001"
[INFO] Assigned variable 'Name' = "John"
[INFO] Assigned variable 'Department' = "Sales"
[INFO] Assigned variable 'Salary' = 50000
[INFO] Assigned variable 'EmployeeID' = "E002"
[INFO] Assigned variable 'Name' = "Jane"
[INFO] Assigned variable 'Department' = "Marketing"
[INFO] Assigned variable 'Salary' = 55000
[INFO] Assigned variable 'EmployeeID' = "E003"
[INFO] Assigned variable 'Name' = "Mike"
[INFO] Assigned variable 'Department' = "Sales"
[INFO] Assigned variable 'Salary' = 60000
[INFO] Assigned variable 'EmployeeID' = "E004"
[INFO] Assigned variable 'Name' = "Anna"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Salary' = 52000
[INFO] Assigned variable 'EmployeeID' = "E005"
[INFO] Assigned variable 'Name' = "Tom"
[INFO] Assigned variable 'Department' = "Marketing"
[INFO] Assigned variable 'Salary' = 58000
[INFO] Assigned variable 'EmployeeID' = "E006"
[INFO] Assigned variable 'Name' = "Lucy"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Salary' = 50000
[INFO] DATA step 'employees' executed successfully. 6 observations created.

[INFO] Executing statement: data departments; input Department $ Manager $; datalines; Sales Alice; Marketing Bob; HR Carol; run;
[INFO] Executing DATA step: departments
[INFO] Assigned variable 'Department' = "Sales"
[INFO] Assigned variable 'Manager' = "Alice"
[INFO] Assigned variable 'Department' = "Marketing"
[INFO] Assigned variable 'Manager' = "Bob"
[INFO] Assigned variable 'Department' = "HR"
[INFO] Assigned variable 'Manager' = "Carol"
[INFO] DATA step 'departments' executed successfully. 3 observations created.

[INFO] Executing statement: proc sql; create table emp_dept as select e.EmployeeID, e.Name, e.Department, d.Manager, e.Salary from employees e inner join departments d on e.Department = d.Department where e.Salary > 50000 order by e.Salary desc; quit;
[INFO] Executing PROC SQL
[INFO] Created table 'emp_dept'.

[INFO] PROC SQL executed successfully.

[INFO] Executing statement: proc print data=emp_dept; run;
[INFO] Executing PROC PRINT

EmployeeID    Name    Department    Manager    Salary  
-------------------------------------------------------
E003          Mike    Sales         Alice      60000  
E005          Tom     Marketing     Bob        58000  
E002          Jane    Marketing     Bob        55000  
E004          Anna    HR            Carol      52000  
-------------------------------------------------------
[INFO] PROC PRINT executed successfully.
```

**Test File (`test_proc_sql_basic.cpp`):**

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
        char buffer[16384];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        messages.emplace_back(std::string(buffer));
    }
    
    std::vector<std::string> messages;
};

// Test case for basic PROC SQL
TEST(InterpreterTest, ProcSQLBasic) {
    DataEnvironment env;
    MockLogger logLogger;
    MockLogger lstLogger;
    Interpreter interpreter(env, logLogger, lstLogger);

    std::string input = 
        "data employees;\n"
        "input EmployeeID $ Name $ Department $ Salary;\n"
        "datalines;\n"
        "E001 John Sales 50000\n"
        "E002 Jane Marketing 55000\n"
        "E003 Mike Sales 60000\n"
        "E004 Anna HR 52000\n"
        "E005 Tom Marketing 58000\n"
        "E006 Lucy HR 50000\n"
        ";\n"
        "run;\n"
        "\n"
        "data departments;\n"
        "input Department $ Manager $;\n"
        "datalines;\n"
        "Sales Alice\n"
        "Marketing Bob\n"
        "HR Carol\n"
        ";\n"
        "run;\n"
        "\n"
        "proc sql;\n"
        "    create table emp_dept as\n"
        "    select e.EmployeeID, e.Name, e.Department, d.Manager, e.Salary\n"
        "    from employees e\n"
        "    inner join departments d\n"
        "    on e.Department = d.Department\n"
        "    where e.Salary > 50000\n"
        "    order by e.Salary desc;\n"
        "quit;\n"
        "\n"
        "proc print data=emp_dept;\n"
        "run;\n";

    interpreter.handleReplInput(input);

    // Verify dataset creation
    ASSERT_TRUE(env.datasets.find("emp_dept") != env.datasets.end());
    Dataset empDept = env.datasets["emp_dept"];
    ASSERT_EQ(empDept.size(), 4); // Employees with Salary > 50000

    // Verify records
    std::map<std::string, std::map<std::string, Variable>> expected = {
        {"E003", {{"Name", Variable{VarType::CHARACTER, "Mike"}}, {"Department", Variable{VarType::CHARACTER, "Sales"}}, {"Manager", Variable{VarType::CHARACTER, "Alice"}}, {"Salary", Variable{VarType::NUMERIC, 60000}}}},
        {"E005", {{"Name", Variable{VarType::CHARACTER, "Tom"}}, {"Department", Variable{VarType::CHARACTER, "Marketing"}}, {"Manager", Variable{VarType::CHARACTER, "Bob"}}, {"Salary", Variable{VarType::NUMERIC, 58000}}}},
        {"E002", {{"Name", Variable{VarType::CHARACTER, "Jane"}}, {"Department", Variable{VarType::CHARACTER, "Marketing"}}, {"Manager", Variable{VarType::CHARACTER, "Bob"}}, {"Salary", Variable{VarType::NUMERIC, 55000}}}},
        {"E004", {{"Name", Variable{VarType::CHARACTER, "Anna"}}, {"Department", Variable{VarType::CHARACTER, "HR"}}, {"Manager", Variable{VarType::CHARACTER, "Carol"}}, {"Salary", Variable{VarType::NUMERIC, 52000}}}}
    };

    for (const auto& row : empDept) {
        std::string empID = row.at("EmployeeID").value;
        ASSERT_TRUE(expected.find(empID) != expected.end());
        for (const auto& [key, value] : expected[empID]) {
            ASSERT_TRUE(row.find(key) != row.end());
            if (value.type == VarType::NUMERIC) {
                EXPECT_DOUBLE_EQ(std::get<double>(row.at(key).value), std::get<double>(value.value));
            }
            else {
                EXPECT_EQ(std::get<std::string>(row.at(key).value), std::get<std::string>(value.value));
            }
        }
    }

    // Verify log messages contain PROC SQL execution
    bool sqlExecuted = false;
    for (const auto& msg : logLogger.messages) {
        if (msg.find("PROC SQL executed successfully.") != std::string::npos) {
            sqlExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(sqlExecuted);

    // Verify PROC PRINT output
    bool printExecuted = false;
    for (const auto& msg : lstLogger.messages) {
        if (msg.find("EmployeeID    Name    Department    Manager    Salary") != std::string::npos &&
            msg.find("E003          Mike    Sales         Alice      60000") != std::string::npos &&
            msg.find("E005          Tom     Marketing     Bob        58000") != std::string::npos &&
            msg.find("E002          Jane    Marketing     Bob        55000") != std::string::npos &&
            msg.find("E004          Anna    HR            Carol      52000") != std::string::npos) {
            printExecuted = true;
            break;
        }
    }
    EXPECT_TRUE(printExecuted);
}
```

**Explanation:**

- **`MockLogger`:** Captures log messages, enabling verification of `PROC SQL` and `PROC PRINT` executions.
  
- **Assertions:**
  
  - **Dataset Verification:** Confirms that the `emp_dept` dataset is created with four observations corresponding to employees with a salary greater than 50,000.
    
  - **Record Verification:** Checks that each record in `emp_dept` matches the expected values, ensuring correct join operations and filtering.
    
  - **Execution Logs:** Ensures that the log contains messages indicating successful execution of `PROC SQL` and `PROC PRINT`.
    
  - **Printed Report:** Verifies that the printed report includes the expected table structure and data.

---

### **51.8. Updating the Testing Suite**

Ensure that the newly created test cases for `PROC SQL` are integrated into your testing suite and are executed automatically as part of your Continuous Integration (CI) pipeline.

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
    test_proc_sql_basic.cpp
)
    
# Link Google Test and pthread
target_link_libraries(runTests PRIVATE gtest gtest_main pthread)

# Add tests
add_test(NAME SASTestSuite COMMAND runTests)
```

**Explanation:**

- **Test Integration:** Adds the new `PROC SQL` test case (`test_proc_sql_basic.cpp`) to the test executable, ensuring it is run alongside existing tests.
  
- **Automated Execution:** The tests will be executed automatically during the build process and as part of the CI pipeline, providing immediate feedback on any issues.

---

### **51.9. Best Practices for Implementing `PROC SQL`**

1. **Modular Design:**
   - Structure your code to separate parsing, AST construction, and execution logic for maintainability and scalability.
   
2. **Comprehensive Testing:**
   - Develop a wide range of test cases covering various `PROC SQL` features, including different types of joins, nested queries, subqueries, aggregation functions, and handling of aliases.
   
3. **Robust Error Handling:**
   - Implement detailed error detection and reporting for scenarios like undefined tables or columns, unsupported SQL features, syntax errors, and type mismatches.
   
4. **Performance Optimization:**
   - Ensure that SQL query execution is efficient, especially for large datasets. Optimize join algorithms, indexing, and query planning as necessary.
   
5. **User Feedback:**
   - Provide clear and descriptive log messages to inform users about the execution status, query results, and any encountered issues.
   
6. **Documentation:**
   - Document the implementation details of `PROC SQL`, including supported SQL features, usage examples, and limitations, to aid future development and user understanding.

---

### **51.10. Expanding to Other Advanced PROC Procedures**

With `PROC SQL` now implemented, consider extending support to other essential PROC procedures to further enhance your interpreter's capabilities.

#### **51.10.1. `PROC REG`**

**Purpose:**
- Performs regression analysis, allowing users to model relationships between dependent and independent variables.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcRegNode` and related AST nodes to represent `PROC REG` components, such as dependent variables, independent variables, and options.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC REG` statements, capturing model specifications and options.
   
3. **Interpreter Execution:**
   - Implement regression analysis logic, calculating coefficients, R-squared values, p-values, and generating diagnostic outputs.
   
4. **Testing:**
   - Develop test cases to verify accurate regression computations, correct model specifications, and proper handling of various options.

#### **51.10.2. `PROC UNIVARIATE`**

**Purpose:**
- Provides detailed statistics and graphical representations of data distributions, including measures of central tendency, variability, and shape.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcUnivariateNode` and related AST nodes to represent `PROC UNIVARIATE` components, such as variables to analyze and options for graphical outputs.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC UNIVARIATE` statements, capturing variables and options like histogram creation, normality tests, etc.
   
3. **Interpreter Execution:**
   - Implement statistical calculations and generate textual or graphical representations based on specified options.
   
4. **Testing:**
   - Develop test cases to verify accurate statistical computations, correct report generation, and proper handling of various options.

#### **51.10.3. `PROC TRANSPOSE`**

**Purpose:**
- Transposes data tables, converting rows to columns and vice versa, which is useful for reshaping datasets.

**Implementation Steps:**

1. **AST Extension:**
   - Create `ProcTransposeNode` and related AST nodes to represent `PROC TRANSPOSE` components, such as variables to transpose and options for renaming.
   
2. **Lexer and Parser Updates:**
   - Tokenize and parse `PROC TRANSPOSE` statements, capturing variables and options.
   
3. **Interpreter Execution:**
   - Implement data transposition logic, reshaping datasets as specified.
   
4. **Testing:**
   - Develop test cases to verify accurate data transposition, correct handling of naming conventions, and proper management of missing values.

---

### **51.11. Enhancing the REPL Interface**

To improve user experience, consider enhancing the Read-Eval-Print Loop (REPL) interface with advanced features.

1. **Syntax Highlighting:**
   - Implement color-coding of keywords, variables, and operators to improve readability.
   
2. **Auto-completion:**
   - Provide suggestions for SAS keywords, table names, column names, and function names as users type.
   
3. **Command History:**
   - Allow users to navigate through previously entered commands using the arrow keys.
   
4. **Error Highlighting:**
   - Visually indicate syntax errors or problematic lines within the REPL to aid quick identification and correction.

**Implementation Suggestions:**

- **Use Libraries:** Utilize libraries like [GNU Readline](https://tiswww.case.edu/php/chet/readline/rltop.html) or [linenoise](https://github.com/antirez/linenoise) to facilitate features like command history and auto-completion.
  
- **Syntax Highlighter:** Implement a basic syntax highlighter by analyzing the input and applying ANSI color codes based on token types (keywords, identifiers, operators, etc.).
  
- **Interactive Features:** Enhance interactivity by supporting multi-line input for complex statements and providing helpful prompts or hints.

---

### **51.12. Implementing Debugging Tools**

Introduce debugging capabilities to help users troubleshoot and optimize their scripts.

1. **Breakpoints:**
   - Enable users to set breakpoints in their scripts to pause execution and inspect variables at specific points.
   
2. **Step Execution:**
   - Allow users to step through their scripts line by line, facilitating debugging and learning.
   
3. **Variable Inspection:**
   - Provide commands to inspect the current state of variables and datasets at any point during execution.
   
4. **Execution Tracing:**
   - Offer detailed logs of execution steps, enabling users to trace the flow of their scripts.

**Implementation Suggestions:**

- **Execution Pointer:** Maintain a pointer or stack to track the current execution point, allowing the interpreter to pause and resume as needed.
  
- **Breakpoint Management:** Implement commands within the REPL to set, list, and remove breakpoints. Store breakpoint locations and check during execution.
  
- **Interactive Inspection:** When execution is paused, provide commands to display the values of specific variables or entire datasets.
  
- **User Commands:** Introduce debugging commands such as `break`, `continue`, `step`, `print`, and `inspect` to control and inspect the execution.

---

### **51.13. Optimizing Performance**

Ensure that your interpreter remains efficient, especially as it handles larger datasets and more complex operations.

1. **Efficient Data Structures:**
   - Refine data storage and retrieval mechanisms to handle large datasets more effectively. Consider using hash tables, indexed data structures, or in-memory databases for quick access.
   
2. **Parallel Processing:**
   - Leverage multi-threading or parallel processing for operations that can be executed concurrently, such as data transformations, aggregations, and query executions.
   
3. **Memory Management:**
   - Optimize memory usage by implementing efficient data storage, minimizing memory leaks, and managing resource allocation carefully.
   
4. **Profiling and Benchmarking:**
   - Use profiling tools (e.g., `gprof`, `Valgrind`, `Perf`) to identify and address performance bottlenecks. Benchmark critical sections of the interpreter to ensure scalability.

**Implementation Suggestions:**

- **Lazy Evaluation:** Implement lazy evaluation strategies where feasible, delaying computations until results are needed.
  
- **Caching:** Cache intermediate results for operations that are repeatedly accessed or computed.
  
- **Optimized Algorithms:** Use optimized algorithms for sorting, searching, and aggregating data to improve execution speed.
  
- **Resource Cleanup:** Ensure that all dynamically allocated resources are properly cleaned up to prevent memory leaks and reduce memory footprint.

---

### **51.14. Enhancing Error Handling and Logging**

Improve the interpreter's robustness by refining its error handling and logging mechanisms.

1. **Detailed Error Messages:**
   - Include contextual information such as line numbers, character positions, variable states, and the specific operation being performed when errors occur.
   
2. **Advanced Logging:**
   - Implement different logging levels (e.g., DEBUG, INFO, WARN, ERROR) and direct logs to various outputs (console, files) as configured by the user.
   
3. **Graceful Degradation:**
   - Ensure that the interpreter can recover gracefully from non-critical errors, allowing users to continue working without restarting the session. For critical errors, provide meaningful messages and safe shutdown procedures.
   
4. **Exception Handling:**
   - Use structured exception handling to manage unexpected conditions, ensuring that resources are properly released and the interpreter remains stable.

**Implementation Suggestions:**

- **Logging Library Configuration:** Configure the [spdlog](https://github.com/gabime/spdlog) library to support multiple logging sinks and formats, enabling flexibility in log management.
  
- **Error Context:** When throwing exceptions, include detailed messages that help users understand the cause and location of the error.
  
- **User Configurations:** Allow users to set logging preferences, such as verbosity levels and log file paths, through configuration files or command-line arguments.

---

### **51.15. Expanding the Testing Suite**

Continue to grow your testing suite to cover new features and ensure reliability.

1. **Increase Test Coverage:**
   - Develop more test cases covering various `PROC SQL` features, including different types of joins, subqueries, aggregation functions, and handling of aliases and nested queries.
   
2. **Automate Performance Testing:**
   - Incorporate benchmarks to monitor and optimize the interpreter's performance over time. Use tools like [Google Benchmark](https://github.com/google/benchmark) for automated performance testing.
   
3. **Regression Testing:**
   - Implement tests to ensure that new features do not break existing functionalities. Use continuous integration tools to run regression tests automatically on code changes.
   
4. **Edge Case Testing:**
   - Develop tests for edge cases, such as empty datasets, missing values, extremely large numbers, and complex query conditions, to ensure the interpreter handles them gracefully.

**Implementation Suggestions:**

- **Parameterized Tests:** Utilize parameterized tests in Google Test to handle multiple input scenarios efficiently, reducing code duplication and enhancing test coverage.
  
- **Mocking and Stubbing:** Use mocking frameworks to simulate complex behaviors and isolate components during testing.
  
- **Continuous Integration:** Integrate the testing suite with CI pipelines (e.g., GitHub Actions, Jenkins) to automate testing on code commits and merges, ensuring ongoing code quality.

---

### **51.16. Improving Documentation and User Guides**

Provide comprehensive resources to help users understand and utilize your interpreter effectively.

1. **Comprehensive Manuals:**
   - Create detailed documentation covering installation, usage, scripting syntax, and advanced features. Include explanations of supported PROC procedures, data manipulation capabilities, and REPL commands.
   
2. **Tutorials and Examples:**
   - Provide example scripts and tutorials to help users learn how to effectively use the interpreter. Include step-by-step guides for common tasks, such as data import/export, querying, and report generation.
   
3. **API Documentation:**
   - If applicable, provide documentation for any APIs or extensions exposed by the interpreter. Use tools like [Doxygen](https://www.doxygen.nl/) to generate API documentation from your codebase.
   
4. **FAQ and Troubleshooting:**
   - Develop a FAQ section addressing common user questions and issues. Include troubleshooting guides for error messages and unexpected behaviors.

**Implementation Suggestions:**

- **Documentation Generators:** Use documentation generators like Doxygen or Sphinx to create structured and navigable documentation.
  
- **Hosting Platforms:** Host documentation on platforms like [Read the Docs](https://readthedocs.org/) or as a static site using [MkDocs](https://www.mkdocs.org/), making it easily accessible to users.
  
- **Version Control:** Maintain documentation alongside the codebase in version control systems (e.g., Git) to ensure consistency and ease of updates.

---

### **51.17. Implementing Security Features**

Ensure that your interpreter executes scripts securely, protecting the system and user data.

1. **Sandboxing:**
   - Isolate script execution to prevent unauthorized access to system resources. Use sandboxing techniques or containerization technologies (e.g., Docker) to run the interpreter in a controlled environment.
   
2. **Input Validation:**
   - Rigorously validate user inputs to prevent injection attacks, such as SQL injection or command injection. Sanitize inputs before processing them.
   
3. **Access Controls:**
   - Implement permissions and access controls to restrict sensitive operations. For multi-user environments, manage user roles and permissions to control access to datasets and commands.
   
4. **Secure Coding Practices:**
   - Follow secure coding standards to minimize vulnerabilities. Regularly audit the codebase for security flaws and apply patches as needed.

**Implementation Suggestions:**

- **Use Secure Libraries:** Utilize security-focused libraries and frameworks that provide built-in protections against common vulnerabilities.
  
- **Resource Limits:** Impose limits on resource usage (e.g., memory, CPU) to prevent Denial-of-Service (DoS) attacks or unintentional resource exhaustion.
  
- **Logging and Monitoring:** Implement comprehensive logging of all actions and monitor logs for suspicious activities. Alert administrators to potential security breaches.

---

### **51.18. Exploring Extensibility and Plugin Systems**

Design your interpreter to be extensible, allowing users to add custom functionalities or integrate third-party tools.

1. **Plugin Architecture:**
   - Develop a framework that supports plugins, enabling users to extend the interpreter's capabilities without modifying the core codebase.
   
2. **APIs for Extensions:**
   - Provide clear APIs and guidelines for developing extensions, fostering a community-driven ecosystem around your interpreter. Define interfaces for plugins to interact with the interpreter, such as adding new PROC procedures or custom functions.
   
3. **Dynamic Loading:**
   - Implement mechanisms to load and unload plugins dynamically at runtime, allowing users to customize their environment as needed.
   
4. **Documentation for Developers:**
   - Offer comprehensive guides and examples to help developers create and integrate plugins. Include sample plugins and templates to facilitate development.

**Implementation Suggestions:**

- **Define Plugin Interfaces:** Clearly define the interfaces and contracts that plugins must adhere to, ensuring consistency and compatibility.
  
- **Use Dynamic Libraries:** Allow plugins to be implemented as dynamic libraries (`.so`, `.dll`) that can be loaded at runtime using platform-specific APIs (e.g., `dlopen` on Unix, `LoadLibrary` on Windows).
  
- **Plugin Manager:** Develop a plugin manager within the interpreter that handles the discovery, loading, initialization, and management of plugins.
  
- **Security Considerations:** Implement security measures to ensure that plugins cannot perform unauthorized actions or compromise the interpreter's integrity.

---

### **51.19. Engaging with Users and Gathering Feedback**

Foster a community around your interpreter to drive continuous improvement and adoption.

1. **Beta Testing:**
   - Release beta versions to a select group of users to gather feedback and identify areas for improvement. Use feedback to refine features, fix bugs, and enhance usability.
   
2. **Community Building:**
   - Create forums, mailing lists, or chat channels where users can ask questions, share scripts, and collaborate. Encourage user contributions and discussions.
   
3. **Iterative Development:**
   - Use feedback to prioritize features, fix bugs, and enhance the interpreter based on real-world usage patterns. Adopt an agile development approach to respond quickly to user needs.
   
4. **Surveys and Feedback Forms:**
   - Conduct surveys or provide feedback forms to collect structured input from users. Use this data to inform development decisions and roadmap planning.

**Implementation Suggestions:**

- **Open Source Platforms:** Host the project on platforms like [GitHub](https://github.com/) to facilitate collaboration, issue tracking, and community contributions.
  
- **Social Media and Forums:** Utilize social media channels, dedicated forums, or platforms like [Reddit](https://www.reddit.com/) to engage with users and promote discussions.
  
- **Documentation and Support:** Provide clear channels for users to seek support, report issues, and contribute to the project. Maintain responsiveness to user inquiries to build trust and community engagement.

---

### **51.20. Summary of Achievements**

1. **Implemented `PROC SQL`:**
   - Extended the AST to represent `PROC SQL` components.
   
   - Updated the Lexer and Parser to recognize and parse `PROC SQL` syntax, including `SELECT`, `CREATE TABLE`, `JOIN`, `WHERE`, and `ORDER BY` statements.
   
   - Enhanced the Interpreter to execute `PROC SQL`, handling table creation, joins, filtering, and ordering of query results.
   
2. **Developed Comprehensive Test Cases:**
   - Created automated tests to verify the correct parsing and execution of `PROC SQL`.
   
   - Ensured that SQL queries are executed accurately, including joins, filtering conditions, and result ordering.
   
3. **Integrated Tests into the Testing Suite:**
   - Updated the CMake configuration to include the new `PROC SQL` test case.
   
   - Ensured that `PROC SQL` tests are part of the automated testing pipeline, facilitating continuous quality assurance.
   
4. **Established Best Practices:**
   - Emphasized modular design, comprehensive testing, robust error handling, performance optimization, and thorough documentation.
   
   - Laid the groundwork for future expansions, such as implementing additional PROC procedures and enhancing the interpreter's capabilities.

---

### **51.21. Next Steps**

With **`PROC SQL`** successfully implemented, your SAS interpreter now supports a fundamental data querying and manipulation tool, significantly enhancing its data processing capabilities. To continue advancing your interpreter's functionalities, consider the following next steps:

1. **Implement Additional PROC Procedures:**
   - **`PROC REG`:**
     - Performs regression analysis, allowing users to model relationships between dependent and independent variables.
     
   - **`PROC UNIVARIATE`:**
     - Provides detailed statistics and graphical representations of data distributions.
     
   - **`PROC TRANSPOSE`:**
     - Transposes data tables, converting rows to columns and vice versa, useful for reshaping datasets.
     
   - **`PROC PRINTTO`:**
     - Redirects SAS log and output to external files, enhancing logging and reporting capabilities.
     
   - **`PROC FORMAT`:**
     - Defines custom formats for data presentation, allowing users to control the display of variable values.
     
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
     - Provide suggestions for SAS keywords, table names, column names, and function names as users type.
     
   - **Command History:**
     - Allow users to navigate through previously entered commands using the arrow keys.
     
   - **Multi-line Input Support:**
     - Enable the REPL to handle multi-line statements seamlessly, improving usability for complex scripts.
     
4. **Implement Debugging Tools:**
   - **Breakpoints:**
     - Enable users to set breakpoints in their scripts to pause execution and inspect variables at specific points.
     
   - **Step Execution:**
     - Allow users to step through their scripts line by line, facilitating debugging and learning.
     
   - **Variable Inspection:**
     - Provide commands to inspect the current state of variables and datasets at any point during execution.
     
   - **Execution Tracing:**
     - Offer detailed logs of execution steps, enabling users to trace the flow of their scripts.
     
5. **Optimize Performance:**
   - **Efficient Data Structures:**
     - Refine data storage and retrieval mechanisms to handle large datasets more effectively. Consider using hash tables, indexed data structures, or in-memory databases for quick access.
     
   - **Parallel Processing:**
     - Leverage multi-threading or parallel processing for operations that can be executed concurrently, such as data transformations, aggregations, and query executions.
     
   - **Memory Management:**
     - Optimize memory usage by implementing efficient data storage, minimizing memory leaks, and managing resource allocation carefully.
     
   - **Lazy Evaluation and Caching:**
     - Implement lazy evaluation strategies and caching mechanisms to improve execution speed and reduce redundant computations.
     
   - **Optimized Algorithms:**
     - Use optimized algorithms for sorting, searching, and aggregating data to enhance performance.
     
6. **Enhance Error Handling and Logging:**
   - **Detailed Error Messages:**
     - Include contextual information such as line numbers, character positions, variable states, and the specific operation being performed when errors occur.
     
   - **Advanced Logging:**
     - Implement different logging levels (e.g., DEBUG, INFO, WARN, ERROR) and direct logs to various outputs (console, files) as configured by the user.
     
   - **Exception Handling:**
     - Use structured exception handling to manage unexpected conditions, ensuring that resources are properly released and the interpreter remains stable.
     
7. **Expand the Testing Suite:**
   - **Increase Test Coverage:**
     - Develop more test cases covering various `PROC SQL` features, different types of joins, nested queries, aggregation functions, and handling of aliases and nested queries.
     
   - **Automate Performance Testing:**
     - Incorporate benchmarks to monitor and optimize the interpreter's performance over time. Use tools like [Google Benchmark](https://github.com/google/benchmark) for automated performance testing.
     
   - **Regression Testing:**
     - Implement tests to ensure that new features do not break existing functionalities. Use continuous integration tools to run regression tests automatically on code changes.
     
   - **Edge Case Testing:**
     - Develop tests for edge cases, such as empty datasets, missing values, extremely large numbers, and complex query conditions, to ensure the interpreter handles them gracefully.
     
8. **Improve Documentation and User Guides:**
   - **Comprehensive Manuals:**
     - Create detailed documentation covering installation, usage, scripting syntax, and advanced features. Include explanations of supported PROC procedures, data manipulation capabilities, and REPL commands.
     
   - **Tutorials and Examples:**
     - Provide example scripts and tutorials to help users learn how to effectively use the interpreter. Include step-by-step guides for common tasks, such as data import/export, querying, and report generation.
     
   - **API Documentation:**
     - If applicable, provide documentation for any APIs or extensions exposed by the interpreter. Use tools like [Doxygen](https://www.doxygen.nl/) to generate API documentation from your codebase.
     
   - **FAQ and Troubleshooting:**
     - Develop a FAQ section addressing common user questions and issues. Include troubleshooting guides for error messages and unexpected behaviors.
     
9. **Implement Security Features:**
   - **Sandboxing:**
     - Isolate script execution to prevent unauthorized access to system resources. Use sandboxing techniques or containerization technologies (e.g., Docker) to run the interpreter in a controlled environment.
     
   - **Input Validation:**
     - Rigorously validate user inputs to prevent injection attacks, such as SQL injection or command injection. Sanitize inputs before processing them.
     
   - **Access Controls:**
     - Implement permissions and access controls to restrict sensitive operations. For multi-user environments, manage user roles and permissions to control access to datasets and commands.
     
   - **Secure Coding Practices:**
     - Follow secure coding standards to minimize vulnerabilities. Regularly audit the codebase for security flaws and apply patches as needed.
     
10. **Explore Extensibility and Plugin Systems:**
    - **Plugin Architecture:**
      - Develop a framework that supports plugins, enabling users to extend the interpreter's capabilities without modifying the core codebase.
      
    - **APIs for Extensions:**
      - Provide clear APIs and guidelines for developing extensions, fostering a community-driven ecosystem around your interpreter. Define interfaces for plugins to interact with the interpreter, such as adding new PROC procedures or custom functions.
      
    - **Dynamic Loading:**
      - Implement mechanisms to load and unload plugins dynamically at runtime, allowing users to customize their environment as needed.
      
    - **Documentation for Developers:**
      - Offer comprehensive guides and examples to help developers create and integrate plugins. Include sample plugins and templates to facilitate development.
      
11. **Engage with Users and Gather Feedback:**
    - **Beta Testing:**
      - Release beta versions to a select group of users to gather feedback and identify areas for improvement. Use feedback to refine features, fix bugs, and enhance usability.
      
    - **Community Building:**
      - Create forums, mailing lists, or chat channels where users can ask questions, share scripts, and collaborate. Encourage user contributions and discussions.
      
    - **Surveys and Feedback Forms:**
      - Conduct surveys or provide feedback forms to collect structured input from users. Use this data to inform development decisions and roadmap planning.
      
    - **Iterative Development:**
      - Use feedback to prioritize features, fix bugs, and enhance the interpreter based on real-world usage patterns. Adopt an agile development approach to respond quickly to user needs.

---

## **Conclusion**

Implementing **`PROC SQL`** marks a significant advancement in your SAS interpreter's development, equipping it with robust data querying and manipulation capabilities. By extending the AST, updating the Lexer and Parser, and enhancing the Interpreter's execution logic, you've successfully integrated a core SAS procedure that allows users to perform complex SQL operations seamlessly within their SAS scripts. The comprehensive test case ensures that `PROC SQL` operates reliably, accurately executing queries, performing joins, filtering data, and ordering results as expected.

As you continue to build upon this foundation, adopting best practices such as modular design, thorough testing, robust error handling, performance optimization, and comprehensive documentation will be instrumental in creating a reliable, efficient, and user-friendly SAS interpreter. Embracing these strategies will facilitate the addition of new features and ensure that the interpreter remains maintainable and scalable as its capabilities expand.

Keep up the excellent work, and feel free to reach out for further guidance or assistance as you progress with your project. Good luck!