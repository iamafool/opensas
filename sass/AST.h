#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() {}
};

// Represents a DATA step block: data <dataset>; set <source>; <statements>; run;
class DataStepNode : public ASTNode {
public:
    std::string outputDataSet; // e.g., mylib.out
    std::string inputDataSet;  // e.g., mylib.in
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// Represents a variable assignment: var = expression;
class AssignmentNode : public ASTNode {
public:
    std::string varName;
    std::unique_ptr<ASTNode> expression;
};

// Represents a literal number or string
class LiteralNode : public ASTNode {
public:
    std::string value;  // Could differentiate numeric vs. string later
};

class NumberNode : public ASTNode {
public:
    double value;

    // Constructor to initialize value
    explicit NumberNode(double val) : value(val) {}
};


class StringNode : public ASTNode {
public:
    std::string value;

    // Constructor to initialize value
    explicit StringNode(std::string val) : value(val) {}
};


// Represents a variable reference
class VariableNode : public ASTNode {
public:
    std::string varName;

    // Constructor to initialize value
    explicit VariableNode(std::string varName) : varName(varName) {}
};

// Represents a binary operation: expr op expr
class BinaryOpNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op; // e.g., '+', '-', '*', '/', '>', '<', '>=', '<=', '==', '!=', 'and', 'or'
};

// Represents an IF-THEN statement: if <condition> then <statements>;
class IfThenNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenStatements;
};

// Represents an OUTPUT statement
class OutputNode : public ASTNode {};

// Represents an OPTIONS statement: options option1=value1 option2=value2;
class OptionsNode : public ASTNode {
public:
    std::vector<std::pair<std::string, std::string>> options;
};

// Represents a LIBNAME statement: libname libref 'path';
class LibnameNode : public ASTNode {
public:
    std::string libref;
    std::string path;
};

// Represents a TITLE statement: title 'Your Title';
class TitleNode : public ASTNode {
public:
    std::string title;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// Represents a function call: function_name(arg1, arg2, ...)
class FunctionCallNode : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;
};

class ProcNode : public ASTNode {
public:
    std::string procName;
    std::string datasetName; // e.g., proc print data=mylib.out;
    // Add more fields as needed
};

// Represents a DROP statement: drop var1 var2 ...;
class DropNode : public ASTNode {
public:
    std::vector<std::string> variables;
};

// Represents a KEEP statement: keep var1 var2 ...;
class KeepNode : public ASTNode {
public:
    std::vector<std::string> variables;
};

// Represents a RETAIN statement: retain var1 var2 ...;
class RetainNode : public ASTNode {
public:
    std::vector<std::string> variables;
};


// Represents an ARRAY statement: array arr {size} var1 var2 ...;
class ArrayNode : public ASTNode {
public:
    std::string arrayName;
    int size;
    std::vector<std::string> variables;
};

// Represents an array element reference: arrayName[index]
class ArrayElementNode : public ASTNode {
public:
    std::string arrayName;
    std::unique_ptr<ASTNode> index;
};

// Represents a DO loop: do <variable> = <start> to <end> by <increment>;
class DoNode : public ASTNode {
public:
    std::string loopVar;
    std::unique_ptr<ASTNode> startExpr;
    std::unique_ptr<ASTNode> endExpr;
    std::unique_ptr<ASTNode> incrementExpr; // Optional
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// Represents an ENDDO statement
class EndDoNode : public ASTNode {};

// Represents a PROC SORT step: proc sort data=<dataset>; by <variables>; run;
class ProcSortNode : public ASTNode {
public:
    std::string datasetName;
    std::vector<std::string> byVariables;
};

// Represents a PROC MEANS step: proc means data=<dataset>; var <variables>; run;
class ProcMeansNode : public ASTNode {
public:
    std::string datasetName;
    std::vector<std::string> varNames;
};

// Represents an IF-ELSE statement: if <condition> then <statements> else <statements>;
class IfElseNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenStatements;
    std::vector<std::unique_ptr<ASTNode>> elseStatements;
};

// Represents an IF-ELSE IF-ELSE statement
class IfElseIfNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenStatements;
    // List of ELSE IF branches
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::vector<std::unique_ptr<ASTNode>>>> elseIfBranches;
    // ELSE branch
    std::vector<std::unique_ptr<ASTNode>> elseStatements;
};

// Represents a block of statements: do ... end;
class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// Represents a BY statement: by var1 var2 ...;
class ByStatementNode : public ASTNode {
public:
    std::vector<std::string> variables;
};

// Represents a MERGE statement: merge dataset1 dataset2 ...;
class MergeStatementNode : public ASTNode {
public:
    std::vector<std::string> datasets;
};

#endif // AST_H
