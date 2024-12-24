#include "Repl.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "Lexer.h"
#include "Parser.h"

namespace sass {

    Repl::Repl(Interpreter& interp) : interpreter(interp) {}

    void Repl::run() {
        std::cout << "SAS Interpreter REPL\nType 'exit' to quit.\n\n";
        std::string line;

        while (true) {
            // Display prompt
            std::cout << "> ";
            if (!std::getline(std::cin, line)) {
                // End-of-file or error
                break;
            }

            // Trim whitespace
            // (simple approach; you can refine)
            auto trimPos = line.find_first_not_of(" \t\r\n");
            if (trimPos != std::string::npos) {
                line.erase(0, trimPos);
            }
            trimPos = line.find_last_not_of(" \t\r\n");
            if (trimPos != std::string::npos) {
                line.erase(trimPos + 1);
            }

            // Check for exit command
            if (line == "exit") {
                std::cout << "Goodbye!\n";
                break;
            }

            if (line == "help" || line == "?") {
                std::cout << "Supported Commands:\n";
                std::cout << "  options ... ;       - Set global options\n";
                std::cout << "  libname ... ;        - Assign a library reference\n";
                std::cout << "  title '...' ;        - Set the title for outputs\n";
                std::cout << "  data ... ; run;      - Define and execute a data step\n";
                std::cout << "  proc print data=...; - Print a dataset\n";
                std::cout << "  proc sort data=...; by var1 var2; run; - Sort a dataset\n";
                std::cout << "  proc means data=...; var var1 var2; run; - Get means of variables\n";
                std::cout << "  quit / exit          - Exit the interpreter\n";
                continue;
            }

            // Accumulate the line into inputBuffer
            inputBuffer += line + "\n";

            // Now try to parse the entire buffer
            // 1) Lex
            Lexer lexer(inputBuffer);
            std::vector<Token> tokens;
            Token tok;
            while ((tok = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
                tokens.push_back(tok);
            }

            // 2) Parse
            Parser parser(tokens);
            ParseResult result = parser.parseStatement();

            if (result.status == ParseStatus::PARSE_SUCCESS) {
                // We have a complete AST
                interpreter.execute(result.node.get());

                // Since we consumed the entire buffer for a statement, 
                // we can clear it or see if there's leftover tokens
                // For simplicity, let's just clear
                inputBuffer.clear();
            }
            else if (result.status == ParseStatus::PARSE_INCOMPLETE) {
                // Keep reading lines until we have a complete statement
                continue;
                // Do nothing else
            }
            else { // ERROR
                std::cout << "[REPL] Parse error: " << result.errorMessage << "\n";
                // Usually we reset the input so we can start fresh
                inputBuffer.clear();
            }

        }
    }

}
