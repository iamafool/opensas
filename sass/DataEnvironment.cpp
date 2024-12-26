#include "DataEnvironment.h"
#include "Dataset.h"
#include "sasdoc.h"

using namespace std;

namespace sass {
    std::shared_ptr<Dataset> DataEnvironment::loadSas7bdat(const std::string& filepath, const std::string& dsName) {
        auto ds = std::make_shared<Dataset>();
        ds->name = dsName;
        dataSets[dsName] = ds;

        SasDoc* doc = new SasDoc();
        wstring filepath1 = wstring(filepath.begin(), filepath.end());
        int rc = doc->read_sas7bdat(filepath1, doc);

        if (rc == READSTAT_OK) {
            return ds;
        }
    }

    void DataEnvironment::saveSas7bdat(const std::string& dsName, const std::string& filepath) {
        auto it = dataSets.find(dsName);
        if (it != dataSets.end()) {
            setCurrentDataSet(it->second);
            auto ds1 = it->second;
            auto ds = (SasDoc*) (ds1.get());
            wstring path(filepath.begin(), filepath.end());
            ds->write_sas7bdat(path, ds);
        }
    }

}
