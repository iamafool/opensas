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
        KEYWORD_DROP,
        KEYWORD_KEEP,
        KEYWORD_RETAIN,
        KEYWORD_ARRAY,
        KEYWORD_DO,
        KEYWORD_ENDDO,
        KEYWORD_TO,
        KEYWORD_BY,
        KEYWORD_VAR,

        EQUALS,
        SEMICOLON,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        COMMA,
        QUOTE,
        DOT,
        GREATER,
        LESS,
        GREATER_EQUAL,
        LESS_EQUAL,
        EQUAL_EQUAL,
        NOT_EQUAL,
        NOT,
        AND,
        OR,

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
