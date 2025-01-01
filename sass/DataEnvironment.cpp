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

    std::shared_ptr<SasDoc> DataEnvironment::loadSas7bdat(const std::string& filepath, const std::string& dsName) {
        SasDoc* doc = new SasDoc();
        wstring filepath1 = wstring(filepath.begin(), filepath.end());
        int rc = doc->read_sas7bdat(filepath1, doc);

        if (rc == READSTAT_OK) {
            return unique_ptr<SasDoc>(doc);
        }
    }

    void DataEnvironment::saveSas7bdat(const std::string& dsName, const std::string& filepath) {
        string libname = getLibname(dsName);
        auto library = getLibrary(libname);
        if (library) {
            auto ds1 = library->getDataset(dsName);
            if (ds1)
            {
                auto ds = (SasDoc*)(ds1.get());
                wstring path(filepath.begin(), filepath.end());
                SasDoc::write_sas7bdat(path, ds);
            }
        }
    }

    void DataEnvironment::saveSas7bdat(const std::string& dsName) {
        // 1) Parse dsName => libref, dataset part
        //    e.g. dsName = "work.a" => libref="work", ds="a"
        std::string lib("WORK");
        std::string ds(dsName);

        // Find the dot
        size_t dotPos = dsName.find('.');
        if (dotPos != std::string::npos) {
            lib = dsName.substr(0, dotPos);
            ds = dsName.substr(dotPos + 1);
        }

        // 2) Look up the library object in 'libraries'
        auto itLib = libraries.find(lib);
        if (itLib == libraries.end()) {
            std::cerr << "[DataEnvironment] Library not found: " << lib << std::endl;
            return; // or throw an exception
        }
        std::shared_ptr<Library> libraryObj = itLib->second;

        // 3) Get the folder (path) from the library
        std::string folderPath = libraryObj->getPath();

        // 4) Construct a final file path, e.g. "C:/temp/a.sas7bdat" or "/tmp/a.sas7bdat"
        std::string filePath = (fs::path(folderPath) / fs::path(ds + ".sas7bdat")).string();

        // 5) Call the existing function
        //    void saveSas7bdat(const std::string& dsName, const std::string& filepath);
        this->saveSas7bdat(dsName, filePath);
    }

    // Suppose your getOrCreateDataset uses the library name and dataset name
    // e.g. "WORK.mydata"
    std::shared_ptr<Dataset> DataEnvironment::getOrCreateDataset(DatasetRefNode& ds) {
        // parse "LIBREF.DATASETNAME"
        std::string lib = ds.libref.empty() ? "WORK" : ds.libref;
        std::string dsName = ds.dataName;

        auto library = getLibrary(lib);
        if (!library) {
            // define a new library if not found?
            // or throw error
            std::cerr << "[DataEnvironment] Library not found: " << lib << std::endl;
            // fallback
            defineLibrary(lib, "/some/path", LibraryAccess::READWRITE);
            library = getLibrary(lib);
        }
        // Now get or create the dataset in that library
        auto rc = loadDataset(lib, dsName);
        return library->getOrCreateDataset(dsName);
    }

    std::unordered_map<std::string, std::shared_ptr<Library>>   DataEnvironment::getLibraries() {
        return libraries;
    }

}
