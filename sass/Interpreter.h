#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "DataEnvironment.h"
#include <memory>

class Interpreter {
public:
    Interpreter(DataEnvironment& env) : env(env) {}
    void execute(std::unique_ptr<ASTNode>& node);

private:
    DataEnvironment& env;

    void executeDataStep(DataStepNode* node);
    void executeAssignment(AssignmentNode* node);
    void executeIfThen(IfThenNode* node);
    void executeOutput(OutputNode* node);

    double toNumber(const Value& v);
    std::string toString(const Value& v);

    Value evaluate(ASTNode* node);
};

#endif // INTERPRETER_H
