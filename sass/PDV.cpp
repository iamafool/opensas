#include "PDV.h"
#include <boost/algorithm/string.hpp>

namespace sass {
    // Add a new variable to the PDV
    void PDV::addVariable(const PdvVar& varDef) {
        // Check if it already exists
        int idx = findVarIndex(varDef.name);
        if (idx >= 0) {
            // Already exists => optionally update metadata
            // or do nothing. For simplicity, do nothing.
            return;
        }
        // else push back
        pdvVars.push_back(varDef);
        pdvValues.push_back(varDef.isNumeric ? Value(double(-INFINITY))
            : Value(""));
    }

    // Return varIndex or -1 if not found
    int PDV::findVarIndex(const std::string& name) const {
        for (size_t i = 0; i < pdvVars.size(); i++) {
            if (boost::iequals(pdvVars[i].name, name)) {
                return (int)i;
            }
        }
        return -1;
    }

    void PDV::setValue(int varIndex, const Value& val) {
        if (varIndex < 0 || varIndex >= (int)pdvValues.size()) {
            return; // or throw an error
        }
        pdvValues[varIndex] = val;
    }

    Value PDV::getValue(int varIndex) const {
        if (varIndex < 0 || varIndex >= (int)pdvValues.size()) {
            // return a missing value
            return Value(double(-INFINITY));
        }
        return pdvValues[varIndex];
    }

    void PDV::resetNonRetained() {
        for (size_t i = 0; i < pdvVars.size(); i++) {
            if (!pdvVars[i].retained) {
                // reset to missing
                if (pdvVars[i].isNumeric) {
                    pdvValues[i] = double(-INFINITY);
                }
                else {
                    pdvValues[i] = "";
                }
            }
        }
    }

    void PDV::setRetainFlag(const std::string& varName, bool retainedFlag) {
        int idx = findVarIndex(varName);
        if (idx >= 0) {
            pdvVars[idx].retained = retainedFlag;
        }
    }

    // Suppose we want to build the PDV from the SasDoc¡¯s var_* vectors
    void PDV::initFromDataset(Dataset* doc) {
        for (auto item : doc->columns) {
            PdvVar vdef;
            vdef.name = item.name;
            vdef.isNumeric = (item.type == READSTAT_TYPE_DOUBLE)
                || (item.type == READSTAT_TYPE_FLOAT)
                || (item.type == READSTAT_TYPE_INT16)
                || (item.type == READSTAT_TYPE_INT32)
                || (item.type == READSTAT_TYPE_INT8);
            vdef.length = item.length;
            vdef.label = item.label;
            vdef.format = item.format;
            vdef.informat = "";
            vdef.decimals = item.decimals;
            vdef.retained = false; // default

            // Add it
            addVariable(vdef);
        }
    }

}
