#include "Interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

// Execute the entire program
void Interpreter::executeProgram(const std::unique_ptr<ProgramNode> &program) {
    for (const auto &stmt : program->statements) {
        execute(stmt.get());
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
    else {
        // Handle other statements
        // For now, ignore unknown statements or throw an error
        throw std::runtime_error("Unknown AST node encountered during execution.");
    }
}

// Execute a DATA step
void Interpreter::executeDataStep(DataStepNode* node) {
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
    DataSet* input = nullptr;
    try {
        input = &env.getOrCreateDataset(inputLibref, inputDataset);
    }
    catch (const std::runtime_error& e) {
        logLogger.error(e.what());
        return;
    }

    // Create or get the output dataset
    DataSet& output = env.getOrCreateDataset(outputLibref, outputDataset);
    output.name = node->outputDataSet;

    for (const auto& row : input->rows) {
        env.currentRow = row; // Set the current row for processing

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
            }
            else {
                // Handle other DATA step statements if needed
                throw std::runtime_error("Unsupported statement in DATA step.");
            }
        }

        // After processing the row, add to output if 'output' was called
        // This logic can be adjusted based on how 'output' is implemented
        // For simplicity, assume 'output' flag is set by executeOutput
        // You can add a flag in DataEnvironment if needed

        // Placeholder: Add the row to output if 'output' was called
        // Implement flagging logic in executeOutput
    }

    logLogger.info("DATA step '{}' executed successfully. Output dataset has {} observations.",
        node->outputDataSet, output.rows.size());

    // Retrieve options
    std::string linesize = env.getOption("LINESIZE", "80"); // Default 80
    std::string pagesize = env.getOption("PAGESIZE", "60"); // Default 60

    lstLogger.info("LINESIZE = {}", linesize);
    lstLogger.info("PAGESIZE = {}", pagesize);

    // Convert to integers for formatting if needed
    int ls = std::stoi(linesize);
    int ps = std::stoi(pagesize);

    // Implement logic to format output based on linesize and pagesize
    // For simplicity, this is not fully implemented
    // You can adjust how titles, headers, and data rows are printed based on these settings

    lstLogger.info("SAS Results (Dataset: {}):", node->outputDataSet);
    if (!env.title.empty()) {
        lstLogger.info("Title: {}", env.title);
    }
    lstLogger.info("OBS\tX");
    int obs = 1;
    for (const auto& row : output.rows) {
        lstLogger.info("{}\t{}", obs++, toString(row.columns.at("x")));
        // Implement linesize and pagesize logic here
    }
}

// Execute an assignment statement
void Interpreter::executeAssignment(AssignmentNode *node) {
    Value val = evaluate(node->expression.get());
    env.setValue(node->varName, val);
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
    // For simplicity, assume 'output' adds the current row to the output dataset
    // Implement a flag or directly add the row in executeDataStep
    // Here, we log that 'output' was called
    logLogger.info("OUTPUT statement executed. Current row will be added to the output dataset.");
    
    // Implement the logic in executeDataStep to add the row
    // For demonstration, assume rows are added automatically
}

// Execute an OPTIONS statement
void Interpreter::executeOptions(OptionsNode* node) {
    for (const auto& opt : node->options) {
        env.setOption(opt.first, opt.second);
        logLogger.info("Set option {} = {}", opt.first, opt.second);

        // Optionally, handle specific options immediately
        if (opt.first == "LINESIZE") {
            // For example, store it in DataEnvironment and use it in output formatting
            // Implement usage in OutputManager if needed
        }
        else if (opt.first == "PAGESIZE") {
            // Similar handling for PAGSIZE
        }
        // Handle more options as needed
    }
}

// Execute a LIBNAME statement
void Interpreter::executeLibname(LibnameNode* node) {
    env.setLibref(node->libref, node->path);
    logLogger.info("Libname assigned: {} = '{}'", node->libref, node->path);

    // Optionally, verify that the path exists
    // For simplicity, not implemented here
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
        return std::to_string(std::get<double>(v));
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
        return env.getVariable(var->varName);
    }
    else if (auto bin = dynamic_cast<BinaryOpNode*>(node)) {
        Value leftVal = evaluate(bin->left.get());
        Value rightVal = evaluate(bin->right.get());
        double l = toNumber(leftVal);
        double r = toNumber(rightVal);
        switch (bin->op) {
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/': return (r != 0.0) ? l / r : std::nan("");
        }
    }
    // Handle more expression types as needed
    throw std::runtime_error("Unsupported expression type during evaluation.");
}
