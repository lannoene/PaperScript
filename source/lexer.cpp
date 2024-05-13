#include "lexer.h"

#include <fstream>
#include <iostream>

enum KEYWORDS {
	KEYWORD_UNKOWN,
	KEYWORD_INC,
	KEYWORD_PUB,
	KEYWORD_PRV,
	KEYWORD_INT,
	KEYWORD_VOID,
	KEYWORD_IF,
	KEYWORD_ELSE,
};

void Lexer::Warning(std::string warning) {
	std::cout << "LEXER WARNING (@" << place << " on line " << line << "): " << warning << std::endl;
}

Lexer::Lexer(std::string sourceDir) {
	std::fstream file(sourceDir);
	if (!file.is_open()) {
		std::cout << "could not ope file: " << sourceDir << std::endl;
		return;
	} else {
		std::cout << "opened file" << std::endl;
	}
	std::string line;
	while (std::getline(file, line)) {
		// remove comments
		if (line.find("//") != std::string::npos)
			line = line.erase(line.find("//"));
		contents += line + '\n';
	}
	contents.pop_back(); // remove last newline
}

std::vector<Token> Lexer::ParseFile() {
	std::vector<Token> tokens;
	while (place < contents.length()) {
		Token tok = GetNextToken();
		AdvancePlace();
		//std::cout << "New token " << tok.GetType() << " " << tok.GetIdentifier() << " " << tok.GetClass() << std::endl;
		tokens.push_back(tok);
	}
	return tokens;
}

void Lexer::SkipWhitespace() {
	while (contents[place] == ' ' || contents[place] == '\n' || contents[place] == '\t') {
		if (contents[place] == '\n')
			++line;
		AdvancePlace();
	}
}

void Lexer::AdvancePlace() {
	place++;
}

std::string Lexer::GetNextString() {
	if (contents[place] == '"') {
		std::string str;
		while (contents[++place] != '"') {
			str += contents[place];
		}
		return str;
	} else {
		
		exit(1);
		return "";
	}
	return "";
}

std::string Lexer::GetStrUntilNonAlpha() {
	std::string str;
	while (isalnum(contents[place])) {
		str += contents[place];
		AdvancePlace();
	}
	place--;
	return str;
}

enum KEYWORD_IDS Lexer::GetKeywordId(std::string keyword) {
	if (keyword == "if") {
		return KEY_IF;
	} else if (keyword == "else") {
		return KEY_ELSE;
	} else if (keyword == "inc") {
		return KEY_INCLUDE;
	} else if (keyword == "pub") {
		return KEY_PUBLIC;
	} else if (keyword == "prv") {
		return KEY_PRIVATE;
	} else if (keyword == "true") {
		return KEY_TRUE;
	} else if (keyword == "false") {
		return KEY_FALSE;
	} else if (keyword == "int") {
		return KEY_INT;
	} else if (keyword == "str") {
		return KEY_STRING;
	} else if (keyword == "void") {
		return KEY_VOID;
	} else if (keyword == "ret") {
		return KEY_RETURN;
	} else if (keyword == "while") {
		return KEY_WHILE;
	} else if (keyword == "float") {
		return KEY_FLOAT;
	}
	return KEY_UNKNOWN;
}

Token Lexer::GetNextToken() {
	SkipWhitespace();
	
	char curChr = contents[place];
	
	// if is alphanumeric, then it may be a keyword
	if (isalnum(curChr)) {
		std::string keyword = GetStrUntilNonAlpha();
		if (isdigit(curChr)) { // TODO: conv to float later using std::stof with checking for exception
			if (keyword.find('.') != std::string::npos) {
				return Token(TOK_INT_VALUE, keyword, std::stof(keyword), CLASS_LITERAL, line);
			} else {
				return Token(TOK_INT_VALUE, keyword, std::stoi(keyword), CLASS_LITERAL, line);
			}
		}
		
		switch (GetKeywordId(keyword)) {
			case KEY_UNKNOWN:
				return FormatToken(TOK_IDENTIFIER, CLASS_IDENTIFIER, keyword);
			case KEY_IF:
				return FormatToken(TOK_IF, CLASS_STATEMENT);
			case KEY_INCLUDE:
				return FormatToken(TOK_INCLUDE, CLASS_STATEMENT);
			case KEY_ELSE:
				return FormatToken(TOK_ELSE, CLASS_STATEMENT);
			case KEY_PRIVATE:
				return FormatToken(TOK_PRIVATE, CLASS_GENERIC);
			case KEY_PUBLIC:
				return FormatToken(TOK_PUBLIC, CLASS_GENERIC);
			case KEY_TRUE:
				return FormatToken(TOK_TRUE, CLASS_GENERIC);
			case KEY_FALSE:
				return FormatToken(TOK_FALSE, CLASS_GENERIC);
			case KEY_INT:
				return FormatToken(TOK_INT, CLASS_TYPE);
			case KEY_STRING:
				return FormatToken(TOK_STRING, CLASS_TYPE);
			case KEY_VOID:
				return FormatToken(TOK_VOID, CLASS_TYPE);
			case KEY_RETURN:
				return FormatToken(TOK_RETURN, CLASS_TYPE);
			case KEY_WHILE:
				return FormatToken(TOK_WHILE, CLASS_STATEMENT);
			case KEY_FLOAT:
				return FormatToken(TOK_FLOAT, CLASS_TYPE);
		}
	}
	
	// otherwise if it is a symbol, check for operator
	switch (curChr) {
		default: {
			std::string warning = "Unexpected symbol: ";
			warning += curChr;
			Warning(warning);
			break;
		}
		case '"': {
			Token t(TOK_STRING_VALUE, GetNextString(), CLASS_LITERAL, line);
			t.SetLiteral(true);
			return t;
		}
		case '(':
			return Token(TOK_LPAREN, "(", CLASS_GENERIC, line);
		case ')':
			return Token(TOK_RPAREN, ")", CLASS_GENERIC, line);
		case '{':
			return Token(TOK_LBRACE, "{", CLASS_GENERIC, line);
		case '}':
			return Token(TOK_RBRACE, "}", CLASS_GENERIC, line);
		case '=':
			if (contents[place + 1] == '=') { // then it is an equality sign
				++place; // skip other sign
				return Token(TOK_EQUALITY, "==", CLASS_EQUALITY, line);
			} else { // it's a regular assignment operator
				return Token(TOK_EQUALS, "=", CLASS_GENERIC, line);
			}
			break;
		case '!':
			if (contents[place + 1] == '=') { // then it is an equality sign
				++place; // skip other sign
				return FormatToken(TOK_NOT_EQUALITY, CLASS_EQUALITY);
			} else { // it's a regular assignment operator
				return FormatToken(TOK_NOT, CLASS_GENERIC);
			}
			break;
		case ',':
			return Token(TOK_COMMA, ",", CLASS_GENERIC, line);
			break;
		case ';':
			return Token(TOK_SEMICOLON, ";", CLASS_GENERIC, line);
			break;
		case '>':
			if (contents[place + 1] == '=') {
				++place;
				return Token(TOK_GREATER_EQ, ">=", CLASS_EQUALITY, line);
			} else {
				return Token(TOK_GREATER, ">", CLASS_EQUALITY, line);
			}
		case '<': {
			if (contents[place + 1] == '=') {
				++place;
				return Token(TOK_LESS_EQ, "<=", CLASS_EQUALITY, line);
			} else {
				return Token(TOK_LESS, "<", CLASS_EQUALITY, line);
			}
		}
		case '+':
			// TODO: check for ++
			return Token(TOK_PLUS, "+", CLASS_MATH_OPERATION, line);
		case '-':
			return FormatToken(TOK_MINUS, CLASS_MATH_OPERATION);
		case '[':
			return Token(TOK_LBRACKET, "[", CLASS_GENERIC, line);
		case ']':
			return Token(TOK_RBRACKET, "]", CLASS_GENERIC, line);
		case '*':
			return FormatToken(TOK_STAR, CLASS_GENERIC);
		case '/':
			return FormatToken(TOK_FSLASH, CLASS_GENERIC);
		case '&': {
			if (contents[place + 1] == '&') {
				return FormatToken(TOK_BIT_AND, CLASS_GENERIC);
			} else {
				return FormatToken(TOK_AND, CLASS_GENERIC);
			}
		}
		case '%':
			return FormatToken(TOK_REMAINDER, CLASS_MATH_OPERATION);
	}
	return Token(TOK_NONE, "---could not decode token (line " + std::to_string(line) + "!---", CLASS_GENERIC, line);
}

Token Lexer::FormatToken(enum token_types t, enum token_class c) {
	return Token(t, tokIden[t], c, line);
}

Token Lexer::FormatToken(enum token_types t, enum token_class c, std::string iden) {
	return Token(t, iden, c, line);
}