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
		keywords["END"] = TokenType::KEYWORD_ENDDO;
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
		keywords["RETAIN"] = TokenType::KEYWORD_RETAIN;
		keywords["DROP"] = TokenType::KEYWORD_DROP;
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

	void Lexer::skipBlockComment() {
		// We already know we have '/' '*' at pos, so let's consume them
		// i.e. pos points to '/', pos+1 is '*'
		pos += 2;  // skip "/*"
		col += 2;

		while (pos < input.size()) {
			// check if we see "*/"
			if (input[pos] == '*' && (pos + 1 < input.size()) && input[pos + 1] == '/') {
				// end of comment
				pos += 2;
				col += 2;
				return;
			}
			// handle newline
			if (input[pos] == '\n') {
				line++;
				col = 1;
			}
			else {
				col++;
			}
			pos++;
		}

		// If we get here, we reached end of file without "*/"
		// In real SAS, might consider that an error or continue
		// We'll just return or throw
		// throw std::runtime_error("Unterminated block comment /* ... */");
	}

	void Lexer::skipLineComment(bool macro) {
	// If macro == false => we saw "*"
	// If macro == true => we saw "%*"

	// We already know we are on '*' or '%*', so let's skip those chars
	if (macro) {
		// skip '%*'
		pos += 2;
		col += 2;
	}
	else {
		// skip '*'
		pos++;
		col++;
	}

	// Now we skip until we see a semicolon ';'
	while (pos < input.size()) {
		char c = input[pos];
		if (c == ';') {
			// We found the end of the comment
			// In SAS, the semicolon belongs to the comment itself and isn't a statement terminator
			pos++;
			col++;
			return;
		}
		// handle newlines for line counting
		if (c == '\n') {
			line++;
			col = 1;
		}
		else {
			col++;
		}
		pos++;
	}

	// If we get here, we reached the end of the file without a semicolon
	// Real SAS might treat that as the end of comment or error
	// We'll just return
	// throw std::runtime_error("Unterminated line comment * ... ;");
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

		// Check which quote char we have: single or double
		char quoteChar = getChar(); // consume the initial quote
		// We already advanced pos by 1, col by 1

		// We'll store the unescaped string in 'value'
		std::string value;

		// We'll keep reading until we find a matching quoteChar
		// If we see the same quoteChar repeated (e.g. '' or ""), that means
		// an embedded quote, so we consume two quotes and add one to 'value'.
		while (true) {
			if (pos >= input.size()) {
				// Reached end of file/string without closing quote
				throw std::runtime_error("Unterminated string literal");
			}

			char c = peekChar();
			if (c == quoteChar) {
				// Look ahead to see if we have a doubled quote
				if (pos + 1 < input.size() && input[pos + 1] == quoteChar) {
					// It's a doubled quote => consume both, add one to 'value'
					// e.g. '' => '  or  "" => "
					getChar(); // consume first quote
					getChar(); // consume second quote
					col += 2;  // we advanced two chars
					// Add a single quoteChar to 'value'
					value.push_back(quoteChar);
				}
				else {
					// It's a real closing quote
					getChar(); // consume
					col++;
					break; // end of string
				}
			}
			else {
				// Normal char => add to 'value'
				value.push_back(c);
				getChar(); // consume it
				col++;
			}
		}

		// Build the token
		token.type = TokenType::STRING;
		token.text = value;

		// token.line, token.col were set at start, but col will be advanced now
		// You can adjust if you want the token's col to point to the start or end

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
				return Token{ TokenType::KEYWORD_ELSE_IF, "ELSE IF", line, col - (int)nextIdent.size() - 5, true };
			}
			else {
				// Not 'ELSE IF', rollback and return 'ELSE'
				pos = savedPos;
				col = savedCol;
				return Token{ TokenType::KEYWORD_ELSE, "ELSE", line, col - (int)ident.size(), true };
			}
		}

		// Check if it's a keyword
		if (keywords.find(upperIdent) != keywords.end()) {
			return Token{ keywords.at(upperIdent), ident, line, static_cast<int>(col - ident.size()), true };
		}
		else {
			return Token{ TokenType::IDENTIFIER, ident, line, static_cast<int>(col - ident.size()), true };
		}
	}


	Token Lexer::getNextToken() {
		skipWhitespace();

		// If we're already inDatalinesMode, read all lines until we see a line that is ';'
		if (inDatalinesMode) {
			return readDatalinesContent();
		}

		char c = peekChar();

		// handle block comment
		if (c == '/' && (pos + 1 < input.size()) && input[pos + 1] == '*') {
			skipBlockComment();
			return getNextToken();
		}

		// handle line comment "* ... ;"
		if (c == '*') {
			if (atStatementStart) {
				skipLineComment(false);
				return getNextToken();
			}
			else {
				// It's a multiplication operator, not a comment
				// produce a Token{ TokenType::STAR, "*", ...}
				Token starTok;
				starTok.type = TokenType::STAR; // or however you represent '*'
				starTok.text = "*";
				starTok.line = line;
				starTok.col = col;
				pos++;
				col++;
				atStatementStart = false; // now we're definitely not at start
				return starTok;
			}
		}

		// handle macro comment "%* ... ;"
		if (c == '%' && (pos + 1 < input.size()) && input[pos + 1] == '*') {
			if (atStatementStart)
			{
				skipLineComment(true);
				return getNextToken();
			}
			else {

			}
		}

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
				atStatementStart = false;
				return tk;
			}
			// else fall through as normal tokens if user typed "datalines X" => error in parser
		}

		// Maybe c == ';' => produce a SEMICOLON token
		if (c == ';') {
			Token semTok;
			semTok.type = TokenType::SEMICOLON;
			semTok.text = ";";
			semTok.line = line;
			semTok.col = col;
			pos++;
			col++;
			// after we read a semicolon, the next token is start-of-statement
			atStatementStart = true;
			return semTok;
		}

		atStatementStart = false;


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
			if (current == '+' || current == '-' || current == '*' || current == '/' || current == '(' || current == ')' || current == ';' || current == ',' || current == '{' || current == '}') {
				pos++;
				col++;
				switch (current) {
				case '+': return Token{ TokenType::PLUS, "+", line, col - 1 };
				case '-': return Token{ TokenType::MINUS, "-", line, col - 1 };
				case '*': return Token{ TokenType::MUL, "*", line, col - 1 };
				case '/': return Token{ TokenType::DIV, "/", line, col - 1 };
				case '(': return Token{ TokenType::LPAREN, "(", line, col - 1 };
				case ')': return Token{ TokenType::RPAREN, ")", line, col - 1 };
				case '{': return Token{ TokenType::LBRACE, "{", line, col - 1 };
				case '}': return Token{ TokenType::RBRACE, "}", line, col - 1 };
				case ',': return Token{ TokenType::COMMA, ",", line, col - 1 };
				default: break;
				}
			}

			// Handle strings
			if (current == '\'' || current == '"') {
				return stringLiteral();
			}

			if (current == '.') {
				pos++;
				col++;
				return Token{ TokenType::DOT, ".", line, col - 1 };
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