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

    static bool operator==(const Value& lhs, const Value& rhs) {
        // Compare the variants. The simplest approach is to check index() and then compare:
        if (lhs.index() != rhs.index()) {
            return false;
        }
        // If both are double
        if (lhs.index() == 0) {
            double epsilon = 1e-7;
            return std::fabs(std::get<double>(lhs) - std::get<double>(rhs)) < epsilon;
        }
        // If both are string
        return std::get<std::string>(lhs) == std::get<std::string>(rhs);
    }

    // Represents a single row in a dataset
    struct Row {
        std::unordered_map<std::string, Value> columns;
    };

    static bool operator==(const Row& lhs, const Row& rhs) {
        // If they differ in size of columns, they aren't equal
        if (lhs.columns.size() != rhs.columns.size())
            return false;
        // Check each key->value
        for (auto& kv : lhs.columns) {
            const auto& key = kv.first;
            const auto& valLHS = kv.second;
            // see if key is in rhs
            auto it = rhs.columns.find(key);
            if (it == rhs.columns.end()) {
                return false; // key not found
            }
            const Value& valRHS = it->second;
            if (!(valLHS == valRHS)) {
                return false;
            }
        }
        return true;
    }

    static void PrintTo(const Row& row, std::ostream* os) {
        // Dump row in a friendly manner
        *os << "{ ";
        for (auto& kv : row.columns) {
            *os << kv.first << ": ";
            // print the Value
            if (kv.second.index() == 0) {
                *os << std::get<double>(kv.second);
            }
            else {
                *os << "\"" << std::get<std::string>(kv.second) << "\"";
            }
            *os << ", ";
        }
        *os << "}";
    }

    // Represents a single column in the dataset. It maps column names to their values.
    using Column = std::vector<Value>;

    class VariableDef {
    public:
        std::string name;
        bool isNumeric;        // or an enum Type { Numeric, Char }
        int length;         // e.g. 8 for numeric, or N for char
        int display_length;
        int decimals;
        int type;
        std::string label;
        std::string format;
        std::string informat;
        // Possibly more attributes
    };

    class RowProxy;
    class ColProxy;

    // Represents a dataset containing multiple rows
    class Dataset {
    public:
        virtual ~Dataset() = default;  // This makes Dataset polymorphic
        std::string name;
        std::vector<Row> rows;
        std::vector<VariableDef> columns;

        virtual void addRow(const Row& row) = 0;
        virtual ColProxy getColProxy(int i) = 0;
        virtual int load(std::wstring path) = 0;
        virtual int save(std::wstring path) = 0;

        // Method to get all the columns
        std::vector<std::string> getColumnNames() const {
            std::vector<std::string> columnNames;
            for (auto item : columns) {
                columnNames.push_back(item.name);
            }
            return columnNames;
        }

        int getRowCount() {
            return rows.size();
        };
        int getColumnCount() {
            return columns.size();
        }
    };
}


#endif // !DATASET_H


