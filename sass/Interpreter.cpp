#include "Interpreter.h"
#include "Interpreter.h"
#include "Interpreter.h"
#include "Sorter.h"
#include <iostream>
#include <stdexcept>
#include <cmath> // for std::nan
#include <algorithm>
#include <iomanip>
#include <unordered_set>
#include <numeric>
#include <set>
#include "Lexer.h"
#include "Parser.h"
#include "PDV.h"
#include "StepTimer.h"

using namespace std;

namespace sass {
// Execute the entire program
void Interpreter::executeProgram(const std::unique_ptr<ProgramNode> &program) {
    for (const auto &stmt : program->statements) {
        try {
            execute(stmt.get());
        }
        catch (const std::runtime_error &e) {
            logLogger.error("Execution error: {}", e.what());
            // Continue with the next statement
        }
    }
}

// Execute a single AST node
void Interpreter::execute(ASTNode *node) {
    if (auto callNode = dynamic_cast<MacroCallNode*>(node)) {
        executeMacroCall(callNode);
    }
    else if (auto macroNode = dynamic_cast<MacroDefinitionNode*>(node)) {
        std::unique_ptr<MacroDefinitionNode> uniqueMacroNode{ macroNode };
        executeMacroDefinition(std::move(uniqueMacroNode));
    }
    else if (auto letNode = dynamic_cast<MacroVariableAssignmentNode*>(node)) {
        executeMacroVariableAssignment(letNode);
    }
    else if (auto ds = dynamic_cast<DataStepNode*>(node)) {
        executeDataStep(ds);
    }
    else if (auto opt = dynamic_cast<OptionsNode*>(node)) {
        executeOptions(opt);
    }
    else if (auto lib = dynamic_cast<LibnameNode*>(node)) {
        executeLibname(lib);
    }
    else if (auto title = dynamic_cast<TitleNode*>(node)) {
        executeTitle(title);
    }
    else if (auto proc = dynamic_cast<ProcNode*>(node)) {
        executeProc(proc);
    }
    else if (auto ifElseIf = dynamic_cast<IfElseIfNode*>(node)) {
        executeIfElse(ifElseIf);
    }
    else if (auto arrayNode = dynamic_cast<ArrayNode*>(node)) {
        executeArray(arrayNode);
    }
    else if (auto mergeNode = dynamic_cast<MergeStatementNode*>(node)) {
        executeMerge(mergeNode);
    }
    else if (auto byNode = dynamic_cast<ByStatementNode*>(node)) {
        executeBy(byNode);
    }
    else if (auto doLoop = dynamic_cast<DoLoopNode*>(node)) {
        executeDoLoop(doLoop);
    }
    else if (auto endNode = dynamic_cast<EndNode*>(node)) {
        executeEnd(endNode);
    }
    else {
        // Handle other statements
        // For now, ignore unknown statements or throw an error
        throw std::runtime_error("Unknown AST node encountered during execution.");
    }
}

void Interpreter::executeDataStepStatement(
    ASTNode* stmt,
    PDV& pdv,
    SasDoc* outDoc,
    bool& doOutputThisRow)
{
    // This function modifies pdv or sets doOutputThisRow if an OutputNode
    // We'll do small examples:

    if (auto assign = dynamic_cast<AssignmentNode*>(stmt)) {
        // Evaluate
        Value val = evaluate(assign->expression.get());
        int pdvIndex = pdv.findVarIndex(assign->varName);
        if (pdvIndex < 0) {
            // If not found, add new variable to PDV
            PdvVar newVar;
            newVar.name = assign->varName;
            newVar.isNumeric = std::holds_alternative<double>(val);
            pdv.addVariable(newVar);
            pdvIndex = pdv.findVarIndex(assign->varName);
        }
        // Convert Value => Cell
        if (std::holds_alternative<double>(val)) {
            pdv.setValue(pdvIndex, std::get<double>(val));
        }
        else {
            pdv.setValue(pdvIndex, flyweight_string(std::get<std::string>(val)));
        }
    }
    else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt)) {
        // Evaluate condition from PDV
        Value condVal = evaluate(ifThen->condition.get());
        double d = toNumber(condVal);
        if (d != 0.0) {
            // run thenStatements
            for (auto& subStmt : ifThen->thenStatements) {
                executeDataStepStatement(subStmt.get(), pdv, outDoc, doOutputThisRow);
            }
        }
    }
    else if (auto outStmt = dynamic_cast<OutputNode*>(stmt)) {
        // Mark that we want to output this row
        doOutputThisRow = true;
    }
    else if (auto dropStmt = dynamic_cast<DropNode*>(stmt)) {
        for (auto& varName : dropStmt->variables) {
            int idx = pdv.findVarIndex(varName);
            if (idx >= 0) {
                pdv.pdvVars.erase(pdv.pdvVars.begin() + idx);
                pdv.pdvValues.erase(pdv.pdvValues.begin() + idx);
            }
        }
    }
    else if (auto keepStmt = dynamic_cast<KeepNode*>(stmt)) {
        std::vector<PdvVar> newVars;
        std::vector<Cell> newVals;
        for (size_t i = 0; i < pdv.pdvVars.size(); i++) {
            auto& varDef = pdv.pdvVars[i];
            // If varDef.name is in keepStmt->variables, keep it
            if (std::find(keepStmt->variables.begin(), keepStmt->variables.end(),
                varDef.name) != keepStmt->variables.end())
            {
                newVars.push_back(varDef);
                newVals.push_back(pdv.pdvValues[i]);
            }
        }
        pdv.pdvVars = newVars;
        pdv.pdvValues = newVals;
    }
    else if (auto retainStmt = dynamic_cast<RetainNode*>(stmt)) {
        for (auto& var : retainStmt->variables) {
            pdv.setRetainFlag(var, true);
        }
    }
    // else handle other statements: array, do loops, merges, etc.
    else {
        // fallback
    }
}


void Interpreter::appendPdvRowToSasDoc(PDV& pdv, SasDoc* doc)
{
    // Step 1: sync new PDV columns to doc
    syncPdvColumnsToSasDoc(pdv, doc);

    // We'll increment doc->obs_count
    int outRowIndex = doc->obs_count;
    doc->obs_count++;
    // Make sure doc->values is big enough
    doc->values.resize(doc->var_count * doc->obs_count);

    // For each variable in doc->var_names / doc->var_count
    for (int c = 0; c < doc->var_count; c++) {
        const std::string& varName = doc->var_names[c];
        int pdvIndex = pdv.findVarIndex(varName);
        if (pdvIndex >= 0) {
            doc->values[outRowIndex * doc->var_count + c] = pdv.getValue(pdvIndex);
        }
        else {
            // missing
            if (doc->var_types[c] == READSTAT_TYPE_STRING) {
                doc->values[outRowIndex * doc->var_count + c] = flyweight_string("");
            }
            else {
                doc->values[outRowIndex * doc->var_count + c] = double(-INFINITY);
            }
        }
    }
}


// Execute a DATA step
void Interpreter::executeDataStep(DataStepNode* node) {
    ScopedStepTimer timer("DATA statement", logLogger);

    // 1) Create or get the output dataset (SasDoc or normal Dataset)
    auto outDatasetPtr = env.getOrCreateDataset(node->outputDataSet);
    // For full readstat integration, cast to SasDoc if you want:
    auto outDoc = std::dynamic_pointer_cast<SasDoc>(outDatasetPtr);
    if (!outDoc) {
        // If it's not SasDoc, we can fallback or just use Dataset
        outDoc = std::make_shared<SasDoc>();
        outDoc->name = node->outputDataSet.dataName;
        // env.dataSets[node->outputDataSet] = outDoc;
    }

    // 2) Build a PDV
    PDV pdv;

    // We want to see if there's an input dataset
    bool hasInputDataset = !node->inputDataSet.dataName.empty();

    // We also want to gather any InputNode or DatalinesNode statements
    std::vector<std::pair<std::string, bool>> inputVars; // (varName, isString)
    std::vector<std::string> datalines;

    // Also gather other data step statements (assignments, if-then, etc.)
    // We can store them in a vector, or handle them inline. 
    // For clarity, let's store them:
    std::vector<ASTNode*> dataStepStmts;

    // 3) Pre-scan node->statements to find InputNode, DatalinesNode, etc.
    for (auto& stmtUniquePtr : node->statements) {
        ASTNode* stmt = stmtUniquePtr.get();
        if (auto inp = dynamic_cast<InputNode*>(stmt)) {
            // We'll collect these variables
            for (auto& varPair : inp->variables) {
                inputVars.push_back(varPair);
            }
        }
        else if (auto dl = dynamic_cast<DatalinesNode*>(stmt)) {
            // Collect lines
            for (auto& line : dl->lines) {
                datalines.push_back(line);
            }
        }
        else {
            // It's not input or datalines, so store it in dataStepStmts
            dataStepStmts.push_back(stmt);
        }
    }

    // We keep track if user calls OUTPUT in statements. We'll handle that row-by-row in the PDV approach
    bool doOutputThisRow = false;

    bool hasOutputStatement = false;
    for (auto stmt : dataStepStmts) {
        if (dynamic_cast<OutputNode*>(stmt))
        {
            hasOutputStatement = true;
            break;
        }
    }

    //-------------------------------------------------------------------
    // 4) If user specified an input dataset: "data out; set in; ..."
    //-------------------------------------------------------------------
    if (hasInputDataset) {
        // Let's get that dataset (SasDoc)
        auto inDocPtr = env.getOrCreateDataset(node->inputDataSet);
        auto inDoc = std::dynamic_pointer_cast<SasDoc>(inDocPtr);
        if (!inDoc) {
            throw std::runtime_error(
                "Input dataset '" + node->inputDataSet.getFullDsName() + "' not found or not a SasDoc."
            );
        }

        // Initialize PDV from inDoc
        pdv.initFromSasDoc(inDoc.get());

        // We'll iterate over each row in inDoc
        int rowCount = inDoc->obs_count;
        for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
            // (a) load row from inDoc->values => PDV
            for (int col = 0; col < inDoc->var_count; ++col) {
                Cell cellVal = inDoc->values[rowIndex * inDoc->var_count + col];
                const std::string& varName = inDoc->var_names[col];
                int pdvIndex = pdv.findVarIndex(varName);
                if (pdvIndex >= 0) {
                    pdv.setValue(pdvIndex, cellVal);
                }
            }

            // (b) execute the other statements for this row
            //     e.g., assignments, if-then, drop, keep, etc.
            doOutputThisRow = false; // reset for this iteration

            for (auto stmt : dataStepStmts) {
                // We'll do a helper method: executeDataStepStatement(stmt, pdv, outDoc, doOutputThisRow)
                // That method will handle assignment, if-then, output, etc.
                executeDataStepStatement(stmt, pdv, outDoc.get(), doOutputThisRow);
            }

            // (c) If doOutputThisRow==true, copy from PDV => outDoc
            if (doOutputThisRow) {
                appendPdvRowToSasDoc(pdv, outDoc.get());
            }

            // (d) resetNonRetained for next iteration
            pdv.resetNonRetained();
        }
    }
    else {
        //-------------------------------------------------------------------
        // 5) If there's NO input dataset (like "data out; input name $ age; datalines; ...; run;")
        //    we handle the lines ourselves, row by row in PDV.
        //-------------------------------------------------------------------
        // First, let's define PDV variables from inputVars:
        for (auto& varPair : inputVars) {
            const std::string& varName = varPair.first;
            bool isStringVar = varPair.second;
            PdvVar vdef;
            vdef.name = varName; // might be "NAME" or "NAME$" depending on parser
            vdef.isNumeric = !isStringVar;
            // if you need to strip trailing '$', do so
            if (isStringVar && !vdef.name.empty() && vdef.name.back() == '$') {
                vdef.name.pop_back();
            }
            pdv.addVariable(vdef);
        }

        // Now for each line in datalines => fill PDV => run statements => possibly output
        for (auto& oneLine : datalines) {
            // parse fields
            std::vector<std::string> fields;
            {
                std::istringstream iss(oneLine);
                std::string f;
                while (iss >> f) {
                    fields.push_back(f);
                }
            }

            // load PDV variables from fields
            // we assume inputVars.size() is the max # of fields
            for (size_t i = 0; i < inputVars.size(); ++i) {
                if (i < fields.size()) {
                    const std::string& field = fields[i];
                    // if numeric, convert
                    int pdvIndex = pdv.findVarIndex(inputVars[i].first);
                    if (pdvIndex < 0) continue; // shouldn't happen unless code mismatch

                    if (inputVars[i].second) {
                        // it's a string
                        pdv.setValue(pdvIndex, flyweight_string(field));
                    }
                    else {
                        // it's numeric
                        try {
                            double dval = std::stod(field);
                            pdv.setValue(pdvIndex, dval);
                        }
                        catch (...) {
                            pdv.setValue(pdvIndex, double(-INFINITY));
                        }
                    }
                }
                else {
                    // If there's not enough fields, we might set missing
                    int pdvIndex = pdv.findVarIndex(inputVars[i].first);
                    if (pdvIndex >= 0) {
                        if (inputVars[i].second) {
                            pdv.setValue(pdvIndex, flyweight_string(""));
                        }
                        else {
                            pdv.setValue(pdvIndex, double(-INFINITY));
                        }
                    }
                }
            }

            // run the other data step statements for this line
            doOutputThisRow = false;
            for (auto stmt : dataStepStmts) {
                executeDataStepStatement(stmt, pdv, outDoc.get(), doOutputThisRow);
            }

            if (doOutputThisRow || !hasOutputStatement) {
                appendPdvRowToSasDoc(pdv, outDoc.get());
            }

            pdv.resetNonRetained();
        }

        // (b) execute the other statements for this row
//     e.g., assignments, if-then, drop, keep, etc.
        doOutputThisRow = false; // reset for this iteration

        for (auto stmt : dataStepStmts) {
            // We'll do a helper method: executeDataStepStatement(stmt, pdv, outDoc, doOutputThisRow)
            // That method will handle assignment, if-then, output, etc.
            executeDataStepStatement(stmt, pdv, outDoc.get(), doOutputThisRow);
        }

        // (c) If doOutputThisRow==true, copy from PDV => outDoc
        if (doOutputThisRow) {
            appendPdvRowToSasDoc(pdv, outDoc.get());
        }

        // (d) resetNonRetained for next iteration
        pdv.resetNonRetained();
    }

    // save
    env.saveSas7bdat(outDoc->name);

    // 6) Final logging
    // outDoc->obs_count should be updated as we appended rows
    int obsCount = outDoc->obs_count;
    // var_count might also be known; if not we can glean from outDoc->var_names
    int varCount = outDoc->var_count;
    logLogger.info("NOTE: The data set {} has {} observations and {} variables.",
        outDoc->name, obsCount, varCount);
}


void Interpreter::syncPdvColumnsToSasDoc(PDV& pdv, SasDoc* doc)
{
    // For each PdvVar in pdv, see if doc->var_names already has it
    for (size_t i = 0; i < pdv.pdvVars.size(); i++) {
        const std::string& pdvVarName = pdv.pdvVars[i].name;
        // search doc->var_names
        auto it = std::find(doc->var_names.begin(), doc->var_names.end(), pdvVarName);
        if (it == doc->var_names.end()) {
            // We have a new column
            doc->var_names.push_back(pdvVarName);
            // guess var_types: if isNumeric => READSTAT_TYPE_DOUBLE else READSTAT_TYPE_STRING
            if (pdv.pdvVars[i].isNumeric) {
                doc->var_types.push_back(READSTAT_TYPE_DOUBLE);
            }
            else {
                doc->var_types.push_back(READSTAT_TYPE_STRING);
            }

            // Optionally define var_labels, var_formats, var_length etc. For now we do minimal:
            doc->var_labels.push_back("");
            doc->var_formats.push_back("");
            doc->var_length.push_back(pdv.pdvVars[i].length <= 0 ? 8 : pdv.pdvVars[i].length);
            doc->var_display_length.push_back(8); // arbitrary
            doc->var_decimals.push_back(0);

            // Increase var_count
            doc->var_count = (int)doc->var_names.size();

            // Now we must expand doc->values to accommodate this new column.
            // For each existing row in doc->obs_count, we insert a missing placeholder
            // typically -INFINITY for numeric or "" for string
            int oldRowCount = doc->obs_count;
            if (oldRowCount > 0) {
                // The old doc->values size was var_count-1 * oldRowCount, we just incremented var_count
                // So we re-allocate doc->values to the new size
                // we must do it carefully: we had old size = (var_count-1) * oldRowCount
                // new size = var_count * oldRowCount
                // We'll copy from old array to new array
                // but a simpler approach is to do it row by row
                int newVarCount = doc->var_count; // after increment
                std::vector<Cell> oldValues = doc->values;
                doc->values.clear();
                doc->values.resize(newVarCount * oldRowCount);

                // row by row copy
                for (int r = 0; r < oldRowCount; r++) {
                    // copy old row
                    for (int c = 0; c < newVarCount - 1; c++) {
                        // the old column c in that row used to be old row index = r*(var_count-1) + c
                        doc->values[r * newVarCount + c] = oldValues[r * (newVarCount - 1) + c];
                    }
                    // fill the new column with missing
                    if (pdv.pdvVars[i].isNumeric) {
                        doc->values[r * newVarCount + (newVarCount - 1)] = double(-INFINITY);
                    }
                    else {
                        doc->values[r * newVarCount + (newVarCount - 1)] = flyweight_string("");
                    }
                }
            }
            else {
                // If there are no existing rows, nothing to do except ensure doc->values is correct size
                // doc->obs_count=0 => doc->values is 0 sized anyway
            }
        }
    }
}


// Execute an assignment statement
void Interpreter::executeAssignment(AssignmentNode *node) {
    Value val = evaluate(node->expression.get());
    env.setVariable(node->varName, val);
    logLogger.info("Assigned {} = {}", node->varName, toString(val));
}

// Execute an IF-THEN statement
void Interpreter::executeIfThen(IfThenNode *node) {
    Value cond = evaluate(node->condition.get());
    double d = toNumber(cond);
    logLogger.info("Evaluating IF condition: {}", d);

    if (d != 0.0) { // Non-zero is true
        for (const auto &stmt : node->thenStatements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
            }
            else {
                throw std::runtime_error("Unsupported statement in IF-THEN block.");
            }
        }
    }
}

// Execute an OUTPUT statement
void Interpreter::executeOutput(OutputNode *node) {
    // In this implementation, 'OUTPUT' sets a flag in the DATA step execution to add the current row
    // The actual addition to the dataset is handled in 'executeDataStep'
    // However, to make this explicit, you can modify 'currentRow' if needed
    logLogger.info("OUTPUT statement executed. Current row will be added to the output dataset.");
    // Optionally, set a flag or manipulate 'currentRow' here
}

// Execute an OPTIONS statement
void Interpreter::executeOptions(OptionsNode* node) {
    for (const auto& opt : node->options) {
        env.setOption(opt.first, opt.second);
        logLogger.info("Set option {} = {}", opt.first, opt.second);
    }
}

// Execute a LIBNAME statement
void Interpreter::executeLibname(LibnameNode* node) {
    int rc = env.defineLibrary(node->libref, node->path, node->accessMode);
    if (rc == 0)
    {
        logLogger.info("NOTE: Libref {} was successfully assigned as follows:", node->libref);
        logLogger.info("      Engine:        V9");
        logLogger.info("      Physical Name : {}", node->path);
    }
    else {
        logLogger.info("NOTE: Library {} does not exist.", node->libref);
    }
}

// Execute a TITLE statement
void Interpreter::executeTitle(TitleNode* node) {
    env.setTitle(node->title);
    logLogger.info("Title set to: '{}'", node->title);
    lstLogger.info("Title: {}", env.title);
}

// Execute a PROC step
void Interpreter::executeProc(ProcNode* node) {
    if (auto procSort = dynamic_cast<ProcSortNode*>(node)) {
        executeProcSort(procSort);
    }
    else if (auto procMeans = dynamic_cast<ProcMeansNode*>(node)) {
        executeProcMeans(procMeans);
    }
    else if (auto procFreq = dynamic_cast<ProcFreqNode*>(node)) {
        executeProcFreq(procFreq);
    }
    else if (auto procPrint = dynamic_cast<ProcPrintNode*>(node)) {
        executeProcPrint(procPrint);
    }
    else if (auto procSQL = dynamic_cast<ProcSQLNode*>(node)) {
        executeProcSQL(procSQL);
    }
    else {
        throw std::runtime_error("Unsupported PROC type.");
    }
}

// Convert Value to number (double)
double Interpreter::toNumber(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        return std::get<double>(v);
    }
    else if (std::holds_alternative<std::string>(v)) {
        try {
            return std::stod(std::get<std::string>(v));
        }
        catch (...) {
            return 0.0; // Represent missing as 0.0 or handle differently
        }
    }
    return 0.0;
}

// Convert Value to string
std::string Interpreter::toString(const Value &v) {
    if (std::holds_alternative<double>(v)) {
        // Remove trailing zeros for cleaner output
        std::string numStr = std::to_string(std::get<double>(v));
        numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
        if (numStr.back() == '.') numStr.pop_back();
        return numStr;
    }
    else {
        return std::get<std::string>(v);
    }
}

Value Interpreter::getVariable(const std::string& varName) const {
    return env.getVariable(varName);
}

void Interpreter::setVariable(const std::string& varName, const Value& val) {
    env.setVariable(varName, val);
}

// Evaluate an expression node
Value Interpreter::evaluate(ASTNode *node) {
    if (auto numNode = dynamic_cast<NumberNode*>(node)) {
        return numNode->value;
    }
    else if (auto strNode = dynamic_cast<StringNode*>(node)) {
        return strNode->value;
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        auto it = env.variables.find(var->varName);
        if (it != env.variables.end()) {
            return it->second;
        }
        else {
            // Variable not found, return missing value
            logLogger.warn("Variable '{}' not found. Using missing value.", var->varName);
            return std::nan("");
        }
    }
    else if (auto funcCall = dynamic_cast<FunctionCallNode*>(node)) {
        return evaluateFunctionCall(funcCall);
    }
    else if (auto arrayElem = dynamic_cast<ArrayElementNode*>(node)) {
        int index = static_cast<int>(toNumber(evaluate(arrayElem->index.get())));
        return getArrayElement(arrayElem->arrayName, index);
    }
    else if (auto bin = dynamic_cast<BinaryOpNode*>(node)) {
        Value leftVal = evaluate(bin->left.get());
        Value rightVal = evaluate(bin->right.get());
        std::string op = bin->op;

        double l = toNumber(leftVal);
        double r = toNumber(rightVal);

        if (op == "+") return l + r;
        else if (op == "-") return l - r;
        else if (op == "*") return l * r;
        else if (op == "/") return (r != 0.0) ? l / r : std::nan("");
        else if (op == ">") return (l > r) ? 1.0 : 0.0;
        else if (op == "<") return (l < r) ? 1.0 : 0.0;
        else if (op == ">=") return (l >= r) ? 1.0 : 0.0;
        else if (op == "<=") return (l <= r) ? 1.0 : 0.0;
        else if (op == "==") return (l == r) ? 1.0 : 0.0;
        else if (op == "!=") return (l != r) ? 1.0 : 0.0;
        else if (op == "and") return ((l != 0.0) && (r != 0.0)) ? 1.0 : 0.0;
        else if (op == "or") return ((l != 0.0) || (r != 0.0)) ? 1.0 : 0.0;
        else {
            throw std::runtime_error("Unsupported binary operator: " + op);
        }
    }
    // Handle more expression types as needed
    throw std::runtime_error("Unsupported expression type during evaluation.");
}

// Execute a DROP statement
void Interpreter::executeDrop(DropNode* node) {
    for (const auto& var : node->variables) {
        env.currentRow.columns.erase(var);
        logLogger.info("Dropped variable '{}'.", var);
    }
}

// Execute a KEEP statement
void Interpreter::executeKeep(KeepNode* node) {
    // Retain only the specified variables
    std::vector<std::string> currentVars;
    for (const auto& varPair : env.currentRow.columns) {
        currentVars.push_back(varPair.first);
    }

    for (const auto& var : currentVars) {
        if (std::find(node->variables.begin(), node->variables.end(), var) == node->variables.end()) {
            env.currentRow.columns.erase(var);
            logLogger.info("Kept variable '{}'; other variables dropped.", var);
        }
    }
}

// Execute a RETAIN statement
void Interpreter::executeRetain(RetainNode* node) {
    for (const auto& var : node->variables) {
        retainVars.push_back(var);
        logLogger.info("Retained variable '{}'.", var);
    }
}

// Execute an ARRAY statement
void Interpreter::executeArray(ArrayNode* node) {
    logLogger.info("Executing ARRAY declaration: {}", node->arrayName);
    // Validate array size
    if (node->size != static_cast<int>(node->variables.size())) {
        throw std::runtime_error("Array size does not match the number of variables.");
    }

    // Store the array in the interpreter's array map
    arrays[node->arrayName] = node->variables;

    logLogger.info("Array '{}' with size {} and variables: {}", node->arrayName, node->size,
        [&]() -> std::string {
            std::string vars;
            for (const auto& var : node->variables) {
                vars += var + " ";
            }
            return vars;
        }());
}

Value Interpreter::getArrayElement(const std::string& arrayName, int index) {
    if (arrays.find(arrayName) == arrays.end()) {
        throw std::runtime_error("Undefined array: " + arrayName);
    }
    if (index < 1 || index > static_cast<int>(arrays[arrayName].size())) {
        throw std::runtime_error("Array index out of bounds for array: " + arrayName);
    }
    std::string varName = arrays[arrayName][index - 1];
    auto it = env.currentRow.columns.find(varName);
    if (it != env.currentRow.columns.end()) {
        return it->second;
    }
    else {
        // Variable not found, assume missing value represented as 0 or empty string
        return 0.0; // or throw an error based on SAS behavior
    }
}

void Interpreter::setArrayElement(const std::string& arrayName, int index, const Value& value) {
    if (arrays.find(arrayName) == arrays.end()) {
        throw std::runtime_error("Undefined array: " + arrayName);
    }
    if (index < 1 || index > static_cast<int>(arrays[arrayName].size())) {
        throw std::runtime_error("Array index out of bounds for array: " + arrayName);
    }
    std::string varName = arrays[arrayName][index - 1];
    env.currentRow.columns[varName] = value;
}


// Execute a DO loop
void Interpreter::executeDo(DoNode* node) {
    // Evaluate start and end expressions
    Value startVal = evaluate(node->startExpr.get());
    Value endVal = evaluate(node->endExpr.get());
    double start = toNumber(startVal);
    double end = toNumber(endVal);
    double increment = 1.0; // Default increment

    if (node->incrementExpr) {
        Value incVal = evaluate(node->incrementExpr.get());
        increment = toNumber(incVal);
    }

    logLogger.info("Starting DO loop: {} = {} to {} by {}", node->loopVar, start, end, increment);

    // Initialize loop variable
    env.currentRow.columns[node->loopVar] = start;

    // Loop
    if (increment > 0) {
        while (env.currentRow.columns[node->loopVar].index() == 0 && std::get<double>(env.currentRow.columns[node->loopVar]) <= end) {
            // Execute loop statements
            for (const auto& stmt : node->statements) {
                if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                    executeAssignment(assign);
                }
                else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt.get())) {
                    executeIfThen(ifThen);
                }
                else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                    executeOutput(out);
                }
                else if (auto drop = dynamic_cast<DropNode*>(stmt.get())) {
                    executeDrop(drop);
                }
                else if (auto keep = dynamic_cast<KeepNode*>(stmt.get())) {
                    executeKeep(keep);
                }
                else if (auto retain = dynamic_cast<RetainNode*>(stmt.get())) {
                    executeRetain(retain);
                }
                else if (auto array = dynamic_cast<ArrayNode*>(stmt.get())) {
                    executeArray(array);
                }
                else if (auto doNode = dynamic_cast<DoNode*>(stmt.get())) {
                    executeDo(doNode);
                }
                else {
                    // Handle other DATA step statements if needed
                    throw std::runtime_error("Unsupported statement in DO loop.");
                }
            }

            // Increment loop variable
            double currentVal = toNumber(env.currentRow.columns[node->loopVar]);
            currentVal += increment;
            env.currentRow.columns[node->loopVar] = currentVal;
        }
    }
    else if (increment < 0) {
        while (env.currentRow.columns[node->loopVar].index() == 0 && std::get<double>(env.currentRow.columns[node->loopVar]) >= end) {
            // Execute loop statements
            for (const auto& stmt : node->statements) {
                if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                    executeAssignment(assign);
                }
                else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt.get())) {
                    executeIfThen(ifThen);
                }
                else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                    executeOutput(out);
                }
                else if (auto drop = dynamic_cast<DropNode*>(stmt.get())) {
                    executeDrop(drop);
                }
                else if (auto keep = dynamic_cast<KeepNode*>(stmt.get())) {
                    executeKeep(keep);
                }
                else if (auto retain = dynamic_cast<RetainNode*>(stmt.get())) {
                    executeRetain(retain);
                }
                else if (auto array = dynamic_cast<ArrayNode*>(stmt.get())) {
                    executeArray(array);
                }
                else if (auto doNode = dynamic_cast<DoNode*>(stmt.get())) {
                    executeDo(doNode);
                }
                else {
                    // Handle other DATA step statements if needed
                    throw std::runtime_error("Unsupported statement in DO loop.");
                }
            }

            // Increment loop variable
            double currentVal = toNumber(env.currentRow.columns[node->loopVar]);
            currentVal += increment;
            env.currentRow.columns[node->loopVar] = currentVal;
        }
    }
    else {
        throw std::runtime_error("DO loop increment cannot be zero.");
    }

    logLogger.info("Completed DO loop: {} reached {}", node->loopVar, env.currentRow.columns[node->loopVar].index() == 0 ? toString(env.currentRow.columns[node->loopVar]) : "unknown");
}

void Interpreter::executeProcSort(ProcSortNode* node) {
    logLogger.info("Executing PROC SORT");

    // Retrieve the input dataset
    Dataset* inputDS = env.getOrCreateDataset(node->inputDataSet).get();
    if (!inputDS) {
        throw std::runtime_error("Input dataset '" + node->inputDataSet.getFullDsName() + "' not found for PROC SORT.");
    }

    // Apply WHERE condition if specified
    Dataset* filteredDS = inputDS;
    if (node->whereCondition) {
        // Create a temporary dataset to hold filtered rows
        DatasetRefNode dsNode;
        dsNode.dataName = "TEMP_SORT_FILTERED";

        auto tempDS = env.getOrCreateDataset(dsNode);
        tempDS->rows.clear();

        for (const auto& row : inputDS->rows) {
            env.currentRow = row;
            Value condValue = evaluate(node->whereCondition.get());
            bool conditionTrue = false;
            if (std::holds_alternative<double>(condValue)) {
                conditionTrue = (std::get<double>(condValue) != 0.0);
            }
            else if (std::holds_alternative<std::string>(condValue)) {
                conditionTrue = (!std::get<std::string>(condValue).empty());
            }
            // Add other data types as needed

            if (conditionTrue) {
                tempDS->rows.push_back(row);
            }
        }

        filteredDS = tempDS.get();
        logLogger.info("Applied WHERE condition. {} observations remain after filtering.", filteredDS->rows.size());
    }

    // Sort the filtered dataset by BY variables
    Sorter::sortDataset(filteredDS, node->byVariables);
    logLogger.info("Sorted dataset '{}' by variables: {}",
        filteredDS->name,
        std::accumulate(node->byVariables.begin(), node->byVariables.end(), std::string(),
            [](const std::string& a, const std::string& b) -> std::string {
                return a.empty() ? b : a + ", " + b;
            }));

    // Handle NODUPKEY option
    Dataset* sortedDS = filteredDS;
    if (node->nodupkey) {
        DatasetRefNode dsNode;
        dsNode.dataName = "TEMP_SORT_NODUPKEY";
        auto tempDS = env.getOrCreateDataset(dsNode);
        tempDS->rows.clear();

        std::unordered_set<std::string> seenKeys;
        for (const auto& row : sortedDS->rows) {
            std::string key = "";
            for (const auto& var : node->byVariables) {
                auto it = row.columns.find(var);
                if (it != row.columns.end()) {
                    if (std::holds_alternative<double>(it->second)) {
                        key += std::to_string(std::get<double>(it->second)) + "_";
                    }
                    else if (std::holds_alternative<std::string>(it->second)) {
                        key += std::get<std::string>(it->second) + "_";
                    }
                    // Handle other data types as needed
                }
                else {
                    key += "NA_";
                }
            }

            if (seenKeys.find(key) == seenKeys.end()) {
                tempDS->rows.push_back(row);
                seenKeys.insert(key);
            }
            else {
                logLogger.info("Duplicate key '{}' found. Skipping duplicate observation.", key);
            }
        }

        sortedDS = tempDS.get();
        logLogger.info("Applied NODUPKEY option. {} observations remain after removing duplicates.", sortedDS->rows.size());
    }

    // Handle DUPLICATES option
    if (node->duplicates) {
        std::unordered_set<std::string> seenKeys;
        for (const auto& row : sortedDS->rows) {
            std::string key = "";
            for (const auto& var : node->byVariables) {
                auto it = row.columns.find(var);
                if (it != row.columns.end()) {
                    if (std::holds_alternative<double>(it->second)) {
                        key += std::to_string(std::get<double>(it->second)) + "_";
                    }
                    else if (std::holds_alternative<std::string>(it->second)) {
                        key += std::get<std::string>(it->second) + "_";
                    }
                    // Handle other data types as needed
                }
                else {
                    key += "NA_";
                }
            }

            if (seenKeys.find(key) != seenKeys.end()) {
                logLogger.info("Duplicate key '{}' found.", key);
            }
            else {
                seenKeys.insert(key);
            }
        }
    }

    // Determine the output dataset
    DatasetRefNode dsNode = node->outputDataSet.dataName.empty() ? node->inputDataSet : node->outputDataSet;
    Dataset* outputDS = env.getOrCreateDataset(dsNode).get();
    if (dsNode.dataName != sortedDS->name) {
        // Copy sortedDS to outputDS
        outputDS->rows = sortedDS->rows;
        logLogger.info("Sorted data copied to output dataset '{}'.", dsNode.getFullDsName());
    }
    else {
        // Overwrite the input dataset
        inputDS->rows = sortedDS->rows;
        logLogger.info("Input dataset '{}' overwritten with sorted data.", inputDS->name);
    }

    logLogger.info("PROC SORT executed successfully. Output dataset '{}' has {} observations.",
        dsNode.getFullDsName(), outputDS->rows.size());
}

void Interpreter::executeProcMeans(ProcMeansNode* node) {
    logLogger.info("Executing PROC MEANS");

    // Retrieve the input dataset
    Dataset* inputDS = env.getOrCreateDataset(node->inputDataSet).get();
    if (!inputDS) {
        throw std::runtime_error("Input dataset '" + node->inputDataSet.getFullDsName() + "' not found for PROC MEANS.");
    }

    // Apply WHERE condition if specified
    Dataset* filteredDS = inputDS;
    if (node->whereCondition) {
        // Create a temporary dataset to hold filtered rows
        DatasetRefNode dsNode;
        dsNode.dataName = "TEMP_MEANS_FILTERED";
        auto tempDS = env.getOrCreateDataset(dsNode);
        tempDS->rows.clear();

        for (const auto& row : inputDS->rows) {
            env.currentRow = row;
            Value condValue = evaluate(node->whereCondition.get());
            bool conditionTrue = false;
            if (std::holds_alternative<double>(condValue)) {
                conditionTrue = (std::get<double>(condValue) != 0.0);
            }
            else if (std::holds_alternative<std::string>(condValue)) {
                conditionTrue = (!std::get<std::string>(condValue).empty());
            }
            // Add other data types as needed

            if (conditionTrue) {
                tempDS->rows.push_back(row);
            }
        }

        filteredDS = tempDS.get();
        logLogger.info("Applied WHERE condition. {} observations remain after filtering.", filteredDS->rows.size());
    }

    // Initialize statistics containers
    struct Stats {
        int n = 0;
        double mean = 0.0;
        double median = 0.0;
        double stddev = 0.0;
        double min = 0.0;
        double max = 0.0;
        std::vector<double> values; // For median calculation
    };

    std::unordered_map<std::string, Stats> statisticsMap;

    // Initialize Stats for each variable
    for (const auto& var : node->varVariables) {
        statisticsMap[var] = Stats();
    }

    // Calculate statistics
    for (const auto& row : filteredDS->rows) {
        for (const auto& var : node->varVariables) {
            auto it = row.columns.find(var);
            if (it != row.columns.end() && std::holds_alternative<double>(it->second)) {
                double val = std::get<double>(it->second);
                statisticsMap[var].n += 1;
                statisticsMap[var].mean += val;
                statisticsMap[var].values.push_back(val);
                if (statisticsMap[var].n == 1 || val < statisticsMap[var].min) {
                    statisticsMap[var].min = val;
                }
                if (statisticsMap[var].n == 1 || val > statisticsMap[var].max) {
                    statisticsMap[var].max = val;
                }
            }
        }
    }

    // Finalize mean and calculate stddev and median
    for (auto& entry : statisticsMap) {
        std::string var = entry.first;
        Stats& stats = entry.second;

        if (stats.n > 0) {
            stats.mean /= stats.n;

            // Calculate standard deviation
            double sumSquares = 0.0;
            for (const auto& val : stats.values) {
                sumSquares += (val - stats.mean) * (val - stats.mean);
            }
            stats.stddev = std::sqrt(sumSquares / (stats.n - 1));

            // Calculate median
            std::vector<double> sortedValues = stats.values;
            std::sort(sortedValues.begin(), sortedValues.end());
            if (stats.n % 2 == 1) {
                stats.median = sortedValues[stats.n / 2];
            }
            else {
                stats.median = (sortedValues[(stats.n / 2) - 1] + sortedValues[stats.n / 2]) / 2.0;
            }
        }
    }

    // Prepare output dataset if specified
    Dataset* outputDS = nullptr;
    if (!node->outputDataSet.dataName.empty()) {
        outputDS = env.getOrCreateDataset(node->outputDataSet).get();
        outputDS->rows.clear();
    }

    // Generate statistics output
    logLogger.info("Generated PROC MEANS statistics:");
    for (const auto& var : node->varVariables) {
        const Stats& stats = statisticsMap[var];
        if (stats.n > 0) {
            std::stringstream ss;
            ss << "Variable: " << var << "\n";
            for (const auto& stat : node->statistics) {
                if (stat == "N") {
                    ss << "  N: " << stats.n << "\n";
                }
                else if (stat == "MEAN") {
                    ss << "  Mean: " << stats.mean << "\n";
                }
                else if (stat == "MEDIAN") {
                    ss << "  Median: " << stats.median << "\n";
                }
                else if (stat == "STD") {
                    ss << "  Std Dev: " << stats.stddev << "\n";
                }
                else if (stat == "MIN") {
                    ss << "  Min: " << stats.min << "\n";
                }
                else if (stat == "MAX") {
                    ss << "  Max: " << stats.max << "\n";
                }
            }
            logLogger.info(ss.str());

            if (outputDS) {
                // Create a row for each statistic
                Row statRow;
                statRow.columns["Variable"] = var;
                for (const auto& stat : node->statistics) {
                    if (stat == "N") {
                        statRow.columns["N"] = static_cast<double>(stats.n);
                    }
                    else if (stat == "MEAN") {
                        statRow.columns["Mean"] = stats.mean;
                    }
                    else if (stat == "MEDIAN") {
                        statRow.columns["Median"] = stats.median;
                    }
                    else if (stat == "STD") {
                        statRow.columns["StdDev"] = stats.stddev;
                    }
                    else if (stat == "MIN") {
                        statRow.columns["Min"] = stats.min;
                    }
                    else if (stat == "MAX") {
                        statRow.columns["Max"] = stats.max;
                    }
                }
                outputDS->rows.push_back(statRow);
            }
        }
        else {
            logLogger.warn("Variable '{}' has no valid observations for PROC MEANS.", var);
        }
    }

    // If OUTPUT dataset is specified, log its creation
    if (outputDS) {
        logLogger.info("PROC MEANS output dataset '{}' created with {} observations.",
            node->outputDataSet.getFullDsName(), outputDS->rows.size());
    }

    logLogger.info("PROC MEANS executed successfully.");
}

void Interpreter::executeIfElse(IfElseIfNode* node) {
    // Evaluate primary IF condition
    Value cond = evaluate(node->condition.get());
    double d = toNumber(cond);
    logLogger.info("Evaluating IF condition: {}", d);

    if (d != 0.0) { // Non-zero is true
        logLogger.info("Condition is TRUE. Executing THEN statements.");
        for (const auto& stmt : node->thenStatements) {
            if (auto block = dynamic_cast<BlockNode*>(stmt.get())) {
                executeBlock(block);
            }
            else {
                execute(stmt.get());
            }
        }
        return;
    }

    // Iterate through ELSE IF branches
    for (const auto& branch : node->elseIfBranches) {
        Value elseIfCond = evaluate(branch.first.get());
        double elseIfD = toNumber(elseIfCond);
        logLogger.info("Evaluating ELSE IF condition: {}", elseIfD);

        if (elseIfD != 0.0) { // Non-zero is true
            logLogger.info("ELSE IF condition is TRUE. Executing ELSE IF statements.");
            for (const auto& stmt : branch.second) {
                if (auto block = dynamic_cast<BlockNode*>(stmt.get())) {
                    executeBlock(block);
                }
                else {
                    execute(stmt.get());
                }
            }
            return;
        }
    }

    // Execute ELSE statements if no conditions were true
    if (!node->elseStatements.empty()) {
        logLogger.info("All conditions FALSE. Executing ELSE statements.");
        for (const auto& stmt : node->elseStatements) {
            if (auto block = dynamic_cast<BlockNode*>(stmt.get())) {
                executeBlock(block);
            }
            else {
                execute(stmt.get());
            }
        }
    }
    else {
        logLogger.info("All conditions FALSE. No ELSE statements to execute.");
    }
}


void Interpreter::executeBlock(BlockNode* node) {
    for (const auto& stmt : node->statements) {
        execute(stmt.get());
    }
}

Value Interpreter::evaluateFunctionCall(FunctionCallNode* node) {
    std::string func = node->functionName;
    // Convert function name to lowercase for case-insensitive matching
    std::transform(func.begin(), func.end(), func.begin(), ::tolower);

    if (func == "substr") {
        // substr(string, position, length)
        if (node->arguments.size() < 2 || node->arguments.size() > 3) {
            throw std::runtime_error("substr function expects 2 or 3 arguments.");
        }
        std::string str = std::get<std::string>(evaluate(node->arguments[0].get()));
        double pos = toNumber(evaluate(node->arguments[1].get()));
        int position = static_cast<int>(pos) - 1; // SAS substr is 1-based
        int length = (node->arguments.size() == 3) ? static_cast<int>(toNumber(evaluate(node->arguments[2].get()))) : str.length() - position;
        if (position < 0 || position >= static_cast<int>(str.length())) {
            return std::string(""); // Out of bounds
        }
        if (position + length > static_cast<int>(str.length())) {
            length = str.length() - position;
        }
        return str.substr(position, length);
    }
    else if (func == "trim") {
        // trim(string)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("trim function expects 1 argument.");
        }
        std::string str = std::get<std::string>(evaluate(node->arguments[0].get()));
        // Remove trailing whitespace
        size_t endpos = str.find_last_not_of(" \t\r\n");
        if (std::string::npos != endpos) {
            str = str.substr(0, endpos + 1);
        }
        else {
            str.clear(); // All spaces
        }
        return str;
    }
    else if (func == "left") {
        // left(string)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("left function expects 1 argument.");
        }
        std::string str = std::get<std::string>(evaluate(node->arguments[0].get()));
        // Remove leading whitespace
        size_t startpos = str.find_first_not_of(" \t\r\n");
        if (std::string::npos != startpos) {
            str = str.substr(startpos);
        }
        else {
            str.clear(); // All spaces
        }
        return str;
    }
    else if (func == "right") {
        // right(string)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("right function expects 1 argument.");
        }
        std::string str = std::get<std::string>(evaluate(node->arguments[0].get()));
        // Remove trailing whitespace
        size_t endpos = str.find_last_not_of(" \t\r\n");
        if (std::string::npos != endpos) {
            str = str.substr(0, endpos + 1);
        }
        else {
            str.clear(); // All spaces
        }
        return str;
    }
    else if (func == "upcase") {
        // upcase(string)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("upcase function expects 1 argument.");
        }
        std::string str = std::get<std::string>(evaluate(node->arguments[0].get()));
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }
    else if (func == "lowcase") {
        // lowcase(string)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("lowcase function expects 1 argument.");
        }
        std::string str = std::get<std::string>(evaluate(node->arguments[0].get()));
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    else if (func == "sqrt") {
        Value argVal = evaluate(node->arguments[0].get());
        double argNum = toNumber(argVal);

        if (argNum < 0) {
            logLogger.warn("sqrt() received a negative value. Returning NaN.");
            return std::nan("");
        }
        return std::sqrt(argNum);
    }
    else if (func == "abs") {
        // abs(number)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("abs function expects 1 argument.");
        }
        double num = toNumber(evaluate(node->arguments[0].get()));
        return std::abs(num);
    }
    else if (func == "log") {
        Value argVal = evaluate(node->arguments[0].get());
        double argNum = toNumber(argVal);

        if (argNum <= 0) {
            logLogger.warn("log() received a non-positive value. Returning NaN.");
            return std::nan("");
        }
        return std::log(argNum);
    }
    else if (func == "ceil") {
        // ceil(number)
        if (node->arguments.size() != 1) {
            throw std::runtime_error("ceil function expects 1 argument.");
        }
        double num = toNumber(evaluate(node->arguments[0].get()));
        return std::ceil(num);
        }
    else if (func == "floor") {
            // floor(number)
            if (node->arguments.size() != 1) {
                throw std::runtime_error("floor function expects 1 argument.");
            }
            double num = toNumber(evaluate(node->arguments[0].get()));
            return std::floor(num);
            }
    else if (func == "round") {
                // round(number, decimal_places)
                if (node->arguments.size() < 1 || node->arguments.size() > 2) {
                    throw std::runtime_error("round function expects 1 or 2 arguments.");
                }
                double num = toNumber(evaluate(node->arguments[0].get()));
                int decimal = 0;
                if (node->arguments.size() == 2) {
                    decimal = static_cast<int>(toNumber(evaluate(node->arguments[1].get())));
                }
                double factor = std::pow(10.0, decimal);
                return std::round(num * factor) / factor;
                }
    else if (func == "exp") {
                    // exp(number)
                    if (node->arguments.size() != 1) {
                        throw std::runtime_error("exp function expects 1 argument.");
                    }
                    double num = toNumber(evaluate(node->arguments[0].get()));
                    return std::exp(num);
                    }
    else if (func == "log10") {
                        // log10(number)
                        if (node->arguments.size() != 1) {
                            throw std::runtime_error("log10 function expects 1 argument.");
                        }
                        double num = toNumber(evaluate(node->arguments[0].get()));
                        if (num <= 0.0) {
                            throw std::runtime_error("log10 function argument must be positive.");
                        }
                        return std::log10(num);
                        }
    // Date and Time Functions
    else if (func == "today") {
        // today()
        if (node->arguments.size() != 0) {
            throw std::runtime_error("today function expects no arguments.");
        }
        std::time_t t = std::time(nullptr);
        std::tm* tm_ptr = std::localtime(&t);
        // Return date as YYYYMMDD integer
        int year = tm_ptr->tm_year + 1900;
        int month = tm_ptr->tm_mon + 1;
        int day = tm_ptr->tm_mday;
        int date_int = year * 10000 + month * 100 + day;
        return static_cast<double>(date_int);
        }
    else if (func == "datepart") {
            // datepart(datetime)
            if (node->arguments.size() != 1) {
                throw std::runtime_error("datepart function expects 1 argument.");
            }
            double datetime = toNumber(evaluate(node->arguments[0].get()));
            // Assuming datetime is in SAS datetime format (seconds since 1960-01-01)
            // Convert to date as YYYYMMDD
            // Placeholder implementation
            // Implement actual conversion based on SAS datetime format
            // For simplicity, return the datetime as is
            return datetime;
            }
    else if (func == "timepart") {
                // timepart(datetime)
                if (node->arguments.size() != 1) {
                    throw std::runtime_error("timepart function expects 1 argument.");
                }
                double datetime = toNumber(evaluate(node->arguments[0].get()));
                // Assuming datetime is in SAS datetime format (seconds since 1960-01-01)
                // Convert to time as HHMMSS
                // Placeholder implementation
                // Implement actual conversion based on SAS datetime format
                // For simplicity, return the datetime as is
                return datetime;
                }
    else if (func == "intck") {
                    // intck(interval, start_date, end_date)
                    if (node->arguments.size() != 3) {
                        throw std::runtime_error("intck function expects 3 arguments.");
                    }
                    std::string interval = std::get<std::string>(evaluate(node->arguments[0].get()));
                    double start = toNumber(evaluate(node->arguments[1].get()));
                    double end = toNumber(evaluate(node->arguments[2].get()));

                    // Placeholder implementation for 'day' interval
                    if (interval == "day") {
                        int days = static_cast<int>(end - start);
                        return static_cast<double>(days);
                    }
                    else {
                        throw std::runtime_error("Unsupported interval in intck function: " + interval);
                    }
                    }
    else if (func == "intnx") {
		// intnx(interval, start_date, increment, alignment)
		if (node->arguments.size() < 3 || node->arguments.size() > 4) {
			throw std::runtime_error("intnx function expects 3 or 4 arguments.");
		}
		std::string interval = std::get<std::string>(evaluate(node->arguments[0].get()));
		double start = toNumber(evaluate(node->arguments[1].get()));
		double increment = toNumber(evaluate(node->arguments[2].get()));
		std::string alignment = "beginning"; // Default alignment
		if (node->arguments.size() == 4) {
			alignment = std::get<std::string>(evaluate(node->arguments[3].get()));
		}

		// Placeholder implementation for 'day' interval
		if (interval == "day") {
			double new_date = start + increment;
			return new_date;
		}
		else {
			throw std::runtime_error("Unsupported interval in intnx function: " + interval);
		}
	}
    else {
        throw std::runtime_error("Unsupported function: " + func);
    }
}

void Interpreter::executeMerge(MergeStatementNode* node) {
    // Ensure all datasets exist
    std::vector<Dataset*> mergeDatasets;
    for (auto& dsNode : node->datasets) {
        Dataset* ds = env.getOrCreateDataset(dsNode).get();
        if (!ds) {
            throw std::runtime_error("Dataset not found for MERGE: " + dsNode.getFullDsName());
        }
        mergeDatasets.push_back(ds);
    }

    // Check if BY statement has been specified
    if (byVariables.empty()) {
        throw std::runtime_error("MERGE statement requires a preceding BY statement.");
    }

    // Sort all datasets by BY variables
    for (auto ds : mergeDatasets) {
        Sorter::sortDataset(ds, byVariables);
        logLogger.info("Dataset '{}' sorted by BY variables.", ds->name);
    }

    // Initialize iterators for each dataset
    std::vector<size_t> iterators(mergeDatasets.size(), 0);
    size_t numDatasets = mergeDatasets.size();

    // Create or clear the output dataset
    auto outputDataSet = env.getCurrentDataSet();
    outputDataSet->rows.clear();

    bool continueMerging = true;

    while (continueMerging) {
        // Collect current BY variable values from each dataset
        std::vector<std::vector<double>> currentBYValues(numDatasets, std::vector<double>());
        bool anyDatasetHasRows = false;

        for (size_t i = 0; i < numDatasets; ++i) {
            if (iterators[i] < mergeDatasets[i]->rows.size()) {
                anyDatasetHasRows = true;
                const Row& row = mergeDatasets[i]->rows[iterators[i]];
                std::vector<double> byVals;
                for (const auto& var : byVariables) {
                    double val = 0.0;
                    auto it = row.columns.find(var);
                    if (it != row.columns.end() && std::holds_alternative<double>(it->second)) {
                        val = std::get<double>(it->second);
                    }
                    byVals.push_back(val);
                }
                currentBYValues[i] = byVals;
            }
        }

        if (!anyDatasetHasRows) {
            break; // All datasets have been fully iterated
        }

        // Check for data type consistency across datasets for BY variables
        for (size_t j = 0; j < byVariables.size(); ++j) {
            double referenceVal = currentBYValues[0][j];
            for (size_t i = 1; i < numDatasets; ++i) {
                if (currentBYValues[i][j] != referenceVal) {
                    throw std::runtime_error("Data type mismatch for BY variable '" + byVariables[j] + "' across datasets.");
                }
            }
        }

        // Determine the minimum BY values across datasets
        std::vector<double> minBYValues = currentBYValues[0];
        for (size_t i = 1; i < numDatasets; ++i) {
            for (size_t j = 0; j < byVariables.size(); ++j) {
                if (currentBYValues[i][j] < minBYValues[j]) {
                    minBYValues[j] = currentBYValues[i][j];
                }
                else if (currentBYValues[i][j] > minBYValues[j]) {
                    // No change
                }
            }
        }

        // Collect all rows from datasets that match the min BY values
        std::vector<Row> matchedRows;
        for (size_t i = 0; i < numDatasets; ++i) {
            if (iterators[i] < mergeDatasets[i]->rows.size()) {
                bool match = true;
                for (size_t j = 0; j < byVariables.size(); ++j) {
                    if (currentBYValues[i][j] != minBYValues[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    matchedRows.push_back(mergeDatasets[i]->rows[iterators[i]]);
                    iterators[i]++; // Move iterator forward
                }
            }
        }

        bool allDatasetsHaveRows = true;
        for (size_t i = 0; i < numDatasets; ++i) {
            if (iterators[i] >= mergeDatasets[i]->rows.size()) {
                allDatasetsHaveRows = false;
                break;
            }
        }

        if (!allDatasetsHaveRows) {
            // Handle unmatched keys by including remaining rows from datasets
            for (size_t i = 0; i < numDatasets; ++i) {
                while (iterators[i] < mergeDatasets[i]->rows.size()) {
                    Row rowCopy = mergeDatasets[i]->rows[iterators[i]];
                    iterators[i]++;
                    // Merge with existing outputRow if necessary
                    outputDataSet->rows.push_back(rowCopy);
                }
            }
            break;
        }

        // Collect existing variable names to detect conflicts
        std::unordered_set<std::string> existingVars;
        for (const auto& var : byVariables) {
            existingVars.insert(var);
        }

        // Merge the matched rows into a single row
        Row mergedRow;
        for (const auto& row : matchedRows) {
            for (const auto& col : row.columns) {
                // Avoid overwriting BY variables
                if (std::find(byVariables.begin(), byVariables.end(), col.first) != byVariables.end()) {
                    mergedRow.columns[col.first] = col.second;
                }
                else {
                    // Handle variable name conflicts by prefixing with dataset name
                    if (mergedRow.columns.find(col.first) == mergedRow.columns.end()) {
                        mergedRow.columns[col.first] = col.second;
                    }
                    else {
                        std::string newColName = row.columns.begin()->first + "_" + col.first;
                        mergedRow.columns[newColName] = col.second;
                    }
                }
            }
        }

        // Append the merged row to the output dataset
        outputDataSet->rows.push_back(mergedRow);
    }

    logLogger.info("MERGE statement executed successfully. Output dataset '{}' has {} observations.",
        outputDataSet->name, outputDataSet->rows.size());
}


void Interpreter::executeBy(ByStatementNode* node) {
    logLogger.info("Executing BY statement with variables:");
    for (const auto& var : node->variables) {
        logLogger.info(" - {}", var);
    }

    // Store the BY variables in the interpreter's context
    byVariables = node->variables;

    // Ensure that the BY variables are present in all datasets to be merged
    // This can be implemented as needed
}

void Interpreter::executeDoLoop(DoLoopNode* node) {
    logLogger.info("Entering DO loop");

    // Push the loop context onto the stack
    loopStack.emplace(std::make_pair(node, 0));

    // Set a maximum number of iterations to prevent infinite loops
    const size_t MAX_ITERATIONS = 1000;

    while (!loopStack.empty()) {
        DoLoopNode* currentLoop = loopStack.top().first;
        size_t& iterationCount = loopStack.top().second;

        if (iterationCount >= MAX_ITERATIONS) {
            logLogger.error("Potential infinite loop detected in DO loop. Exiting loop.");
            loopStack.pop();
            break;
        }

        bool conditionMet = true;

        if (currentLoop->condition) {
            Value condValue = evaluate(currentLoop->condition.get());
            if (currentLoop->isWhile) {
                conditionMet = std::holds_alternative<double>(condValue) ? std::get<double>(condValue) != 0.0 : false;
            }
            else { // DO UNTIL
                conditionMet = !(std::holds_alternative<double>(condValue) ? std::get<double>(condValue) != 0.0 : false);
            }
        }

        if (conditionMet) {
            // Execute the loop body
            execute(currentLoop->body.get());
        }
        else {
            // Exit the loop
            loopStack.pop();
            logLogger.info("Exiting DO loop");
            break;
        }

        // In this simplified implementation, we assume the loop condition is re-evaluated after each iteration
    }
}

void Interpreter::executeEnd(EndNode* node) {
    if (loopStack.empty()) {
        throw std::runtime_error("END statement encountered without a corresponding DO loop.");
    }

    // Pop the current loop context to signify exiting the loop
    loopStack.pop();
    logLogger.info("Exiting DO loop via END statement");
}

void Interpreter::executeProcFreq(ProcFreqNode* node) {
    logLogger.info("Executing PROC FREQ");

    // Retrieve the input dataset
    Dataset* inputDS = env.getOrCreateDataset(node->inputDataSet).get();
    if (!inputDS) {
        throw std::runtime_error("Input dataset '" + node->inputDataSet.getFullDsName() + "' not found for PROC FREQ.");
    }

    // Apply WHERE condition if specified
    Dataset* filteredDS = inputDS;
    if (node->whereCondition) {
        // Create a temporary dataset to hold filtered rows
        DatasetRefNode dsNode;
        dsNode.dataName = "TEMP_FREQ_FILTERED";
        auto tempDS = env.getOrCreateDataset(dsNode);
        tempDS->rows.clear();

        for (const auto& row : inputDS->rows) {
            env.currentRow = row;
            Value condValue = evaluate(node->whereCondition.get());
            bool conditionTrue = false;
            if (std::holds_alternative<double>(condValue)) {
                conditionTrue = (std::get<double>(condValue) != 0.0);
            }
            else if (std::holds_alternative<std::string>(condValue)) {
                conditionTrue = (!std::get<std::string>(condValue).empty());
            }
            // Add other data types as needed

            if (conditionTrue) {
                tempDS->rows.push_back(row);
            }
        }

        filteredDS = tempDS.get();
        logLogger.info("Applied WHERE condition. {} observations remain after filtering.", filteredDS->rows.size());
    }

    // Process each table specification
    for (const auto& tablePair : node->tables) {
        std::string tableSpec = tablePair.first;
        std::vector<std::string> tableOptions = tablePair.second;

        // Split tableSpec into variables, e.g., var1 or var1*var2
        std::vector<std::string> vars;
        size_t starPos = tableSpec.find('*');
        if (starPos != std::string::npos) {
            vars.push_back(tableSpec.substr(0, starPos));
            vars.push_back(tableSpec.substr(starPos + 1));
        }
        else {
            vars.push_back(tableSpec);
        }

        if (vars.size() == 1) {
            // Single variable frequency table
            std::map<std::string, int> freqMap;
            for (const auto& row : filteredDS->rows) {
                auto it = row.columns.find(vars[0]);
                if (it != row.columns.end()) {
                    std::string key;
                    if (std::holds_alternative<double>(it->second)) {
                        key = std::to_string(std::get<double>(it->second));
                    }
                    else if (std::holds_alternative<std::string>(it->second)) {
                        key = std::get<std::string>(it->second);
                    }
                    // Handle other data types as needed

                    freqMap[key]++;
                }
            }

            // Log frequency table
            std::stringstream ss;
            ss << "Frequency Table for Variable: " << vars[0] << "\n";
            ss << "Value\tFrequency\n";
            for (const auto& pair : freqMap) {
                ss << pair.first << "\t" << pair.second << "\n";
            }
            logLogger.info(ss.str());

            // Handle OUTPUT options if any (e.g., OUT=)
            // This implementation focuses on logging frequencies. Extending to output datasets can be added here.
        }
        else if (vars.size() == 2) {
            // Cross-tabulation
            std::map<std::string, std::map<std::string, int>> crosstab;
            std::set<std::string> var1Levels;
            std::set<std::string> var2Levels;

            for (const auto& row : filteredDS->rows) {
                auto it1 = row.columns.find(vars[0]);
                auto it2 = row.columns.find(vars[1]);

                if (it1 != row.columns.end() && it2 != row.columns.end()) {
                    std::string key1, key2;
                    if (std::holds_alternative<double>(it1->second)) {
                        key1 = std::to_string(std::get<double>(it1->second));
                    }
                    else if (std::holds_alternative<std::string>(it1->second)) {
                        key1 = std::get<std::string>(it1->second);
                    }

                    if (std::holds_alternative<double>(it2->second)) {
                        key2 = std::to_string(std::get<double>(it2->second));
                    }
                    else if (std::holds_alternative<std::string>(it2->second)) {
                        key2 = std::get<std::string>(it2->second);
                    }

                    crosstab[key1][key2]++;
                    var1Levels.insert(key1);
                    var2Levels.insert(key2);
                }
            }

            // Log cross-tabulation table
            std::stringstream ss;
            ss << "Cross-Tabulation Table for Variables: " << vars[0] << " * " << vars[1] << "\n";
            ss << vars[0] << "\\" << vars[1] << "\t";

            for (const auto& var2Level : var2Levels) {
                ss << var2Level << "\t";
            }
            ss << "\n";

            for (const auto& var1Level : var1Levels) {
                ss << var1Level << "\t";
                for (const auto& var2Level : var2Levels) {
                    int count = 0;
                    auto it = crosstab.find(var1Level);
                    if (it != crosstab.end()) {
                        auto it2 = it->second.find(var2Level);
                        if (it2 != it->second.end()) {
                            count = it2->second;
                        }
                    }
                    ss << count << "\t";
                }
                ss << "\n";
            }

            logLogger.info(ss.str());

            // Handle OPTIONS like CHISQ
            for (const auto& option : tableOptions) {
                if (option == "CHISQ") {
                    // Perform Chi-Square Test
                    // This is a simplified implementation. In practice, you would calculate the Chi-Square statistic.
                    logLogger.info("Chi-Square test requested for the cross-tabulation.");
                    // Placeholder for Chi-Square calculation
                }
                // Handle other options as needed
            }

            // Handle OUTPUT options if any (e.g., OUT=)
            // This implementation focuses on logging cross-tabulation. Extending to output datasets can be added here.
        }
        else {
            logLogger.warn("Unsupported number of variables in TABLES statement: {}", vars.size());
        }
    }
}

void Interpreter::executeProcPrint(ProcPrintNode* node) {
    ScopedStepTimer timer("PROCEDURE PRINT", logLogger);

    // Retrieve the input dataset
    Dataset* inputDS = env.getOrCreateDataset(node->inputDataSet).get();
    if (!inputDS) {
        throw std::runtime_error("Input dataset '" + node->inputDataSet.getFullDsName() + "' not found for PROC PRINT.");
    }

    // Determine which variables to print
    std::vector<std::string> varsToPrint;
    if (!node->varVariables.empty()) {
        varsToPrint = node->varVariables;
    }
    else {
        // If VAR statement is not specified, print all variables
        for (const auto& column : inputDS->columnOrder) {
            varsToPrint.push_back(column);
        }
    }

    // Handle options
    int obsLimit = -1; // -1 means no limit
    bool noObs = false;
    bool useLabels = false;

    auto it = node->options.find("OBS");
    if (it != node->options.end()) {
        obsLimit = std::stoi(it->second);
    }

    it = node->options.find("NOOBS");
    if (it != node->options.end()) {
        noObs = true;
    }

    it = node->options.find("LABEL");
    if (it != node->options.end()) {
        useLabels = true;
    }

    // Prepare header
    std::stringstream header;
    if (!noObs) {
        header << "OBS\t";
    }
    for (size_t i = 0; i < varsToPrint.size(); ++i) {
        const std::string& var = varsToPrint[i];
        std::string displayName = var;
        if (useLabels) {
            // Assume variables have labels stored somewhere; placeholder for actual label retrieval
            // For now, use variable names
            displayName = var; // Replace with label if available
        }
        header << displayName;
        if (i != varsToPrint.size() - 1) {
            header << "\t";
        }
    }

    // Log header
    lstLogger.info(env.title);
    lstLogger.info(header.str());

    // Iterate over rows and print data
    int obsCount = 0;
    for (size_t i = 0; i < inputDS->rows.size(); ++i) {
        if (obsLimit != -1 && obsCount >= obsLimit) {
            break;
        }

        const Row& row = inputDS->rows[i];
        std::stringstream rowStream;
        if (!noObs) {
            rowStream << (i + 1) << "\t";
        }

        for (size_t j = 0; j < varsToPrint.size(); ++j) {
            const std::string& var = varsToPrint[j];
            auto itVar = row.columns.find(var);
            if (itVar != row.columns.end()) {
                if (std::holds_alternative<double>(itVar->second)) {
                    rowStream << std::fixed << std::setprecision(2) << std::get<double>(itVar->second);
                }
                else if (std::holds_alternative<std::string>(itVar->second)) {
                    rowStream << std::get<std::string>(itVar->second);
                }
                // Handle other data types as needed
            }
            else {
                rowStream << "NA"; // Handle missing variables
            }

            if (j != varsToPrint.size() - 1) {
                rowStream << "\t";
            }
        }
        logLogger.info(rowStream.str());
        obsCount++;
    }

    logLogger.info("NOTE: There were {} observations read from the data set {}.", inputDS->rows.size(), inputDS->name);
}

void Interpreter::executeProcSQL(ProcSQLNode* node) {
    logLogger.info("Executing PROC SQL");

    for (const auto& sqlStmt : node->statements) {
        if (auto selectStmt = dynamic_cast<SelectStatementNode*>(sqlStmt.get())) {
            Dataset* resultDS = executeSelect(selectStmt);
            // Handle the result dataset as needed
            // For example, if creating a new table, it has been handled in executeSelect
            // Otherwise, you can log the results or perform further actions
        }
        else if (auto createStmt = dynamic_cast<CreateTableStatementNode*>(sqlStmt.get())) {
            executeCreateTable(createStmt);
        }
        else {
            logLogger.warn("Unsupported SQL statement encountered in PROC SQL.");
        }
    }

    logLogger.info("PROC SQL executed successfully.");
}

Dataset* Interpreter::executeSelect(const SelectStatementNode* selectStmt) {
    // For simplicity, handle basic SELECT statements without joins or subqueries
    // Extend this method to handle joins, subqueries, and other SQL features

    // Create a new dataset to store the results
    DatasetRefNode dsNode;
    dsNode.dataName = "SQL_RESULT";
    Dataset* resultDS = env.getOrCreateDataset(dsNode).get();
    resultDS->rows.clear();

    // Determine source tables
    if (selectStmt->fromTables.empty()) {
        throw std::runtime_error("SELECT statement requires at least one table in FROM clause.");
    }

    // For simplicity, handle single table SELECT
    if (selectStmt->fromTables.size() > 1) {
        throw std::runtime_error("Multi-table SELECT statements (joins) are not yet supported.");
    }

    DatasetRefNode dsNodeFrom;
    dsNodeFrom.dataName = selectStmt->fromTables[0];
    Dataset* sourceDS = env.getOrCreateDataset(dsNodeFrom).get();
    if (!sourceDS) {
        throw std::runtime_error("Source table '" + dsNodeFrom.getFullDsName() + "' not found for SELECT statement.");
    }

    // Iterate over source dataset rows and apply WHERE condition
    for (const auto& row : sourceDS->rows) {
        bool includeRow = true;
        if (selectStmt->whereCondition) {
            env.currentRow = row;
            Value condValue = evaluate(selectStmt->whereCondition.get());
            if (std::holds_alternative<double>(condValue)) {
                includeRow = (std::get<double>(condValue) != 0.0);
            }
            else if (std::holds_alternative<std::string>(condValue)) {
                includeRow = (!std::get<std::string>(condValue).empty());
            }
            // Add other data types as needed
        }

        if (includeRow) {
            Row newRow;
            for (const auto& col : selectStmt->selectColumns) {
                auto it = row.columns.find(col);
                if (it != row.columns.end()) {
                    newRow.columns[col] = it->second;
                }
                else {
                    newRow.columns[col] = "NA"; // Handle missing columns
                }
            }
            resultDS->rows.push_back(newRow);
        }
    }

    // Handle GROUP BY and HAVING clauses if present
    if (!selectStmt->groupByColumns.empty()) {
        // Implement GROUP BY logic with aggregations
        // For simplicity, this implementation does not handle aggregations
        logLogger.warn("GROUP BY clauses are not yet fully supported in PROC SQL.");
    }

    // Handle ORDER BY clause if present
    if (!selectStmt->orderByColumns.empty()) {
        // Implement ORDER BY logic
        // For simplicity, sort by the first column specified
        std::string sortColumn = selectStmt->orderByColumns[0];
        std::sort(resultDS->rows.begin(), resultDS->rows.end(),
            [&](const Row& a, const Row& b) -> bool {
                auto itA = a.columns.find(sortColumn);
                auto itB = b.columns.find(sortColumn);
                if (itA != a.columns.end() && itB != b.columns.end()) {
                    if (std::holds_alternative<double>(itA->second) &&
                        std::holds_alternative<double>(itB->second)) {
                        return std::get<double>(itA->second) < std::get<double>(itB->second);
                    }
                    else if (std::holds_alternative<std::string>(itA->second) &&
                        std::holds_alternative<std::string>(itB->second)) {
                        return std::get<std::string>(itA->second) < std::get<std::string>(itB->second);
                    }
                }
                return false;
            });
    }

    // Log the results
    std::stringstream ss;
    ss << "PROC SQL SELECT Results (Dataset: " << resultDS->name << "):\n";
    if (resultDS->rows.empty()) {
        ss << "No records found.\n";
    }
    else {
        // Print header
        ss << "OBS\t";
        size_t colCount = 0;
        for (const auto& col : selectStmt->selectColumns) {
            ss << col;
            if (colCount != selectStmt->selectColumns.size() - 1) {
                ss << "\t";
            }
            colCount++;
        }
        ss << "\n";

        // Print rows
        for (size_t i = 0; i < resultDS->rows.size(); ++i) {
            ss << (i + 1) << "\t";
            const Row& row = resultDS->rows[i];
            for (size_t j = 0; j < selectStmt->selectColumns.size(); ++j) {
                const std::string& col = selectStmt->selectColumns[j];
                auto it = row.columns.find(col);
                if (it != row.columns.end()) {
                    if (std::holds_alternative<double>(it->second)) {
                        ss << std::fixed << std::setprecision(2) << std::get<double>(it->second);
                    }
                    else if (std::holds_alternative<std::string>(it->second)) {
                        ss << std::get<std::string>(it->second);
                    }
                    // Handle other data types as needed
                }
                else {
                    ss << "NA";
                }

                if (j != selectStmt->selectColumns.size() - 1) {
                    ss << "\t";
                }
            }
            ss << "\n";
        }
    }

    logLogger.info(ss.str());

    return resultDS;
}

void Interpreter::executeCreateTable(const CreateTableStatementNode* createStmt) {
    // Create a new dataset with the specified columns
    DatasetRefNode dsNode;
    dsNode.dataName = createStmt->tableName;
    Dataset* newDS = env.getOrCreateDataset(dsNode).get();
    newDS->rows.clear();

    // For simplicity, initialize columns without specific data types
    for (const auto& col : createStmt->columns) {
        // todo newDS->columns[col] = Value(); // Initialize with default values
    }

    logLogger.info("PROC SQL: Created table '{}'.", dsNode.getFullDsName());
}

// Implement other SQL statement executors (INSERT, UPDATE, DELETE) as needed

std::string Interpreter::resolveMacroVariables(const std::string& input) {
    std::string result = input;
    size_t startPos = 0;

    while ((startPos = result.find('&', startPos)) != std::string::npos) {
        size_t endPos = startPos + 1;
        while (endPos < result.size() && (isalnum(result[endPos]) || result[endPos] == '_')) {
            ++endPos;
        }

        std::string varName = result.substr(startPos + 1, endPos - startPos - 1);

        auto it = macroVariables.find(varName);
        if (it != macroVariables.end()) {
            logLogger.debug("Resolving macro variable '&{}' to '{}'", varName, it->second);
            result.replace(startPos, endPos - startPos, it->second);
        }
        else {
            throw std::runtime_error("Unresolved macro variable: " + varName);
        }
    }

    return result;
}


void Interpreter::executeMacroVariableAssignment(MacroVariableAssignmentNode* node) {
    macroVariables[node->varName] = resolveMacroVariables(node->value);
    logLogger.info("Macro variable '{}' set to '{}'", node->varName, macroVariables[node->varName]);
}

void Interpreter::executeMacroDefinition(std::unique_ptr<MacroDefinitionNode> node) {
    if (macros.find(node->macroName) != macros.end()) {
        throw std::runtime_error("Macro '" + node->macroName + "' is already defined.");
    }
    macros[node->macroName] = std::move(node);
    logLogger.info("Macro '{}' defined.", macros[node->macroName]->macroName);
}

void Interpreter::executeMacroCall(MacroCallNode* node) {
    auto it = macros.find(node->macroName);
    if (it == macros.end()) {
        throw std::runtime_error("Undefined macro: " + node->macroName);
    }

    MacroDefinitionNode* macro = it->second.get();

    // Map arguments to parameters
    if (node->arguments.size() != macro->parameters.size()) {
        throw std::runtime_error("Macro '" + macro->macroName + "' expects " +
            std::to_string(macro->parameters.size()) +
            " arguments, but got " + std::to_string(node->arguments.size()));
    }

    std::unordered_map<std::string, std::string> localVariables;
    for (size_t i = 0; i < macro->parameters.size(); ++i) {
        localVariables[macro->parameters[i]] = resolveMacroVariables(
            dynamic_cast<StringNode*>(node->arguments[i].get())->value);
    }

    // Temporarily override macro variables
    auto backup = macroVariables;
    for (const auto& pair : localVariables) {
        macroVariables[pair.first] = pair.second;
    }

    // Execute macro body
    for (const auto& stmt : macro->body) {
        execute(stmt.get());
    }

    // Restore original macro variables
    macroVariables = backup;

    logLogger.info("Macro '{}' executed successfully.", macro->macroName);
}

void Interpreter::reset() {
    // Clear macros
    macros.clear();

    // Clear macro variables
    macroVariables.clear();

    // Clear arrays
    arrays.clear();

    // Reset other interpreter state as needed
    logLogger.info("Interpreter state has been reset.");
}

void Interpreter::handleReplInput(const std::string& input) {
    // Tokenize the input
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    // Parse the tokens into AST
    Parser parser(tokens);
    ParseResult parseResult;
    try {
        parseResult = parser.parseStatement();
    }
    catch (const std::runtime_error& e) {
        logLogger.error("Parsing error: {}", e.what());
        return;
    }

    // Execute the AST
    try {
        if (parseResult.status == ParseStatus::PARSE_SUCCESS) {
            execute(parseResult.node.get());
        }
    }
    catch (const std::runtime_error& e) {
        logLogger.error("Execution error: {}", e.what());
    }
}

}

