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
#include "AST.h"

namespace sass {
    // Manages datasets, global options, librefs, and titles
    class DataEnvironment {
    public:
        DataEnvironment();
        ~DataEnvironment();

        // Current row being processed in a DATA step
        Row currentRow;

        // Current data set
        std::shared_ptr<Dataset> currentDataSet;

        // Global settings
        std::unordered_map<std::string, std::string> options;
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
        std::shared_ptr<Dataset> getOrCreateDataset(DatasetRefNode& ds);

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

        // Set a title
        void setTitle(const std::string& t) {
            title = t;
        }

        void saveDataset(DatasetRefNode& dsNode);

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
        int defineLibrary(const std::string& libref, const std::string& path, LibraryAccess access);

        // Retrieve a library pointer
        std::shared_ptr<Library> getLibrary(const std::string& libref);

        // Possibly a method to remove a library
        void removeLibrary(const std::string& libref);

        // Get libraries
        std::unordered_map<std::string, std::shared_ptr<Library>>   getLibraries();

        // Possibly a method to load a dataset: libref.datasetName
        bool loadDataset(const std::string& libref, const std::string& dsName) {
            auto lib = getLibrary(libref);
            if (!lib) {
                std::cerr << "Library not found: " << libref << std::endl;
                return false;
            }
            return lib->loadDataset(dsName);
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
