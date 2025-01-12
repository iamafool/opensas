#include "Library.h"
#include "Library.h"
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
        fileType = LibraryFileType::SAS7BDAT;
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

    bool Library::loadDataset(const std::string& dsName)
    {
        switch (fileType)
        {
        case sass::LibraryFileType::SAS7BDAT:
            return loadDatasetFromSas7bdat(dsName);
        case sass::LibraryFileType::XPT:
            break;
        case sass::LibraryFileType::RDS:
            break;
        case sass::LibraryFileType::CSV:
            break;
        case sass::LibraryFileType::XLSX:
            break;
        default:
            break;
        }
        return false;
    }

    // Example method: load a dataset from .sas7bdat 
    // If successful, store it in datasets[dsName]
    bool Library::loadDatasetFromSas7bdat(const std::string& dsName) {
        if (accessMode == LibraryAccess::READONLY || accessMode == LibraryAccess::READWRITE) {
            fs::path filePath = fs::path(libPath) / fs::path(dsName + ".sas7bdat");
            if (!fs::exists(filePath)) return false;

            auto doc = std::make_shared<SasDoc>();
            if (doc.get()->load(filePath.wstring()) == 0) {
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

    bool Library::saveDataset(const std::string& dsName)
    {
        std::string filePath = (fs::path(getPath()) / fs::path(dsName + ".sas7bdat")).string();

        switch (fileType)
        {
        case sass::LibraryFileType::SAS7BDAT:
            return saveDatasetToSas7bdat(dsName, filePath);
        case sass::LibraryFileType::XPT:
            break;
        case sass::LibraryFileType::RDS:
            break;
        case sass::LibraryFileType::CSV:
            break;
        case sass::LibraryFileType::XLSX:
            break;
        default:
            break;
        }
        return false;
    }

    bool Library::saveDatasetToSas7bdat(const std::string& dsName, std::string filepath) {
        auto ds1 = getDataset(dsName);
        if (ds1)
        {
            auto ds = (SasDoc*)(ds1.get());
            std::wstring path(filepath.begin(), filepath.end());
            return SasDoc::write_sas7bdat(path, ds);
        }
        return false;
    }

    // If the dataset doesn't exist, create it in memory:
    std::shared_ptr<Dataset> Library::getOrCreateDataset(const std::string& dsName) {
        auto it = datasets.find(dsName);
        if (it != datasets.end()) {

            return it->second;
        }

        std::shared_ptr<Dataset> newds = nullptr;
        switch (fileType)
        {
        case sass::LibraryFileType::SAS7BDAT:
            newds = std::make_shared<SasDoc>();
            break;
        case sass::LibraryFileType::XPT:
            break;
        case sass::LibraryFileType::RDS:
            break;
        case sass::LibraryFileType::CSV:
            break;
        case sass::LibraryFileType::XLSX:
            break;
        default:
            break;
        }

        newds->name = dsName;
        datasets[dsName] = newds;
        return newds;
    }

}

