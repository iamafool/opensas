#include "Interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

// Execute the entire program
void Interpreter::executeProgram(const std::unique_ptr<ProgramNode> &program) {
    for (const auto &stmt : program->statements) {
        try {
            execute(stmt.get());
        }
        catch (const std::runtime_error& e) {
            logLogger.error("Execution error: {}", e.what());
            // Continue with the next statement
        }
    }
}

// Execute a single AST node
void Interpreter::execute(ASTNode *node) {
    if (auto ds = dynamic_cast<DataStepNode*>(node)) {
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
    else {
        // Handle other statements
        // For now, ignore unknown statements or throw an error
        throw std::runtime_error("Unknown AST node encountered during execution.");
    }
}

// Execute a DATA step
void Interpreter::executeDataStep(DataStepNode *node) {
    logLogger.info("Executing DATA step: data {}; set {};", node->outputDataSet, node->inputDataSet);

    // Resolve output dataset name
    std::string outputLibref, outputDataset;
    size_t dotPos = node->outputDataSet.find('.');
    if (dotPos != std::string::npos) {
        outputLibref = node->outputDataSet.substr(0, dotPos);
        outputDataset = node->outputDataSet.substr(dotPos + 1);
    }
    else {
        outputDataset = node->outputDataSet;
    }

    // Resolve input dataset name
    std::string inputLibref, inputDataset;
    dotPos = node->inputDataSet.find('.');
    if (dotPos != std::string::npos) {
        inputLibref = node->inputDataSet.substr(0, dotPos);
        inputDataset = node->inputDataSet.substr(dotPos + 1);
    }
    else {
        inputDataset = node->inputDataSet;
    }

    // Check if input dataset exists
    std::shared_ptr<DataSet> input = nullptr;
    try {
        input = env.getOrCreateDataset(inputLibref, inputDataset);
    }
    catch (const std::runtime_error& e) {
        logLogger.error(e.what());
        return;
    }

    // Create or get the output dataset
    std::shared_ptr<DataSet> output;
    try {
        output = env.getOrCreateDataset(outputLibref, outputDataset);
        output->name = node->outputDataSet;
    }
    catch (const std::runtime_error &e) {
        logLogger.error(e.what());
        return;
    }

    // Log dataset sizes
    logLogger.info("Input dataset '{}' has {} observations.", node->inputDataSet, input->rows.size());
    logLogger.info("Output dataset '{}' will store results.", node->outputDataSet);

    // Execute each row in the input dataset
    for (const auto &row : input->rows) {
        env.currentRow = row; // Set the current row for processing

        // Flag to determine if the row should be output
        bool shouldOutput = false;

        // Execute each statement in the DATA step
        for (const auto& stmt : node->statements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt.get())) {
                executeIfThen(ifThen);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
                shouldOutput = true;
            }
            else {
                // Handle other DATA step statements if needed
                throw std::runtime_error("Unsupported statement in DATA step.");
            }
        }

        // If 'OUTPUT' was called, add the current row to the output dataset
        if (shouldOutput) {
            output->addRow(env.currentRow);
            logLogger.info("Row outputted to '{}'.", node->outputDataSet);
        }
    }

    logLogger.info("DATA step '{}' executed successfully. Output dataset has {} observations.",
                   node->outputDataSet, output->rows.size());

    // For demonstration, print the output dataset
    lstLogger.info("SAS Results (Dataset: {}):", node->outputDataSet);
    if (!env.title.empty()) {
        lstLogger.info("Title: {}", env.title);
    }
    lstLogger.info("OBS\tX");
    int obs = 1;
    for (const auto &row : output->rows) {
        lstLogger.info("{}\t{}", obs++, toString(row.columns.at("x")));
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
    env.setLibref(node->libref, node->path);
    logLogger.info("Libname assigned: {} = '{}'", node->libref, node->path);

    // Optionally, load existing datasets from the path
    // For demonstration, let's assume the path contains CSV files named as datasets
    // e.g., 'in.csv' corresponds to 'libref.in'

    // Example: Load 'in.csv' as 'mylib.in'
    std::string csvPath = node->path + "\\" + "in.csv"; // Adjust path separator as needed
    try {
        env.loadDatasetFromCSV(node->libref, "in", csvPath);
        logLogger.info("Loaded dataset '{}' from '{}'", node->libref + ".in", csvPath);
    }
    catch (const std::runtime_error &e) {
        logLogger.error("Failed to load dataset: {}", e.what());
    }
}

// Execute a TITLE statement
void Interpreter::executeTitle(TitleNode* node) {
    env.setTitle(node->title);
    logLogger.info("Title set to: '{}'", node->title);
    lstLogger.info("Title: {}", env.title);
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
    if (auto lit = dynamic_cast<LiteralNode*>(node)) {
        // Try to parse as number
        try {
            double d = std::stod(lit->value);
            return d;
        }
        catch (...) {
            return lit->value;
        }
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
        Value argVal = evaluate(funcCall->argument.get());
        double argNum = toNumber(argVal);
        if (funcCall->funcName == "sqrt") {
            return std::sqrt(argNum);
        }
        else if (funcCall->funcName == "abs") {
            return std::abs(argNum);
        }
        else {
            throw std::runtime_error("Unsupported function: " + funcCall->funcName);
        }
    }
    else if (auto bin = dynamic_cast<BinaryOpNode*>(node)) {
        Value leftVal = evaluate(bin->left.get());
        Value rightVal = evaluate(bin->right.get());
        double l = toNumber(leftVal);
        double r = toNumber(rightVal);
        std::string op = bin->op;

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
        // Implement logical operators (and, or) as needed
        else {
            throw std::runtime_error("Unsupported binary operator: " + op);
        }
    }
    // Handle more expression types as needed
    throw std::runtime_error("Unsupported expression type during evaluation.");
}

void Interpreter::executeProc(ProcNode* node) {
    if (node->procName == "print") {
        logLogger.info("Executing PROC PRINT on dataset '{}'.", node->datasetName);
        try {
            auto dataset = env.getOrCreateDataset("", node->datasetName);
            lstLogger.info("PROC PRINT Results for Dataset '{}':", dataset->name);
            if (!env.title.empty()) {
                lstLogger.info("Title: {}", env.title);
            }

            // Print column headers
            std::string header;
            for (size_t i = 0; i < dataset->columnOrder.size(); ++i) {
                header += dataset->columnOrder[i];
                if (i < dataset->columnOrder.size() - 1) header += "\t";
            }
            lstLogger.info("{}", header);

            // Print rows
            int obs = 1;
            for (const auto& row : dataset->rows) {
                std::string rowStr = std::to_string(obs++) + "\t";
                for (size_t i = 0; i < dataset->columnOrder.size(); ++i) {
                    const std::string& col = dataset->columnOrder[i];
                    auto it = row.columns.find(col);
                    if (it != row.columns.end()) {
                        rowStr += toString(it->second);
                    }
                    else {
                        rowStr += ".";
                    }
                    if (i < dataset->columnOrder.size() - 1) rowStr += "\t";
                }
                lstLogger.info("{}", rowStr);
            }
        }
        catch (const std::runtime_error& e) {
            logLogger.error("PROC PRINT failed: {}", e.what());
        }
    }
    else {
        logLogger.error("Unsupported PROC: {}", node->procName);
    }
}
