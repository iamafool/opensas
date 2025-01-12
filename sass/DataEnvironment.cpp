#include "DataEnvironment.h"
#include "DataEnvironment.h"
#include "Dataset.h"
#include "sasdoc.h"
#include "TempUtils.h"
#include "utility.h"
#include <filesystem>
#include "AST.h"

using namespace std;
namespace fs = std::filesystem;

namespace sass {
    DataEnvironment::DataEnvironment()
        : workCreated(false)
    {
        // create a subfolder in system temp
        this->workFolder = createUniqueTempFolder();
        // define a library named "WORK" with read/write access
        defineLibrary("WORK", workFolder, LibraryAccess::READWRITE);
        workCreated = true;
    }

    DataEnvironment::~DataEnvironment() {
        // If we created a WORK library, remove its folder
        // after the SAS interpreter closes
        if (workCreated) {
            removeDirectoryRecursively(workFolder);
        }
    }


    int DataEnvironment::defineLibrary(const std::string& libref, const std::string& path, LibraryAccess access) {
        // create or update library

        if (fs::exists(path))
        {
            auto lib = std::make_shared<Library>(libref, path, access);
            libraries[libref] = lib;
            return 0;
        }
        return 1;
    }

    std::shared_ptr<Library> DataEnvironment::getLibrary(const std::string& libref) {
        auto it = libraries.find(libref);
        if (it != libraries.end()) {
            return it->second;
        }
        return nullptr;
    }

    void DataEnvironment::removeLibrary(const std::string& libref) {
        libraries.erase(libref);
    }

    void DataEnvironment::saveDataset(DatasetRefNode& ds) {
        std::string lib = ds.libref.empty() ? "WORK" : ds.libref;
        std::string dsName = ds.dataName;

        auto library = getLibrary(lib);
        if (library)
        {
            library->saveDataset(dsName);
        }
    }

    std::shared_ptr<Dataset> DataEnvironment::getOrCreateDataset(DatasetRefNode& ds) {
        std::string lib = ds.libref.empty() ? "WORK" : ds.libref;
        std::string dsName = ds.dataName;

        auto library = getLibrary(lib);
        if (library) {
            library->loadDataset(dsName);
            return library->getOrCreateDataset(dsName);
        }
        return nullptr;
    }

    std::unordered_map<std::string, std::shared_ptr<Library>>   DataEnvironment::getLibraries() {
        return libraries;
    }

}
