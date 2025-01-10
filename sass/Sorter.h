#ifndef SORTER_H
#define SORTER_H

#include "DataEnvironment.h"
#include <vector>
#include <string>
#include <algorithm>

namespace sass {
    class Sorter {
    public:
        // Sorts the dataset by the specified variables
        static void sortDataset(Dataset* dataset, const std::vector<std::string>& byVariables) {
            std::sort(dataset->rows.begin(), dataset->rows.end(),
                [&](const Row& a, const Row& b) -> bool {
                    for (const auto& var : byVariables) {
                        double valA = 0.0;
                        double valB = 0.0;
                        auto itA = a.columns.find(var);
                        if (itA != a.columns.end() && std::holds_alternative<double>(itA->second)) {
                            valA = std::get<double>(itA->second);
                        }
                        auto itB = b.columns.find(var);
                        if (itB != b.columns.end() && std::holds_alternative<double>(itB->second)) {
                            valB = std::get<double>(itB->second);
                        }
                        if (valA < valB) return true;
                        if (valA > valB) return false;
                        // If equal, continue to next BY variable
                    }
                    return false; // All BY variables are equal
                }
            );
        }

        static void sortSasDoc(SasDoc* doc, const std::vector<std::string>& byVariables) {
            if (!doc) return;
            if (doc->obs_count <= 1) {
                // 0 or 1 row => already "sorted"
                return;
            }
            // Build a list of row indices
            std::vector<int> rowIndices(doc->obs_count);
            for (int i = 0; i < doc->obs_count; i++) {
                rowIndices[i] = i;
            }

            // A helper lambda to compare two rows in doc->values
            auto compareRows = [&](int leftIdx, int rightIdx) {
                // We compare each by variable in order.
                for (const auto& var : byVariables) {
                    // 1) find column index
                    auto it = std::find(doc->var_names.begin(), doc->var_names.end(), var);
                    if (it == doc->var_names.end()) {
                        // if we can't find the var, skip it or throw
                        continue; // or throw std::runtime_error("var not found...");
                    }
                    int colIndex = static_cast<int>(it - doc->var_names.begin());

                    // 2) Retrieve the cell for left row
                    Cell leftCell = doc->values[leftIdx * doc->var_count + colIndex];
                    Cell rightCell = doc->values[rightIdx * doc->var_count + colIndex];

                    // 3) Compare
                    // First check if either is double or string
                    bool leftIsDouble = std::holds_alternative<double>(leftCell);
                    bool rightIsDouble = std::holds_alternative<double>(rightCell);

                    if (leftIsDouble && rightIsDouble) {
                        double lval = std::get<double>(leftCell);
                        double rval = std::get<double>(rightCell);
                        if (lval < rval) return true;
                        if (lval > rval) return false;
                        // else they're equal => move to next by variable
                    }
                    else if (!leftIsDouble && !rightIsDouble) {
                        // both are strings
                        const std::string& lstr = std::get<flyweight_string>(leftCell).get();
                        const std::string& rstr = std::get<flyweight_string>(rightCell).get();
                        if (lstr < rstr) return true;
                        if (lstr > rstr) return false;
                        // equal => continue
                    }
                    else {
                        // Mismatched types? Decide how to handle. 
                        // For now, treat numeric < string or something
                        // We'll do numeric < string
                        if (leftIsDouble && !rightIsDouble) {
                            // treat double < string => return true
                            return true;
                        }
                        else {
                            // treat string > double => return false
                            return false;
                        }
                    }
                    // If we get here => they are "equal" under this by var
                    // so we move to next by var
                }
                // If all by variables matched => consider them equal
                return false;
                };

            // Sort the rowIndices using the comparator
            std::sort(rowIndices.begin(), rowIndices.end(), compareRows);

            // Now we permute doc->values to reflect the new row order
            // We'll do the standard "create a temporary copy, then reorder"
            std::vector<Cell> newValues(doc->values.size());
            int varCount = doc->var_count;
            int obsCount = doc->obs_count;
            for (int newRow = 0; newRow < obsCount; newRow++) {
                int oldRow = rowIndices[newRow];
                // copy the entire row from oldRow to newRow
                for (int c = 0; c < varCount; c++) {
                    newValues[newRow * varCount + c] = doc->values[oldRow * varCount + c];
                }
            }

            doc->values = std::move(newValues);
        }

    };
}
#endif // SORTER_H
