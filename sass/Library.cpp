#include "Library.h"
#include <chrono>
#include <iostream>
#include "sasdoc.h"
#include "Dataset.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace sass {
    Library::Library(const std::string& name, const std::string& path, LibraryAccess access)
        : libName(name), libPath(path), accessMode(access)
    {
        // Optionally set creationTime = now
        creationTime = std::time(nullptr);
    }

    bool Library::hasDataset(const std::string& dsName) const {
        auto it = datasets.find(dsName);
        return (it != datasets.end());
    }

    void Library::addDataset(const std::string& dsName, std::shared_ptr<Dataset> ds) {
        datasets[dsName] = ds;
    }

    // Return pointer if found, else nullptr
    std::shared_ptr<Dataset> Library::getDataset(const std::string& dsName) const {
        auto it = datasets.find(dsName);
        if (it != datasets.end()) {
            return it->second;
        }
        return nullptr;
    }

    void Library::removeDataset(const std::string& dsName) {
        auto it = datasets.find(dsName);
        if (it != datasets.end()) {
            datasets.erase(it);
        }
    }

    std::vector<std::string> Library::listDatasets() const {
        std::vector<std::string> result;
        result.reserve(datasets.size());
        for (auto& kv : datasets) {
            result.push_back(kv.first);
        }
        return result;
    }

    // Example method: load a dataset from .sas7bdat 
    // If successful, store it in datasets[dsName]
    bool Library::loadDatasetFromSas7bdat(const std::string& dsName) {
        if (accessMode == LibraryAccess::READONLY || accessMode == LibraryAccess::READWRITE) {
            fs::path filePath = fs::path(libPath) / fs::path(dsName + ".sas7bdat");
            if (!fs::exists(filePath)) return false;

            auto doc = std::make_shared<SasDoc>();
            if (SasDoc::read_sas7bdat(filePath.wstring(), doc.get()) == 0) {
                // success
                this->datasets[dsName] = doc; // or convert doc-> to a Dataset
                return true;
            } else {
                return false;
            }
        }
        else {
            std::cerr << "[Library] Cannot read dataset in TEMP or restricted mode.\n";
            return false;
        }
    }

    // Example method: save a dataset to .sas7bdat
    bool Library::saveDatasetToSas7bdat(const std::string& dsName) {
        if (accessMode == LibraryAccess::READWRITE) {
            auto it = datasets.find(dsName);
            if (it == datasets.end()) {
                std::cerr << "[Library] Dataset not found: " << dsName << std::endl;
                return false;
            }
            // we have the dataset in memory, write it out
            std::string filePath = (fs::path(libPath) /  fs::path(dsName + ".sas7bdat")).string();
            std::cout << "[Library] Saving " << dsName << " to " << filePath << std::endl;

            // if using SasDoc or readstat writer calls:
            // auto doc = std::dynamic_pointer_cast<SasDoc>(it->second);
            // if (doc) {
            //     int rc = SasDoc::write_sas7bdat(filePath, doc.get());
            //     return (rc == 0);
            // }
            return true;
        }
        else {
            std::cerr << "[Library] Library is read-only or temp, cannot save dataset.\n";
            return false;
        }
    }

    // If the dataset doesn't exist, create it in memory:
    std::shared_ptr<Dataset> Library::getOrCreateDataset(const std::string& dsName) {
        auto it = datasets.find(dsName);
        if (it != datasets.end()) {

            return it->second;
        }
        auto newds = std::make_shared<SasDoc>();
        newds->name = dsName;
        datasets[dsName] = newds;
        return newds;
    }

}

