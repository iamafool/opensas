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


