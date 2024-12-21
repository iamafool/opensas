#include "Interpreter.h"
#include <iostream>
#include <stdexcept>

void Interpreter::execute(std::unique_ptr<ASTNode>& node) {
    if (auto ds = dynamic_cast<DataStepNode*>(node.get())) {
        executeDataStep(ds);
    }
    else {
        // Not handling other top-level constructs
    }
}

void Interpreter::executeDataStep(DataStepNode* node) {
    // For each row in inputDataSet, run statements, possibly output rows
    auto it = env.dataSets.find(node->inputDataSet);
    if (it == env.dataSets.end()) {
        std::cerr << "Input dataset not found: " << node->inputDataSet << "\n";
        return;
    }

    DataSet& input = it->second;
    DataSet& output = env.dataSets[node->outputDataSet]; // creates if not present

    for (auto& row : input.rows) {
        env.currentRow = row;
        bool outputThisRow = false;

        for (auto& stmt : node->statements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto ifThen = dynamic_cast<IfThenNode*>(stmt.get())) {
                executeIfThen(ifThen);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
                outputThisRow = true;
            }
        }

        if (outputThisRow) {
            output.rows.push_back(env.currentRow);
        }
    }

    // After run, maybe print the output for demonstration
    std::cout << "Output dataset: " << node->outputDataSet << "\n";
    for (auto& r : env.dataSets[node->outputDataSet].rows) {
        for (auto& col : r.columns) {
            if (std::holds_alternative<double>(col.second)) {
                std::cout << col.first << "=" << std::get<double>(col.second) << " ";
            }
            else {
                std::cout << col.first << "='" << std::get<std::string>(col.second) << "' ";
            }
        }
        std::cout << "\n";
    }
}

void Interpreter::executeAssignment(AssignmentNode* node) {
    Value val = evaluate(node->expression.get());
    env.setValue(node->varName, val);
}

void Interpreter::executeIfThen(IfThenNode* node) {
    Value cond = evaluate(node->condition.get());
    double d = toNumber(cond);
    if (d != 0.0) { // treat nonzero as true
        for (auto& stmt : node->thenStatements) {
            if (auto assign = dynamic_cast<AssignmentNode*>(stmt.get())) {
                executeAssignment(assign);
            }
            else if (auto out = dynamic_cast<OutputNode*>(stmt.get())) {
                executeOutput(out);
            }
        }
    }
}

void Interpreter::executeOutput(OutputNode* node) {
    // Mark that we output this row
    // Actually done in executeDataStep by setting a flag
}

double Interpreter::toNumber(const Value& v) {
    if (std::holds_alternative<double>(v)) {
        return std::get<double>(v);
    }
    else if (std::holds_alternative<std::string>(v)) {
        try {
            return std::stod(std::get<std::string>(v));
        }
        catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

std::string Interpreter::toString(const Value& v) {
    if (std::holds_alternative<double>(v)) {
        return std::to_string(std::get<double>(v));
    }
    else {
        return std::get<std::string>(v);
    }
}

Value Interpreter::evaluate(ASTNode* node) {
    if (auto lit = dynamic_cast<LiteralNode*>(node)) {
        // try number conversion first
        try {
            double d = std::stod(lit->value);
            return d;
        }
        catch (...) {
            return lit->value;
        }
    }
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        return env.getValue(var->varName);
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
        case '/': return (r != 0.0) ? l / r : 0.0;
        }
        return 0.0;
    }
    // If unknown node, return 0
    return 0.0;
}
