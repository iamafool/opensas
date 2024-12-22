#ifndef DATAENVIRONMENT_H
#define DATAENVIRONMENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

// Define a variant type to hold different data types
using Value = std::variant<double, std::string>;

// Represents a single row in a dataset
struct Row {
    std::unordered_map<std::string, Value> columns;
};

// Represents a dataset containing multiple rows
class DataSet {
public:
    std::string name;
    std::vector<Row> rows;

    // Optional: Define column order for consistent output
    std::vector<std::string> columnOrder;

    // Method to add a row to the dataset
    void addRow(const Row& row) {
        // Ensure columns are consistent with columnOrder
        // If columnOrder is empty, initialize it with the first row's columns
        if (columnOrder.empty()) {
            for (const auto& [col, _] : row.columns) {
                columnOrder.push_back(col);
            }
        }
        rows.push_back(row);
    }

    // Method to print the dataset (for debugging)
    void print() const {
        // Print column headers
        for (size_t i = 0; i < columnOrder.size(); ++i) {
            std::cout << columnOrder[i];
            if (i < columnOrder.size() - 1) std::cout << "\t";
        }
        std::cout << "\n";

        // Print rows
        for (const auto& row : rows) {
            for (size_t i = 0; i < columnOrder.size(); ++i) {
                const std::string& col = columnOrder[i];
                auto it = row.columns.find(col);
                if (it != row.columns.end()) {
                    if (std::holds_alternative<double>(it->second)) {
                        std::cout << std::get<double>(it->second);
                    }
                    else {
                        std::cout << std::get<std::string>(it->second);
                    }
                }
                else {
                    std::cout << ".";
                }
                if (i < columnOrder.size() - 1) std::cout << "\t";
            }
            std::cout << "\n";
        }
    }
};

// Manages datasets, global options, librefs, and titles
class DataEnvironment {
public:
    // Store datasets by name
    std::unordered_map<std::string, std::shared_ptr<DataSet>> dataSets;

    // Current row being processed in a DATA step
    Row currentRow;

    // Current data set
    std::shared_ptr<DataSet> currentDataSet;

    // Global settings
    std::unordered_map<std::string, std::string> options;
    std::unordered_map<std::string, std::string> librefs;
    std::string title;

    // Symbol table for variables (could be nested for scopes if needed)
    std::unordered_map<std::string, Value> variables;

    // Helper to get a variable value from current row
    Value getValue(const std::string& varName) const {
        auto it = currentRow.columns.find(varName);
        if (it != currentRow.columns.end()) return it->second;
        // If not found, return missing value (numeric missing is represented as NaN in SAS)
        return std::nan("");
    }

    void setValue(const std::string& varName, const Value& val) {
        currentRow.columns[varName] = val;
    }

    // Retrieve or create a dataset
    std::shared_ptr<DataSet> getOrCreateDataset(const std::string &libref, const std::string &datasetName) {
        std::string fullName = resolveLibref(libref, datasetName);
        auto it = dataSets.find(fullName);
        if (it != dataSets.end()) {
            return it->second;
        }
        else {
            // Create a new dataset
            auto dataset = std::make_shared<DataSet>();
            dataset->name = fullName;
            dataSets[fullName] = dataset;
            return dataset;
        }
    }

    // Resolve libref and dataset name to a full dataset name
    std::string resolveLibref(const std::string &libref, const std::string &datasetName) const {
        if (!libref.empty()) {
            auto it = librefs.find(libref);
            if (it != librefs.end()) {
                return libref + "." + datasetName;
            }
            else {
                throw std::runtime_error("Undefined libref: " + libref);
            }
        }
        else {
            // Default library (e.g., WORK)
            return "WORK." + datasetName;
        }
    }

    // Set a global option
    void setOption(const std::string &option, const std::string &value) {
        options[option] = value;
    }

    // Get a global option with a default value
    std::string getOption(const std::string &option, const std::string &defaultValue = "") const {
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

    // Set a libref
    void setLibref(const std::string &libref, const std::string &path) {
        librefs[libref] = path;
    }

    // Set a title
    void setTitle(const std::string &t) {
        title = t;
    }

    // Load a dataset from a CSV file (for demonstration)
    void loadDatasetFromCSV(const std::string &libref, const std::string &datasetName, const std::string &filepath) {
        auto dataset = getOrCreateDataset(libref, datasetName);
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open CSV file: " + filepath);
        }

        std::string line;
        // Read header
        if (!std::getline(file, line)) {
            throw std::runtime_error("Empty CSV file: " + filepath);
        }

        std::vector<std::string> headers = split(line, ',');
        dataset->columnOrder = headers;

        // Read data rows
        while (std::getline(file, line)) {
            std::vector<std::string> values = split(line, ',');
            Row row;
            for (size_t i = 0; i < headers.size() && i < values.size(); ++i) {
                // Attempt to parse as double, else treat as string
                try {
                    double num = std::stod(values[i]);
                    row.columns[headers[i]] = num;
                }
                catch (...) {
                    row.columns[headers[i]] = values[i];
                }
            }
            dataset->addRow(row);
        }

        file.close();
    }

    // Helper function to split a string by a delimiter
    std::vector<std::string> split(const std::string &s, char delimiter) const {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            // Trim whitespace
            item.erase(0, item.find_first_not_of(" \t\n\r\f\v"));
            item.erase(item.find_last_not_of(" \t\n\r\f\v") + 1);
            tokens.push_back(item);
        }
        return tokens;
    }

    void setCurrentDataSet(std::shared_ptr<DataSet> ds) {
        currentDataSet = ds;
    }

    std::shared_ptr<DataSet> getCurrentDataSet() {
        return currentDataSet;
    }

};

#endif // DATAENVIRONMENT_H
