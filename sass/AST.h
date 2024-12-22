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

// Represents a variable reference
class VariableNode : public ASTNode {
public:
    std::string varName;
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

// Represents a function call: func(expr)
class FunctionCallNode : public ASTNode {
public:
    std::string funcName;
    std::unique_ptr<ASTNode> argument;
};

class ProcNode : public ASTNode {
public:
    std::string procName;
    std::string datasetName; // e.g., proc print data=mylib.out;
    // Add more fields as needed
};
#endif // AST_H
