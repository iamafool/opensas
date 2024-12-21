#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    STRING,
    KEYWORD_DATA,
    KEYWORD_SET,
    KEYWORD_IF,
    KEYWORD_THEN,
    KEYWORD_OUTPUT,
    KEYWORD_RUN,
    EQUALS,
    SEMICOLON,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    MUL,
    DIV,
    EOF_TOKEN,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
};

#endif // TOKEN_H
