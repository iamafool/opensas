#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <ctime>
#include "Dataset.h"

namespace sass {
    // Forward-declare any classes you need, e.g. SasDoc or DataEnvironment
    // class SasDoc; // if you want to store references to SasDoc objects in the library

    enum class LibraryAccess {
        READWRITE,
        READONLY,
        TEMPORARY
        // extend as needed
    };

    // Represents a single SAS library (libref). 
    // Typically points to a directory or path.
    class Library {
    public:
        // Constructors
        Library() = default;
        Library(const std::string& name, const std::string& path, LibraryAccess access = LibraryAccess::READWRITE);

        // Basic getters
        const std::string& getName() const { return libName; }
        const std::string& getPath() const { return libPath; }
        LibraryAccess getAccessMode() const { return accessMode; }

        // Possibly store or retrieve metadata like creationTime
        time_t getCreationTime() const { return creationTime; }
        void setCreationTime(time_t t) { creationTime = t; }

        // Access or manage dataset references
        // e.g. "mydata" -> a pointer to a loaded Dataset or SasDoc
        bool hasDataset(const std::string& dsName) const;
        void addDataset(const std::string& dsName, std::shared_ptr<Dataset> ds);
        std::shared_ptr<Dataset> getDataset(const std::string& dsName) const;
        void removeDataset(const std::string& dsName);
        std::vector<std::string> listDatasets() const;

        // Example methods to read and write SAS7BDAT from this library path
        // (integration with a read/write logic - you might use SasDoc or ReadStat behind the scenes)
        bool loadDatasetFromSas7bdat(const std::string& dsName);
        bool saveDatasetToSas7bdat(const std::string& dsName);
        std::shared_ptr<Dataset> getOrCreateDataset(const std::string& dsName);
    private:
        std::string libName;   // e.g. "MYLIB"
        std::string libPath;   // e.g. "/my/directory"
        LibraryAccess accessMode;
        time_t creationTime;

        // A map from dataset name -> dataset pointer
        // You can store a "SasDoc" instead if you prefer
        std::unordered_map<std::string, std::shared_ptr<Dataset>> datasets;
    };

}

#endif // LIBRARY_H
