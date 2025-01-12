#ifndef PDV_H
#define PDV_H
#include <unordered_map>
#include <boost/flyweight.hpp>
#include "Dataset.h"
#include "sasdoc.h"

namespace sass {
    // Define a flyweight string type
    using flyweight_string = boost::flyweight<std::string>;

    // Define a variant type that can hold either a flyweight string or a double
    using Cell = std::variant<flyweight_string, double>;

    // Represents one variable's metadata for the PDV
    struct PdvVar {
        std::string name;      // e.g. "AGE", "NAME", etc.
        bool isNumeric;        // true => numeric, false => character
        int length;            // for character vars, or 8 for numeric
        std::string label;     // variable label
        std::string format;    // e.g. "BEST12."
        std::string informat;  // if you support that
        int decimals;          // decimal places for numeric
        bool retained;         // if RETAIN statement used

        PdvVar()
            : isNumeric(false), length(0), decimals(0), retained(false) {}
    };

    // The PDV holds an array of PdvVar plus the current row's values
// for a single iteration.
    class PDV {
    public:
        // Each variable in the PDV corresponds to one PdvVar. 
        // The order in pdvVars must match the order in pdvValues
        std::vector<PdvVar> pdvVars;
        std::vector<Value> pdvValues;

    public:
        // Adds a new variable to the PDV
        // If the variable name already exists, we might skip or update
        void addVariable(const PdvVar& varDef);

        // Find index by name
        int findVarIndex(const std::string& name) const;

        // Get or set a value by var index
        void setValue(int varIndex, const Value& val);
        Value getValue(int varIndex) const;

        // Reset non-retained variables to missing. 
        // Called at the start of each iteration, except for the first
        // (assuming default missing is a double=-INF or an empty string).
        void resetNonRetained();

        // (Optional) Mark a variable as retained
        void setRetainFlag(const std::string& varName, bool retainedFlag);

        // Initialize from dataset
        void initFromDataset(Dataset* doc);
    };


}

#endif // !PDV_H
