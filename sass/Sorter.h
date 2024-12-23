#ifndef SORTER_H
#define SORTER_H

#include "DataEnvironment.h"
#include <vector>
#include <string>
#include <algorithm>

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
};

#endif // SORTER_H
