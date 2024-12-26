#ifndef PDV_H
#define PDV_H
#include <unordered_map>
#include "Dataset.h"

namespace sass {
    class PDV {
    public:
        // variable name => { value, VarDef }
        std::unordered_map<std::string, Value> values;
        std::unordered_map<std::string, VariableDef> varDefs;

        // a method to "reset" non-retained variables each iteration
        void resetNonRetained();

        // a method to mark a variable as retained
        void retain(const std::string& varName);

        // etc.
    };

}

#endif // !PDV_H
