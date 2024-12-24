### Next Step: **Implement Data Import and Export Features**

To enhance the usability and functionality of your SAS interpreter, focus on enabling **data import** and **data export** capabilities. These features will allow users to work with external datasets, broadening the interpreter's applicability and integration potential.

---

### **Step: Implementing Data Import and Export**

### **1. Goals**

- **Import Data:**
  - Allow users to read datasets from common formats, such as:
    - CSV
    - Excel (`.xlsx`)
    - JSON
    - SQL databases
- **Export Data:**
  - Provide functionality to write datasets to external formats, supporting:
    - CSV
    - Excel (`.xlsx`)
    - JSON
    - SQL databases

---

### **2. Design Overview**

1. **New Statements:**
   - Introduce `IMPORT` and `EXPORT` statements in the language syntax.
   - Example:
     ```sas
     import datafile="data.csv" out=work.mydata;
     export data=work.mydata outfile="output.json";
     ```

2. **Lexer Updates:**
   - Recognize `IMPORT` and `EXPORT` as keywords, along with associated options (`datafile`, `outfile`, `out`, `data`).

3. **AST Updates:**
   - Create `ImportNode` and `ExportNode` classes to represent these operations.

4. **Parser Updates:**
   - Parse `IMPORT` and `EXPORT` statements into corresponding AST nodes.

5. **Interpreter Updates:**
   - Implement methods to read/write datasets using libraries appropriate for each format.

6. **External Libraries:**
   - Use established libraries to handle file I/O and format-specific operations:
     - **CSV:** [RapidCSV](https://github.com/d99kris/rapidcsv)
     - **Excel:** [libxlsxwriter](https://libxlsxwriter.github.io/)
     - **JSON:** [nlohmann/json](https://github.com/nlohmann/json)
     - **SQL Databases:** [SQLite](https://www.sqlite.org/) or [libpq](https://www.postgresql.org/docs/current/libpq.html) for PostgreSQL

---

### **3. Implementation Details**

#### **3.1. Lexer Updates**

Extend the lexer to tokenize `IMPORT` and `EXPORT` statements:

**Lexer.cpp**
```cpp
if (identifier == "IMPORT") {
    tokens.emplace_back(TokenType::IMPORT, identifier);
} else if (identifier == "EXPORT") {
    tokens.emplace_back(TokenType::EXPORT, identifier);
} else if (identifier == "DATAFILE" || identifier == "OUTFILE" || identifier == "OUT" || identifier == "DATA") {
    tokens.emplace_back(TokenType::OPTION, identifier);
}
```

---

#### **3.2. AST Updates**

Define `ImportNode` and `ExportNode`:

**AST.h**
```cpp
class ImportNode : public ASTNode {
public:
    std::string datafile;   // Path to the input file
    std::string outputName; // Name of the output dataset
    ImportNode(const std::string& datafile, const std::string& outputName)
        : datafile(datafile), outputName(outputName) {}
};

class ExportNode : public ASTNode {
public:
    std::string datasetName; // Name of the dataset to export
    std::string outfile;     // Path to the output file
    ExportNode(const std::string& datasetName, const std::string& outfile)
        : datasetName(datasetName), outfile(outfile) {}
};
```

---

#### **3.3. Parser Updates**

Parse `IMPORT` and `EXPORT` statements into AST nodes:

**Parser.cpp**
```cpp
std::unique_ptr<ASTNode> Parser::parseImport() {
    consume(TokenType::IMPORT, "Expected IMPORT keyword.");
    std::string datafile = consume(TokenType::STRING, "Expected datafile path.").lexeme;
    consume(TokenType::OPTION, "Expected 'OUT'.");
    std::string outputName = consume(TokenType::IDENTIFIER, "Expected output dataset name.").lexeme;
    consume(TokenType::SEMICOLON, "Expected ';' after IMPORT statement.");
    return std::make_unique<ImportNode>(datafile, outputName);
}

std::unique_ptr<ASTNode> Parser::parseExport() {
    consume(TokenType::EXPORT, "Expected EXPORT keyword.");
    consume(TokenType::OPTION, "Expected 'DATA'.");
    std::string datasetName = consume(TokenType::IDENTIFIER, "Expected dataset name.").lexeme;
    consume(TokenType::OPTION, "Expected 'OUTFILE'.");
    std::string outfile = consume(TokenType::STRING, "Expected output file path.").lexeme;
    consume(TokenType::SEMICOLON, "Expected ';' after EXPORT statement.");
    return std::make_unique<ExportNode>(datasetName, outfile);
}
```

---

#### **3.4. Interpreter Updates**

Implement `executeImport` and `executeExport` methods:

**Interpreter.cpp**
```cpp
#include <fstream>
#include <nlohmann/json.hpp>
#include "rapidcsv.h"
#include "libxlsxwriter.hpp"

// Method for executing IMPORT
void Interpreter::executeImport(ImportNode* node) {
    std::ifstream infile(node->datafile);
    if (!infile.is_open()) {
        throw std::runtime_error("Unable to open file: " + node->datafile);
    }

    Dataset dataset;

    if (node->datafile.ends_with(".csv")) {
        rapidcsv::Document doc(node->datafile);
        auto colNames = doc.GetColumnNames();
        for (size_t i = 0; i < doc.GetRowCount(); ++i) {
            Row row;
            for (const auto& col : colNames) {
                row[col] = Value(doc.GetCell<std::string>(col, i));
            }
            dataset.push_back(row);
        }
    } else if (node->datafile.ends_with(".json")) {
        nlohmann::json j;
        infile >> j;
        for (const auto& record : j) {
            Row row;
            for (const auto& [key, value] : record.items()) {
                row[key] = Value(value.get<std::string>());
            }
            dataset.push_back(row);
        }
    } else {
        throw std::runtime_error("Unsupported file format: " + node->datafile);
    }

    env.datasets[node->outputName] = dataset;
    logLogger.info("Imported data into dataset '{}'.", node->outputName);
}

// Method for executing EXPORT
void Interpreter::executeExport(ExportNode* node) {
    auto it = env.datasets.find(node->datasetName);
    if (it == env.datasets.end()) {
        throw std::runtime_error("Dataset not found: " + node->datasetName);
    }

    const Dataset& dataset = it->second;
    std::ofstream outfile(node->outfile);
    if (!outfile.is_open()) {
        throw std::runtime_error("Unable to create file: " + node->outfile);
    }

    if (node->outfile.ends_with(".csv")) {
        for (const auto& [key, _] : dataset[0]) {
            outfile << key << ",";
        }
        outfile << "\n";
        for (const auto& row : dataset) {
            for (const auto& [_, value] : row) {
                outfile << value.as<std::string>() << ",";
            }
            outfile << "\n";
        }
    } else if (node->outfile.ends_with(".json")) {
        nlohmann::json j;
        for (const auto& row : dataset) {
            nlohmann::json record;
            for (const auto& [key, value] : row) {
                record[key] = value.as<std::string>();
            }
            j.push_back(record);
        }
        outfile << j.dump(4);
    } else {
        throw std::runtime_error("Unsupported file format: " + node->outfile);
    }

    logLogger.info("Exported dataset '{}' to file '{}'.", node->datasetName, node->outfile);
}
```

---

#### **3.5. Testing**

Create test cases to validate importing and exporting datasets in supported formats.

- Test importing CSV and JSON files with various structures.
- Test exporting datasets to CSV and JSON and validate the output.

---

### **4. Next Steps**

- **Add Advanced Format Support:** Extend to handle Excel files, SQL databases, or other formats as needed.
- **Optimize File Handling:** Streamline reading and writing operations for large datasets.
- **Error Reporting:** Enhance error messages for common issues, like missing files or incorrect formats.

By implementing data import and export, your SAS interpreter will become significantly more versatile, making it easier for users to integrate with external tools and data workflows.