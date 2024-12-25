#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "Token.h"
#include <unordered_map>

namespace sass {
    class Lexer {
    public:
        Lexer(const std::string& input);
        Token getNextToken();
        std::vector<Token> tokenize();

    private:
        std::string input;
        size_t pos = 0;
        int line = 1;
        int col = 1;

        char peekChar() const;
        char getChar();
        void skipWhitespace();
        Token number();
        Token stringLiteral();
        Token identifierOrKeyword();
        Token macroToken(); // For %macro, %let, etc.
        Token macroVariable(); // For &varname or &&varname
        bool inDatalinesMode = false;  // <-- We'll set this to true after we see 'datalines;'

        // Because we only enter "inDatalinesMode" after reading `datalines;`,
        // we also track if we've *just* seen the 'KEYWORD_DATALINES' token
        // to see if next token is SEMICOLON => then switch mode. 
        bool justSawDatalinesKeyword = false;

        // read raw lines until a line that is just ';'
        Token readDatalinesContent();

        std::unordered_map<std::string, TokenType> keywords;
    };

}
#endif // LEXER_H
