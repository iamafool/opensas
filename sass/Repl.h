#ifndef REPL_H
#define REPL_H

#include "Interpreter.h"
#include <string>
namespace sass {
    class Repl {
    public:
        Repl(Interpreter& interpreter);
        void run(); // Starts the REPL loop

    private:
        Interpreter& interpreter;
        std::string inputBuffer;
    };

}
#endif // REPL_H
