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
#include <map>
#include "Dataset.h"
#include "Library.h"
#include "sasdoc.h"

namespace sass {
    // Manages datasets, global options, librefs, and titles
    class DataEnvironment {
    public:
        DataEnvironment();
        ~DataEnvironment();

        // Store datasets by name
        std::unordered_map<std::string, std::shared_ptr<Dataset>> dataSets;

        // Current row being processed in a DATA step
        Row currentRow;

        // Current data set
        std::shared_ptr<Dataset> currentDataSet;

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
        std::shared_ptr<Dataset> getOrCreateDataset(const std::string& datasetName);

        // Resolve libref and dataset name to a full dataset name
        std::string resolveLibref(const std::string& libref, const std::string& datasetName) const {
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
        void setOption(const std::string& option, const std::string& value) {
            options[option] = value;
        }

        // Get a global option with a default value
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

        // Set a libref
        void setLibref(const std::string& libref, const std::string& path) {
            librefs[libref] = path;
        }

        // Set a title
        void setTitle(const std::string& t) {
            title = t;
        }

        // Load a dataset from a CSV file (for demonstration)
        void loadDatasetFromCSV(const std::string& libref, const std::string& datasetName, const std::string& filepath) {
            auto dataset = getOrCreateDataset(datasetName);
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

        std::shared_ptr<Dataset> loadSas7bdat(const std::string& filepath, const std::string& dsName); 

        void saveSas7bdat(const std::string& dsName, const std::string& filepath);

        void saveSas7bdat(const std::string& dsName);

        // Helper function to split a string by a delimiter
        std::vector<std::string> split(const std::string& s, char delimiter) const {
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

        void setCurrentDataSet(std::shared_ptr<Dataset> ds) {
            currentDataSet = ds;
        }

        std::shared_ptr<Dataset> getCurrentDataSet() {
            return currentDataSet;
        }

        // The existing method for LIBNAME statement:
        void defineLibrary(const std::string& libref, const std::string& path, LibraryAccess access);

        // Retrieve a library pointer
        std::shared_ptr<Library> getLibrary(const std::string& libref);

        // Possibly a method to remove a library
        void removeLibrary(const std::string& libref);

        // Possibly a method to load a dataset: libref.datasetName
        bool loadDataset(const std::string& libref, const std::string& dsName) {
            auto lib = getLibrary(libref);
            if (!lib) {
                std::cerr << "Library not found: " << libref << std::endl;
                return false;
            }
            return lib->loadDatasetFromSas7bdat(dsName);
        }

    private:
        // A map from libref => Library instance
        std::unordered_map<std::string, std::shared_ptr<Library>> libraries;

        // for WORK
        std::string workFolder;
        bool workCreated;

    };

}
#endif // DATAENVIRONMENT_H
