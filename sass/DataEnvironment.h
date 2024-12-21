#ifndef DATAENVIRONMENT_H
#define DATAENVIRONMENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <stdexcept>

using Value = std::variant<double, std::string>;

struct Row {
    std::unordered_map<std::string, Value> columns;
};

class DataSet {
public:
    std::string name;
    std::vector<Row> rows;
};

class DataEnvironment {
public:
    // Store datasets by name
    std::unordered_map<std::string, DataSet> dataSets;

    // Current row being processed
    Row currentRow;

    // Global settings
    std::unordered_map<std::string, std::string> options;
    std::unordered_map<std::string, std::string> librefs;
    std::string title;

    // Symbol table for variables (could be nested for scopes if needed)
    std::unordered_map<std::string, Value> variables;

    // Helper to get a variable value from current row
    Value getValue(const std::string &varName) const {
        auto it = currentRow.columns.find(varName);
        if (it != currentRow.columns.end()) return it->second;
        // If not found, return missing value (numeric missing is represented as NaN in SAS)
        return std::nan("");
    }

    void setValue(const std::string &varName, const Value &val) {
        currentRow.columns[varName] = val;
    }

    // Methods to handle global settings
    void setOption(const std::string &option, const std::string &value) {
        options[option] = value;
    }

    void setLibref(const std::string &libref, const std::string &path) {
        librefs[libref] = path;
    }

    void setTitle(const std::string &t) {
        title = t;
    }

    // Method to get a dataset, creating it if necessary
    DataSet& getOrCreateDataset(const std::string& libref, const std::string& datasetName) {
        std::string fullName;
        if (!libref.empty()) {
            auto it = librefs.find(libref);
            if (it != librefs.end()) {
                // For simplicity, ignore the path and use libref + datasetName
                fullName = libref + "." + datasetName;
            }
            else {
                throw std::runtime_error("Undefined libref: " + libref);
            }
        }
        else {
            fullName = datasetName; // Use default or work library
        }
        return dataSets[fullName];
    }

    // Retrieve an option value with a default
    std::string getOption(const std::string& option, const std::string& defaultValue = "") const {
        auto it = options.find(option);
        if (it != options.end()) {
            return it->second;
        }
        return defaultValue;
    }

    // Variable management
    Value getVariable(const std::string& varName) const {
        auto it = variables.find(varName);
        if (it != variables.end()) return it->second;
        // Return missing value (numeric missing is NaN, string missing is empty string)
        return std::nan("");
    }

    void setVariable(const std::string& varName, const Value& val) {
        variables[varName] = val;
    }

};

#endif // DATAENVIRONMENT_H
