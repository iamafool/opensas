#include "Lexer.h"
#include "utility.h"
#include <cctype>
#include <algorithm>
#include <stdexcept>

namespace sass {

    Lexer::Lexer(const std::string& in) : input(in) {
        keywords["AND"] = TokenType::AND;
        keywords["ARRAY"] = TokenType::KEYWORD_ARRAY;
        keywords["AS"] = TokenType::KEYWORD_AS;
        keywords["BY"] = TokenType::KEYWORD_BY;
        keywords["CHISQ"] = TokenType::KEYWORD_CHISQ;
        keywords["CREATE"] = TokenType::KEYWORD_CREATE;
        keywords["DATA"] = TokenType::KEYWORD_DATA;
        keywords["DATALINES"] = TokenType::KEYWORD_DATALINES;
        keywords["DELETE"] = TokenType::KEYWORD_DELETE;
        keywords["DO"] = TokenType::KEYWORD_DO;
        keywords["DUPLICATES"] = TokenType::KEYWORD_DUPLICATES;
        keywords["ELSE IF"] = TokenType::KEYWORD_ELSE_IF;
        keywords["ELSE"] = TokenType::KEYWORD_ELSE;
        keywords["END"] = TokenType::KEYWORD_END;
        keywords["FREQ"] = TokenType::KEYWORD_FREQ;
        keywords["FROM"] = TokenType::KEYWORD_FROM;
        keywords["FULL"] = TokenType::KEYWORD_FULL;
        keywords["GROUP"] = TokenType::KEYWORD_GROUP;
        keywords["HAVING"] = TokenType::KEYWORD_HAVING;
        keywords["IF"] = TokenType::KEYWORD_IF;
        keywords["INNER"] = TokenType::KEYWORD_INNER;
        keywords["INPUT"] = TokenType::KEYWORD_INPUT;
        keywords["INSERT"] = TokenType::KEYWORD_INSERT;
        keywords["JOIN"] = TokenType::KEYWORD_JOIN;
        keywords["LABEL"] = TokenType::KEYWORD_LABEL;
        keywords["LEFT"] = TokenType::KEYWORD_LEFT;
        keywords["LIBNAME"] = TokenType::KEYWORD_LIBNAME;
        keywords["MAX"] = TokenType::KEYWORD_MAX;
        keywords["MEAN"] = TokenType::KEYWORD_MEAN;
        keywords["MEANS"] = TokenType::KEYWORD_MEANS;
        keywords["MEDIAN"] = TokenType::KEYWORD_MEDIAN;
        keywords["MERGE"] = TokenType::KEYWORD_MERGE;
        keywords["MIN"] = TokenType::KEYWORD_MIN;
        keywords["N"] = TokenType::KEYWORD_N;
        keywords["NOCUM"] = TokenType::KEYWORD_NOCUM;
        keywords["NODUPKEY"] = TokenType::KEYWORD_NODUPKEY;
        keywords["NOMISSING"] = TokenType::KEYWORD_NOMISSING; // Optional for 
        keywords["NOOBS"] = TokenType::KEYWORD_NOOBS;
        keywords["NOPRINT"] = TokenType::KEYWORD_NOPRINT;
        keywords["NOT"] = TokenType::NOT;
        keywords["OBS"] = TokenType::KEYWORD_OBS;
        keywords["ON"] = TokenType::KEYWORD_ON;
        keywords["OPTIONS"] = TokenType::KEYWORD_OPTIONS;
        keywords["OR"] = TokenType::OR;
        keywords["ORDER"] = TokenType::KEYWORD_ORDER;
        keywords["OUT"] = TokenType::KEYWORD_OUT;
        keywords["OUTER"] = TokenType::KEYWORD_OUTER;
        keywords["OUTPUT"] = TokenType::KEYWORD_OUTPUT;
        keywords["PRINT"] = TokenType::KEYWORD_PRINT;
        keywords["PROC"] = TokenType::KEYWORD_PROC;
        keywords["QUIT"] = TokenType::KEYWORD_QUIT;
        keywords["RIGHT"] = TokenType::KEYWORD_RIGHT;
        keywords["RUN"] = TokenType::KEYWORD_RUN;
        keywords["SELECT"] = TokenType::KEYWORD_SELECT;
        keywords["SET"] = TokenType::KEYWORD_SET;
        keywords["SORT"] = TokenType::KEYWORD_SORT;
        keywords["SQL"] = TokenType::KEYWORD_SQL;
        keywords["STD"] = TokenType::KEYWORD_STD;
        keywords["TABLE"] = TokenType::KEYWORD_TABLE;
        keywords["TABLES"] = TokenType::KEYWORD_TABLES;
        keywords["THEN"] = TokenType::KEYWORD_THEN;
        keywords["TITLE"] = TokenType::KEYWORD_TITLE;
        keywords["UNTIL"] = TokenType::KEYWORD_UNTIL;
        keywords["UPDATE"] = TokenType::KEYWORD_UPDATE;
        keywords["VAR"] = TokenType::KEYWORD_VAR;
        keywords["WHERE"] = TokenType::KEYWORD_WHERE;
        keywords["WHILE"] = TokenType::KEYWORD_WHILE;

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

    // We detect if this identifier is "DATA", "DATALINES", "INPUT", etc.
    Token Lexer::identifierOrKeyword() {
        std::string ident;
        while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
            ident += input[pos];
            pos++;
            col++;
        }
        // Convert to uppercase for case-insensitive matching
        std::string upperIdent = ident;
        for (auto& c : upperIdent) c = toupper(c);

        if (upperIdent == "DATALINES")
            justSawDatalinesKeyword = true;

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


    Token Lexer::getNextToken() {
        // If we're already inDatalinesMode, read all lines until we see a line that is ';'
        if (inDatalinesMode) {
            return readDatalinesContent();
        }

        skipWhitespace();

        char c = peekChar();

        // If we just saw the 'KEYWORD_DATALINES' token, the *very next* token should be
        // either SEMICOLON => which triggers inDatalinesMode on the next call
        // or something else. Let's handle that logic:
        if (justSawDatalinesKeyword) {
            justSawDatalinesKeyword = false;  // reset
            if (c == ';') {
                // produce a SEMICOLON token
                getChar(); // consume
                Token tk{ TokenType::SEMICOLON, ";", line, col - 1 };
                // Next time getNextToken is called => we set inDatalinesMode = true
                // Because in SAS, "datalines;" => from next line onward we read data
                inDatalinesMode = true;
                return tk;
            }
            // else fall through as normal tokens if user typed "datalines X" => error in parser
        }

        // Handle macro statements starting with '%'
        if (c == '%') {
            return macroToken();
        }

        // Handle macro variables starting with '&'
        if (c == '&') {
            return macroVariable();
        }

        if (c == '$') {
            getChar(); // consume
            return { TokenType::DOLLAR, "$", line, col - 1 };
        }

        while (pos < input.size()) {
            char current = input[pos];

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
                case '=': return Token{ TokenType::EQUAL, "=", line, col - 1 };
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
                return identifierOrKeyword();
            }

            // If we reach here, it's an unknown character
            throw std::runtime_error(std::string("Unknown character: ") + current);
        }

        // Return EOF token if end of input is reached
        return Token{ TokenType::EOF_TOKEN, "", line, col };
    }

    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;
        Token tok;
        while ((tok = getNextToken()).type != TokenType::EOF_TOKEN) {
            tokens.push_back(tok);
        }
        return tokens;
    }

    Token Lexer::macroToken() {
        getChar(); // Consume '%'
        std::string value;
        while (std::isalnum(peekChar())) {
            value += getChar();
        }

        if (value == "let") return Token{ TokenType::KEYWORD_MACRO_LET, "%let", line, col };
        if (value == "macro") return Token{ TokenType::KEYWORD_MACRO_MACRO, "%macro", line, col };
        if (value == "mend") return Token{ TokenType::KEYWORD_MACRO_MEND, "%mend", line, col };
        if (value == "do") return Token{ TokenType::KEYWORD_MACRO_DO, "%do", line, col };
        if (value == "if") return Token{ TokenType::KEYWORD_MACRO_IF, "%if", line, col };
        if (value == "then") return Token{ TokenType::KEYWORD_MACRO_THEN, "%then", line, col };
        if (value == "else") return Token{ TokenType::KEYWORD_MACRO_ELSE, "%else", line, col };
        throw std::runtime_error("Unknown macro keyword: %" + value);
    }

    Token Lexer::macroVariable() {
        getChar(); // Consume '&'
        std::string value;
        while (std::isalnum(peekChar()) || peekChar() == '_') {
            value += getChar();
        }
        return Token{ TokenType::MACRO_VAR, "&" + value, line, col };
    }

    // Actual reading of datalines content
    Token Lexer::readDatalinesContent() {
        // We'll read line by line until we see a line whose "trimmed" content is ";"
        // We'll accumulate those lines (minus the final ";")
        // Then produce a single DATALINES_CONTENT token.
        std::string allLines;
        while (true) {
            if (pos >= input.size()) {
                // End of file => done
                inDatalinesMode = false;
                break;
            }

            // read one line from the raw input
            int lineStart = line;
            size_t colStart = col;

            std::string currentLine;
            while (pos < input.size() && peekChar() != '\n') {
                currentLine.push_back(getChar());
            }

            // consume newline if present
            if (pos < input.size() && peekChar() == '\n') {
                getChar(); // consume it
            }

            // We advanced line, col in getChar()
            line++;
            col = 1;

            // Trim
            std::string trimmedLine = trim(currentLine);

            if (trimmedLine == ";") {
                // This is the end of datalines block
                inDatalinesMode = false;
                break;
            }

            // Accumulate the untrimmed line or minimal trimmed version
            // Usually we keep them as is, or you can store them with trailing spaces removed.
            // We'll store them with minimal trailing whitespace
            allLines += currentLine + "\n";
        }

        // produce the one token
        Token tok;
        tok.type = TokenType::DATALINES_CONTENT;
        tok.text = allLines;
        tok.line = line;
        tok.col = col;
        return tok;
    }
}
