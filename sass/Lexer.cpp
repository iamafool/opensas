#include "Lexer.h"
#include <cctype>
#include <algorithm>
#include <stdexcept>

Lexer::Lexer(const std::string &in) : input(in) {
    keywords["AND"] = TokenType::AND;
    keywords["OR"] = TokenType::OR;
    keywords["NOT"] = TokenType::NOT;
    keywords["ELSE"] = TokenType::KEYWORD_ELSE;
    keywords["ELSE IF"] = TokenType::KEYWORD_ELSE_IF;
    keywords["ARRAY"] = TokenType::KEYWORD_ARRAY;
    keywords["MERGE"] = TokenType::KEYWORD_MERGE;
    keywords["BY"] = TokenType::KEYWORD_BY;
    keywords["DO"] = TokenType::KEYWORD_DO;
    keywords["WHILE"] = TokenType::KEYWORD_WHILE;
    keywords["UNTIL"] = TokenType::KEYWORD_UNTIL;
    keywords["END"] = TokenType::KEYWORD_END;

}

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
    while (pos < input.size()) {
        char current = input[pos];

        // Skip whitespace
        if (isspace(current)) {
            if (current == '\n') {
                line++;
                col = 1;
            }
            else {
                col++;
            }
            pos++;
            continue;
        }

        // Handle comments
        if (current == '*') {
            // Skip until the end of the line
            while (pos < input.size() && input[pos] != '\n') pos++;
            continue;
        }

        // Handle multi-character operators
        if (current == '>' || current == '<' || current == '=' || current == '!') {
            if (pos + 1 < input.size()) {
                char next = input[pos + 1];
                if ((current == '>' || current == '<') && next == '=') {
                    pos += 2;
                    col += 2;
                    return Token{ current == '>' ? TokenType::GREATER_EQUAL : TokenType::LESS_EQUAL, std::string(1, current) + "=", line, col - 2 };
                }
                if (current == '=' && next == '=') {
                    pos += 2;
                    col += 2;
                    return Token{ TokenType::EQUAL_EQUAL, "==", line, col - 2 };
                }
                if (current == '!' && next == '=') {
                    pos += 2;
                    col += 2;
                    return Token{ TokenType::NOT_EQUAL, "!=", line, col - 2 };
                }
            }
            // Single '>' or '<' or '=' or '!' if not part of multi-char operator
            pos++;
            col++;
            switch (current) {
            case '>': return Token{ TokenType::GREATER, ">", line, col - 1 };
            case '<': return Token{ TokenType::LESS, "<", line, col - 1 };
            case '=': return Token{ TokenType::EQUALS, "=", line, col - 1 };
            case '!': return Token{ TokenType::NOT, "!", line, col - 1 };
            default: break;
            }
        }

        // Handle other single-character operators
        if (current == '+' || current == '-' || current == '*' || current == '/' || current == '(' || current == ')' || current == ';' || current == ',') {
            pos++;
            col++;
            switch (current) {
            case '+': return Token{ TokenType::PLUS, "+", line, col - 1 };
            case '-': return Token{ TokenType::MINUS, "-", line, col - 1 };
            case '*': return Token{ TokenType::MUL, "*", line, col - 1 };
            case '/': return Token{ TokenType::DIV, "/", line, col - 1 };
            case '(': return Token{ TokenType::LPAREN, "(", line, col - 1 };
            case ')': return Token{ TokenType::RPAREN, ")", line, col - 1 };
            case ';': return Token{ TokenType::SEMICOLON, ";", line, col - 1 };
            case ',': return Token{ TokenType::COMMA, ",", line, col - 1 };
            default: break;
            }
        }

        // Handle strings
        if (current == '\'') {
            pos++;
            col++;
            std::string str;
            while (pos < input.size() && input[pos] != '\'') {
                str += input[pos];
                pos++;
                col++;
            }
            if (pos == input.size()) {
                throw std::runtime_error("Unterminated string literal");
            }
            pos++; // Skip closing quote
            col++;
            return Token{ TokenType::STRING, str, line, static_cast<int>(col - str.size() - 2) };
        }

        // Handle numbers
        if (isdigit(current) || (current == '.' && pos + 1 < input.size() && isdigit(input[pos + 1]))) {
            std::string num;
            bool hasDot = false;
            while (pos < input.size() && (isdigit(input[pos]) || input[pos] == '.')) {
                if (input[pos] == '.') {
                    if (hasDot) break; // Second dot, stop
                    hasDot = true;
                }
                num += input[pos];
                pos++;
                col++;
            }
            return Token{ TokenType::NUMBER, num, line, static_cast<int>(col - num.size()) };
        }

        // Handle identifiers and keywords
        if (isalpha(current) || current == '_') {
            std::string ident;
            while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
                ident += input[pos];
                pos++;
                col++;
            }
            // Convert to uppercase for case-insensitive matching
            std::string upperIdent = ident;
            for (auto& c : upperIdent) c = toupper(c);

            // Check for 'ELSE IF' combination
            if (upperIdent == "ELSE" && pos < input.size()) {
                size_t savedPos = pos;
                int savedCol = col;
                // Look ahead for 'IF'
                while (pos < input.size() && isspace(input[pos])) {
                    if (input[pos] == '\n') {
                        line++;
                        col = 1;
                    }
                    else {
                        col++;
                    }
                    pos++;
                }
                std::string nextIdent;
                while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
                    nextIdent += input[pos];
                    pos++;
                    col++;
                }
                std::string upperNextIdent = nextIdent;
                for (auto& c : upperNextIdent) c = toupper(c);
                if (upperNextIdent == "IF") {
                    return Token{ TokenType::KEYWORD_ELSE_IF, "ELSE IF", line, col - (int)nextIdent.size() - 5 };
                }
                else {
                    // Not 'ELSE IF', rollback and return 'ELSE'
                    pos = savedPos;
                    col = savedCol;
                    return Token{ TokenType::KEYWORD_ELSE, "ELSE", line, col - (int)ident.size() };
                }
            }

            // Check if it's a keyword
            if (keywords.find(upperIdent) != keywords.end()) {
                return Token{ keywords.at(upperIdent), ident, line, static_cast<int>(col - ident.size()) };
            }
            else {
                return Token{ TokenType::IDENTIFIER, ident, line, static_cast<int>(col - ident.size()) };
            }
        }

        // If we reach here, it's an unknown character
        throw std::runtime_error(std::string("Unknown character: ") + current);
    }

    // Return EOF token if end of input is reached
    return Token{ TokenType::EOF_TOKEN, "", line, col };
}
