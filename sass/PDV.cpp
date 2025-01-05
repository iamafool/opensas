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
    void PDV::initFromSasDoc(SasDoc* doc) {
        for (int i = 0; i < doc->var_count; i++) {
            PdvVar vdef;
            vdef.name = doc->var_names[i];
            vdef.isNumeric = (doc->var_types[i] == READSTAT_TYPE_DOUBLE)
                || (doc->var_types[i] == READSTAT_TYPE_FLOAT)
                || (doc->var_types[i] == READSTAT_TYPE_INT16)
                || (doc->var_types[i] == READSTAT_TYPE_INT32)
                || (doc->var_types[i] == READSTAT_TYPE_INT8);
            vdef.length = doc->var_length[i];
            vdef.label = doc->var_labels[i];
            vdef.format = doc->var_formats[i];
            // If you track informats, do similarly
            vdef.informat = "";
            vdef.decimals = doc->var_decimals[i];
            vdef.retained = false; // default

            // Add it
            addVariable(vdef);
        }
    }

}
