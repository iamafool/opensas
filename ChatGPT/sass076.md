### Next Step: **Implement `PROC TRANSPOSE`**

Adding **`PROC TRANSPOSE`** to your interpreter allows users to reshape their datasets by converting rows to columns and vice versa. This is an essential feature in many data processing workflows.

---

### **Step: Implementing `PROC TRANSPOSE`**

### **1. Goals**

- Enable users to transpose datasets for reshaping and pivoting data.
- Support options for specifying:
  - Variables to transpose (`VAR`).
  - Grouping variables (`BY`).
  - Column identifiers (`ID`).
  - Output dataset name (`OUT`).

---

### **2. Design Overview**

1. **Syntax Example:**

   ```sas
   proc transpose data=work.sales out=work.sales_transposed;
       by region;
       id month;
       var revenue;
   run;
   ```

2. **Lexer Updates:**
   - Recognize `TRANSPOSE`, `BY`, `ID`, `VAR`, `OUT`, and `DATA` keywords.

3. **AST Updates:**
   - Create a `ProcTransposeNode` class to represent the procedure and its options.

4. **Parser Updates:**
   - Parse `PROC TRANSPOSE` statements into `ProcTransposeNode`.

5. **Interpreter Updates:**
   - Implement the transpose logic, including grouping by `BY` variables and using `ID` variables for column headers.

---

### **3. Implementation Details**

#### **3.1. AST Updates**

Add a node to represent `PROC TRANSPOSE`:

**AST.h**
```cpp
class ProcTransposeNode : public ProcStepNode {
public:
    std::string inputDataset;
    std::string outputDataset;
    std::vector<std::string> byVariables;
    std::string idVariable;
    std::vector<std::string> varVariables;

    ProcTransposeNode(const std::string& data, const std::string& out)
        : inputDataset(data), outputDataset(out) {}
};
```

---

#### **3.2. Lexer Updates**

Update the lexer to recognize new keywords:

**Lexer.cpp**
```cpp
if (identifier == "TRANSPOSE") {
    tokens.emplace_back(TokenType::PROC_TRANSPOSE, identifier);
} else if (identifier == "BY") {
    tokens.emplace_back(TokenType::BY, identifier);
} else if (identifier == "ID") {
    tokens.emplace_back(TokenType::ID, identifier);
} else if (identifier == "VAR") {
    tokens.emplace_back(TokenType::VAR, identifier);
} else if (identifier == "OUT") {
    tokens.emplace_back(TokenType::OUT, identifier);
} else if (identifier == "DATA") {
    tokens.emplace_back(TokenType::DATA, identifier);
}
```

---

#### **3.3. Parser Updates**

Parse `PROC TRANSPOSE` into `ProcTransposeNode`:

**Parser.cpp**
```cpp
std::unique_ptr<ASTNode> Parser::parseProcTranspose() {
    consume(TokenType::PROC_TRANSPOSE, "Expected TRANSPOSE keyword.");
    consume(TokenType::DATA, "Expected DATA keyword.");
    std::string inputDataset = consume(TokenType::IDENTIFIER, "Expected input dataset name.").lexeme;
    consume(TokenType::OUT, "Expected OUT keyword.");
    std::string outputDataset = consume(TokenType::IDENTIFIER, "Expected output dataset name.").lexeme;

    auto node = std::make_unique<ProcTransposeNode>(inputDataset, outputDataset);

    while (!check(TokenType::RUN) && !isAtEnd()) {
        if (match(TokenType::BY)) {
            node->byVariables = parseVariableList();
        } else if (match(TokenType::ID)) {
            node->idVariable = consume(TokenType::IDENTIFIER, "Expected ID variable name.").lexeme;
        } else if (match(TokenType::VAR)) {
            node->varVariables = parseVariableList();
        } else {
            throw std::runtime_error("Unexpected token in PROC TRANSPOSE.");
        }
    }

    consume(TokenType::RUN, "Expected RUN keyword.");
    consume(TokenType::SEMICOLON, "Expected ';' after RUN.");
    return node;
}

std::vector<std::string> Parser::parseVariableList() {
    std::vector<std::string> variables;
    do {
        variables.push_back(consume(TokenType::IDENTIFIER, "Expected variable name.").lexeme);
    } while (match(TokenType::COMMA));
    consume(TokenType::SEMICOLON, "Expected ';' after variable list.");
    return variables;
}
```

---

#### **3.4. Interpreter Updates**

Implement the transpose logic:

**Interpreter.cpp**
```cpp
void Interpreter::executeProcTranspose(ProcTransposeNode* node) {
    auto it = env.datasets.find(node->inputDataset);
    if (it == env.datasets.end()) {
        throw std::runtime_error("Dataset not found: " + node->inputDataset);
    }

    Dataset& inputDataset = it->second;
    Dataset outputDataset;

    // Group by BY variables
    std::map<std::vector<Value>, std::vector<Row>> groupedRows;
    for (const auto& row : inputDataset) {
        std::vector<Value> groupKey;
        for (const auto& byVar : node->byVariables) {
            groupKey.push_back(row.at(byVar).value);
        }
        groupedRows[groupKey].push_back(row);
    }

    // Transpose each group
    for (const auto& [groupKey, rows] : groupedRows) {
        for (const auto& var : node->varVariables) {
            Row newRow;
            // Add BY variables
            for (size_t i = 0; i < groupKey.size(); ++i) {
                newRow[node->byVariables[i]] = Value(groupKey[i]);
            }

            // Add transposed values
            for (const auto& row : rows) {
                std::string idValue = row.at(node->idVariable).value.as<std::string>();
                newRow[idValue] = row.at(var).value;
            }

            outputDataset.push_back(newRow);
        }
    }

    // Save the output dataset
    env.datasets[node->outputDataset] = outputDataset;
    logLogger.info("PROC TRANSPOSE executed. Output dataset: '{}'", node->outputDataset);
}
```

---

#### **3.5. Testing**

**SAS Script (`test_proc_transpose.sas`):**
```sas
data sales;
    input region $ month $ revenue;
    datalines;
North Jan 100
North Feb 150
North Mar 200
South Jan 120
South Feb 180
South Mar 240
;
run;

proc transpose data=sales out=sales_transposed;
    by region;
    id month;
    var revenue;
run;
```

**Expected Output (`sales_transposed`):**
| Region | Jan  | Feb  | Mar  |
|--------|------|------|------|
| North  | 100  | 150  | 200  |
| South  | 120  | 180  | 240  |

---

### **4. Next Steps**

1. **Advanced Features:**
   - Add support for missing values and handling non-numeric transposed variables.
   - Support additional transpose options, like prefixing transposed columns.

2. **Testing Suite:**
   - Add test cases for edge scenarios, such as:
     - Missing `ID` values.
     - Datasets with no `BY` variables.
     - Multiple `VAR` variables.

3. **Documentation:**
   - Update the user guide with syntax and examples for `PROC TRANSPOSE`.

4. **Performance Optimization:**
   - Optimize grouping and transposing logic for large datasets.

This implementation adds significant data reshaping capability to your interpreter, making it a more powerful tool for data analysis workflows.