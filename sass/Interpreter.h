#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "DataEnvironment.h"
#include <memory>
#include <spdlog/spdlog.h>

class Interpreter {
public:
    Interpreter(DataEnvironment &env, spdlog::logger &logLogger, spdlog::logger &lstLogger)
        : env(env), logLogger(logLogger), lstLogger(lstLogger) {}

    void executeProgram(const std::unique_ptr<ProgramNode> &program);
    spdlog::logger &logLogger;

private:
    DataEnvironment &env;
    spdlog::logger &lstLogger;
    // Add a member variable to hold arrays
    std::unordered_map<std::string, std::vector<std::string>> arrays;
    std::vector<std::string> retainVars;


    void execute(ASTNode *node);
    void executeDataStep(DataStepNode *node);
    void executeAssignment(AssignmentNode *node);
    void executeIfThen(IfThenNode *node);
    void executeIfElse(IfElseIfNode* node); // Updated method
    void executeOutput(OutputNode *node);
    void executeOptions(OptionsNode *node);
    void executeLibname(LibnameNode *node);
    void executeTitle(TitleNode *node);
    void executeProc(ProcNode* node);
    void executeDrop(DropNode* node);
    void executeKeep(KeepNode* node);
    void executeRetain(RetainNode* node);
    void executeArray(ArrayNode* node);
    void executeDo(DoNode* node);
    void executeProcSort(ProcSortNode* node);
    void executeProcMeans(ProcMeansNode* node);
    void executeBlock(BlockNode* node);

    double toNumber(const Value &v);
    std::string toString(const Value &v);

    Value getVariable(const std::string& varName) const;

    void setVariable(const std::string& varName, const Value& val);

    Value evaluate(ASTNode *node);
    Value evaluateFunctionCall(FunctionCallNode* node); // New method
};

#endif // INTERPRETER_H
