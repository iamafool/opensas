#include "Lexer.h"
#include <cctype>
#include <algorithm>

Lexer::Lexer(const std::string &in) : input(in) {}

char Lexer::peekChar() const {
    if (pos >= input.size()) return '\0';
    return input[pos];
}

char Lexer::getChar() {
    if (pos >= input.size()) return '\0';
    char c = input[pos++];
    if (c == '\n') { line++; col = 1; }
    else { col++; }
    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(static_cast<unsigned char>(peekChar()))) {
        getChar();
    }
}

Token Lexer::identifierOrKeyword() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::IDENTIFIER;

    std::string value;
    while (std::isalnum(static_cast<unsigned char>(peekChar())) || peekChar() == '_') {
        value += getChar();
    }

    // Convert to uppercase for case-insensitive matching (SAS is case-insensitive)
    std::string upperValue = value;
    std::transform(upperValue.begin(), upperValue.end(), upperValue.begin(), ::toupper);

    // Keyword detection
    if (upperValue == "DATA") token.type = TokenType::KEYWORD_DATA;
    else if (upperValue == "SET") token.type = TokenType::KEYWORD_SET;
    else if (upperValue == "IF") token.type = TokenType::KEYWORD_IF;
    else if (upperValue == "THEN") token.type = TokenType::KEYWORD_THEN;
    else if (upperValue == "OUTPUT") token.type = TokenType::KEYWORD_OUTPUT;
    else if (upperValue == "RUN") token.type = TokenType::KEYWORD_RUN;
    // New global statement keywords
    else if (upperValue == "OPTIONS") token.type = TokenType::KEYWORD_OPTIONS;
    else if (upperValue == "LIBNAME") token.type = TokenType::KEYWORD_LIBNAME;
    else if (upperValue == "TITLE") token.type = TokenType::KEYWORD_TITLE;

    token.text = value;

    return token;
}

Token Lexer::number() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::NUMBER;

    std::string value;
    while (std::isdigit(static_cast<unsigned char>(peekChar())) || peekChar() == '.') {
        value += getChar();
    }
    token.text = value;

    return token;
}

Token Lexer::stringLiteral() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::STRING;
    getChar(); // skip the opening quote
    std::string value;
    while (peekChar() != '\0' && peekChar() != '\'') {
        value += getChar();
    }
    getChar(); // skip the closing quote
    token.text = value;
    return token;
}

Token Lexer::getNextToken() {
    skipWhitespace();
    Token token;
    token.line = line;
    token.col = col;
    char c = peekChar();
    if (c == '\0') {
        token.type = TokenType::EOF_TOKEN;
        return token;
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        return identifierOrKeyword();
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        return number();
    }

    switch (c) {
        case '=': getChar(); token.type = TokenType::EQUALS; token.text = "="; return token;
        case ';': getChar(); token.type = TokenType::SEMICOLON; token.text = ";"; return token;
        case '(': getChar(); token.type = TokenType::LPAREN; token.text = "("; return token;
        case ')': getChar(); token.type = TokenType::RPAREN; token.text = ")"; return token;
        case ',': getChar(); token.type = TokenType::COMMA; token.text = ","; return token;
        case '\'': return stringLiteral();
        case '.': getChar(); token.type = TokenType::DOT; token.text = "."; return token;
        case '+': getChar(); token.type = TokenType::PLUS; token.text = "+"; return token;
        case '-': getChar(); token.type = TokenType::MINUS; token.text = "-"; return token;
        case '*': getChar(); token.type = TokenType::MUL; token.text = "*"; return token;
        case '/': getChar(); token.type = TokenType::DIV; token.text = "/"; return token;
        default:
            getChar();
            token.type = TokenType::UNKNOWN;
            token.text = std::string(1, c);
            return token;
    }
}
