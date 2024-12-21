#include <iostream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "Interpreter.h"
#include "DataEnvironment.h"

int main() {
    // A simple REPL
    DataEnvironment dataEnv;
    Interpreter interpreter(dataEnv);

    std::string line;
    while (true) {
        std::cout << "SAS> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit") break;

        // Lexing
        Lexer lexer(line);
        auto tokens = lexer.tokenize();

        // Parsing
        Parser parser(tokens);
        auto ast = parser.parse(); // returns an AST node representing the statement

        // Interpretation
        interpreter.execute(ast);

        // Possibly print results or logs
    }

    return 0;
}
