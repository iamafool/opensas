#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace sass {
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
        KEYWORD_OPTIONS,
        KEYWORD_LIBNAME,
        KEYWORD_TITLE,
        KEYWORD_PROC,

        EQUALS,
        SEMICOLON,
        LPAREN,
        RPAREN,
        COMMA,
        QUOTE,
        DOT,
        GREATER,
        LESS,

        PLUS,
        MINUS,
        MUL,
        DIV,
        EOF_TOKEN,
        UNKNOWN
    };

}

using sass::TokenType;

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
};

#endif // TOKEN_H
