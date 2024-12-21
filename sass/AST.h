#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() {}
};

// Represents a DATA step block: data <dataset>; set <source>; <statements>; run;
class DataStepNode : public ASTNode {
public:
    std::string outputDataSet;
    std::string inputDataSet;
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
    char op; // '+', '-', '*', '/'
};

// Represents an IF-THEN statement: if <condition> then <statements>;
class IfThenNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenStatements;
};

// Represents an OUTPUT statement
class OutputNode : public ASTNode {};

#endif // AST_H
