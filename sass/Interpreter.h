#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "DataEnvironment.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>

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

    // Current BY variables and their order
    std::vector<std::string> byVariables;

    // Stack to manage loop contexts
    std::stack<std::pair<DoLoopNode*, size_t>> loopStack;

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
    void executeProcFreq(ProcFreqNode* node);
    void executeProcPrint(ProcPrintNode* node);
    void executeProcSQL(ProcSQLNode* node);
    void executeBlock(BlockNode* node);
    void executeMerge(MergeStatementNode* node);
    void executeBy(ByStatementNode* node);
    void executeDoLoop(DoLoopNode* node);
    void executeEnd(EndNode* node);

    double toNumber(const Value &v);
    std::string toString(const Value &v);

    Value getVariable(const std::string& varName) const;

    void setVariable(const std::string& varName, const Value& val);

    Value evaluate(ASTNode *node);
    Value evaluateFunctionCall(FunctionCallNode* node);

    // Helper methods for array operations
    Value getArrayElement(const std::string& arrayName, int index);
    void setArrayElement(const std::string& arrayName, int index, const Value& value);

    // SQL execution helpers
    Dataset* executeSelect(const SelectStatementNode* selectStmt);
    void executeCreateTable(const CreateTableStatementNode* createStmt);
    // Implement other SQL statement executors (INSERT, UPDATE, DELETE) as needed
};

#endif // INTERPRETER_H
