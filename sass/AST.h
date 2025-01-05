#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include "Library.h"

namespace sass {
    // Base class for all AST nodes
    class ASTNode {
    public:
        virtual ~ASTNode() {}
    };

    // Expression nodes
    class ExpressionNode : public ASTNode {
    public:
        virtual ~ExpressionNode() = default;
    };

    // dataset
    class DatasetRefNode : public ASTNode {
    public:
        std::string libref;
        std::string dataName;

        std::string getFullDsName()
        {
            return (libref.empty() ? "WORK" : libref) + '.' + dataName;
        }
    };

    // e.g. dataStep code: data out; set in1 in2 ; ...
    class SetStatementNode : public ASTNode {
    public:
        // a list of dataset references
        std::vector<DatasetRefNode> dataSets;
        // Possibly, we also store optional modifiers like "obs=10" or "nobs=var"
        // For a minimal approach, just store the dataset names

        // Example:
        // data out;
        //   set a_in.dm b_in.test;
        // run;
        //
        // dataSets => { "a_in.dm", "b_in.test" }
    };

    // Represents a DATA step block: data <dataset>; set <source>; <statements>; run;
    class DataStepNode : public ASTNode {
    public:
        DatasetRefNode outputDataSet; // e.g., mylib.out
        DatasetRefNode inputDataSet;  // e.g., mylib.in
        std::vector<std::unique_ptr<ASTNode>> statements;
        std::vector<DatasetRefNode> inputDataSets;
        bool hasOutput;
    };

    // Represents a variable assignment: var = expression;
    class AssignmentNode : public ASTNode {
    public:
        std::unique_ptr<ASTNode> lhs;  // left-hand side, can be VariableNode or ArrayElementNode
        std::unique_ptr<ASTNode> expression; // right-hand side
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
    class OutputNode : public ASTNode {
    public:
        std::vector<DatasetRefNode> outDatasets;
    };

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
        LibraryAccess accessMode;
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
        // Each element could be (varName, optional<initialValue>)
        // Where initialValue could itself be a variant<double, std::string>.
        struct RetainElement {
            std::string varName;
            std::optional<Value> initialValue; // or variant
        };

        std::vector<RetainElement> elements;

        bool allFlag = false;       // if we saw _ALL_
        bool charFlag = false;      // if we saw _CHAR_
        bool numericFlag = false;   // if we saw _NUMERIC_

        // Possibly more for arrays
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
        DatasetRefNode inputDataSet;    // Dataset to sort (DATA=)
        DatasetRefNode outputDataSet;   // Output dataset (OUT=), can be empty
        std::vector<std::string> byVariables; // Variables to sort by
        std::unique_ptr<ASTNode> whereCondition; // Optional WHERE condition
        bool nodupkey;               // Flag for NODUPKEY option
        bool duplicates;             // Flag for DUPLICATES option
    };

    // Represents the PROC MEANS procedure
    class ProcMeansNode : public ASTNode {
    public:
        DatasetRefNode inputDataSet;                    // Dataset to analyze (DATA=)
        std::vector<std::string> statistics;         // Statistical options (N, MEAN, etc.)
        std::vector<std::string> varVariables;       // Variables to analyze (VAR statement)
        DatasetRefNode outputDataSet;                   // Output dataset (OUT=), can be empty
        std::unordered_map<std::string, std::string> outputOptions; // Output options like n=, mean=, etc.
        std::unique_ptr<ASTNode> whereCondition; // Optional WHERE 
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
        std::vector<DatasetRefNode> datasets;
    };

    // Represents a DO loop: do; ... end;
    class DoLoopNode : public ASTNode {
    public:
        std::unique_ptr<ASTNode> condition; // Can be nullptr for simple DO loops without conditions
        std::unique_ptr<BlockNode> body;    // The block of statements within the loop
        bool isWhile;                        // True for DO WHILE, False for DO UNTIL
    };

    // Represents an END statement: end;
    class EndNode : public ASTNode {
    public:
        // Can include labels or other attributes if needed
    };

    // Represents the PROC FREQ procedure
    class ProcFreqNode : public ASTNode {
    public:
        DatasetRefNode inputDataSet;                              // Dataset to analyze (DATA=)
        std::vector<std::pair<std::string, std::vector<std::string>>> tables; // Tables to generate, e.g., var1*var2
        std::unique_ptr<ASTNode> whereCondition;        // Optional WHERE condition
        std::vector<std::string> options;                      // Options for the TABLES statement
    };

    // Represents the PROC PRINT procedure
    class ProcPrintNode : public ProcNode {
    public:
        DatasetRefNode inputDataSet;                    // Dataset to print (DATA=)
        std::vector<std::string> varVariables;       // Variables to display (VAR statement), optional
        std::unordered_map<std::string, std::string> options; // Options like OBS=, NOOBS, LABEL, etc.
    };

    // Base class for SQL statements
    class SQLStatementNode : public ASTNode {};

    // Represents the PROC SQL procedure
    class ProcSQLNode : public ASTNode {
    public:
        std::vector<std::unique_ptr<SQLStatementNode>> statements; // SQL statements within PROC SQL
    };

    // Represents a SELECT statement
    class SelectStatementNode : public SQLStatementNode {
    public:
        std::vector<std::string> selectColumns; // Columns to select
        std::vector<std::string> fromTables;    // Tables to select from
        std::unique_ptr<ASTNode> whereCondition; // Optional WHERE condition
        std::vector<std::string> groupByColumns; // Optional GROUP BY columns
        std::unique_ptr<ASTNode> havingCondition; // Optional HAVING condition
        std::vector<std::string> orderByColumns; // Optional ORDER BY columns
    };

    // Represents a CREATE TABLE statement
    class CreateTableStatementNode : public SQLStatementNode {
    public:
        std::string tableName; // Name of the table to create
        std::vector<std::string> columns; // Columns and their definitions
    };

    // Additional SQL statement nodes (INSERT, UPDATE, DELETE) can be added similarly

    class MacroVariableAssignmentNode : public ASTNode {
    public:
        std::string varName;
        std::string value;
    };

    class MacroDefinitionNode : public ASTNode {
    public:
        std::string macroName;
        std::vector<std::string> parameters;
        std::vector<std::unique_ptr<ASTNode>> body;
    };

    class MacroCallNode : public ASTNode {
    public:
        std::string macroName;
        std::vector<std::unique_ptr<ASTNode>> arguments;
    };

    // input var1 var2 ...;
    class InputNode : public ASTNode {
    public:
        // For each declared variable: (varName, isString)
        std::vector<std::pair<std::string, bool>> variables;
    };

    // datalines; <lines> ;
    class DatalinesNode : public ASTNode {
    public:
        std::vector<std::string> lines;  // the raw lines
    };
}
#endif // AST_H
