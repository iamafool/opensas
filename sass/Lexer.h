#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "Token.h"

class Lexer {
public:
    Lexer(const std::string& input);
    Token getNextToken();

private:
    std::string input;
    size_t pos = 0;
    int line = 1;
    int col = 1;

    char peekChar() const;
    char getChar();
    void skipWhitespace();
    Token identifierOrKeyword();
    Token number();
    Token stringLiteral();
};

#endif // LEXER_H