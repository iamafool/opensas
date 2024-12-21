#include <iostream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "DataEnvironment.h"

int main() {
    // Example SAS-like code:
    // data out; set in;
    // x = 42;
    // if x then output;
    // run;

    std::string code = "data out; set in; x = 42; if x then output; run;";

    // Prepare environment
    DataEnvironment env;
    // Create a sample input dataset
    DataSet inData;
    inData.rows.push_back({ {{"x", 0.0}} });
    inData.rows.push_back({ {{"x", 1.0}} });
    env.dataSets["in"] = inData;

    // Lex
    Lexer lexer(code);
    std::vector<Token> tokens;
    Token tok;
    while ((tok = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
    }

    // Parse
    Parser parser(tokens);
    std::unique_ptr<ASTNode> root = parser.parse();

    // Interpret
    Interpreter interpreter(env);
    interpreter.execute(root);

    return 0;
}
