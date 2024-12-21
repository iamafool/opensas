#ifndef DATAENVIRONMENT_H
#define DATAENVIRONMENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

// A simplistic representation of data sets and variables

using Value = std::variant<double, std::string>;

struct Row {
    std::unordered_map<std::string, Value> columns;
};

class DataSet {
public:
    std::vector<Row> rows;
};

class DataEnvironment {
public:
    // store datasets by name
    std::unordered_map<std::string, DataSet> dataSets;
    // current row being processed
    Row currentRow;

    // Helper to get a variable value from current row
    Value getValue(const std::string& varName) {
        auto it = currentRow.columns.find(varName);
        if (it != currentRow.columns.end()) return it->second;
        // If not found, maybe it's a numeric missing. For simplicity:
        return 0.0;
    }

    void setValue(const std::string& varName, const Value& val) {
        currentRow.columns[varName] = val;
    }
};

#endif // DATAENVIRONMENT_H
