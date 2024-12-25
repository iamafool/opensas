#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace sass {
    enum class TokenType {
        IDENTIFIER,
        DOLLAR,
        NUMBER,
        STRING,

        KEYWORD_DATA,
        KEYWORD_SET,
        KEYWORD_IF,
        KEYWORD_THEN,
        KEYWORD_ELSE,
        KEYWORD_ELSE_IF,
        KEYWORD_OUTPUT,
        KEYWORD_INPUT,
        KEYWORD_DATALINES,
        DATALINES_CONTENT,
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
        KEYWORD_MERGE,
        KEYWORD_DOLOOP,
        KEYWORD_WHILE,
        KEYWORD_UNTIL,
        KEYWORD_END,
        KEYWORD_SORT,
        KEYWORD_OUT,
        KEYWORD_WHERE,
        KEYWORD_NODUPKEY,
        KEYWORD_DUPLICATES,
        KEYWORD_MAX,
        KEYWORD_MEAN,
        KEYWORD_MEANS,
        KEYWORD_MEDIAN,
        KEYWORD_MIN,
        KEYWORD_N,
        KEYWORD_NOMISSING,
        KEYWORD_STD,
        KEYWORD_TABLES,
        KEYWORD_CHISQ,
        KEYWORD_NOCUM,
        KEYWORD_NOPRINT,
        KEYWORD_ORDER,
        KEYWORD_FREQ,
        KEYWORD_PRINT,
        KEYWORD_OBS,
        KEYWORD_NOOBS,
        KEYWORD_LABEL,
        KEYWORD_SQL,
        KEYWORD_SELECT,
        KEYWORD_FROM,
        KEYWORD_GROUP,
        KEYWORD_HAVING,
        KEYWORD_AS,
        KEYWORD_CREATE,
        KEYWORD_TABLE,
        KEYWORD_INSERT,
        KEYWORD_UPDATE,
        KEYWORD_DELETE,
        KEYWORD_QUIT,
        KEYWORD_INNER,
        KEYWORD_LEFT,
        KEYWORD_RIGHT,
        KEYWORD_FULL,
        KEYWORD_OUTER,
        KEYWORD_JOIN,
        KEYWORD_ON,
        KEYWORD_MACRO_LET,        // %let
        KEYWORD_MACRO_MACRO,      // %macro
        KEYWORD_MACRO_MEND,       // %mend
        KEYWORD_MACRO_DO,         // %do
        KEYWORD_MACRO_IF,         // %if
        KEYWORD_MACRO_THEN,       // %then
        KEYWORD_MACRO_ELSE,       // %else
        MACRO_VAR,          // &varname or &&varname
        EQUAL,
        SEMICOLON,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,
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
        STAR,
        PLUS,
        MINUS,
        MUL,
        DIV,
        EOF_TOKEN,
        SLASH,
        UNKNOWN
    };


using sass::TokenType;

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
};

}
#endif // TOKEN_H
