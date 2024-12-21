#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& in) : input(in) {}

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
    while (std::isspace((unsigned char)peekChar())) getChar();
}

Token Lexer::identifierOrKeyword() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::IDENTIFIER;

    std::string value;
    while (std::isalnum((unsigned char)peekChar()) || peekChar() == '_') {
        value.push_back(getChar());
    }
    token.text = value;

    // Very simplistic keyword detection
    if (value == "data") token.type = TokenType::KEYWORD_DATA;
    else if (value == "set") token.type = TokenType::KEYWORD_SET;
    else if (value == "if") token.type = TokenType::KEYWORD_IF;
    else if (value == "then") token.type = TokenType::KEYWORD_THEN;
    else if (value == "output") token.type = TokenType::KEYWORD_OUTPUT;
    else if (value == "run") token.type = TokenType::KEYWORD_RUN;

    return token;
}

Token Lexer::number() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::NUMBER;

    std::string value;
    while (std::isdigit((unsigned char)peekChar()) || peekChar() == '.') {
        value.push_back(getChar());
    }
    token.text = value;

    return token;
}

Token Lexer::stringLiteral() {
    Token token;
    token.line = line;
    token.col = col;
    token.type = TokenType::STRING;
    getChar(); // skip the quote
    std::string value;
    while (peekChar() != '\0' && peekChar() != '\'') {
        value.push_back(getChar());
    }
    getChar(); // skip closing quote
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

    if (std::isalpha((unsigned char)c) || c == '_') {
        return identifierOrKeyword();
    }

    if (std::isdigit((unsigned char)c)) {
        return number();
    }

    switch (c) {
    case '=': getChar(); token.type = TokenType::EQUALS; token.text = "="; return token;
    case ';': getChar(); token.type = TokenType::SEMICOLON; token.text = ";"; return token;
    case '(': getChar(); token.type = TokenType::LPAREN; token.text = "("; return token;
    case ')': getChar(); token.type = TokenType::RPAREN; token.text = ")"; return token;
    case '+': getChar(); token.type = TokenType::PLUS; token.text = "+"; return token;
    case '-': getChar(); token.type = TokenType::MINUS; token.text = "-"; return token;
    case '*': getChar(); token.type = TokenType::MUL; token.text = "*"; return token;
    case '/': getChar(); token.type = TokenType::DIV; token.text = "/"; return token;
    case '\'': return stringLiteral();
    default:
        getChar();
        token.type = TokenType::UNKNOWN;
        token.text = std::string(1, c);
        return token;
    }
}
