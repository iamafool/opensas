#ifndef DATASET_H
#define DATASET_H

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>

namespace sass {
    // Define a variant type to hold different data types
    using Value = std::variant<double, std::string>;

    // Represents a single row in a dataset
    struct Row {
        std::unordered_map<std::string, Value> columns;
    };

    // Represents a single column in the dataset. It maps column names to their values.
    using Column = std::vector<Value>;

    class VariableDef {
    public:
        std::string name;
        bool isNumeric;        // or an enum Type { Numeric, Char }
        size_t length;         // e.g. 8 for numeric, or N for char
        std::string label;
        std::string format;
        std::string informat;
        // Possibly more attributes
    };

    // Represents a dataset containing multiple rows
    class Dataset {
    public:
        std::string name;
        std::vector<Row> rows;

        // Column metadata
        std::vector<VariableDef> varDefs;

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
    };


}


#endif // !DATASET_H


