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
	}
	return KEY_UNKNOWN;
}

Token Lexer::GetNextToken() {
	SkipWhitespace();
	
	char curChr = contents[place];
	
	// if is alphanumeric, then it may be a keyword
	if (isalnum(curChr)) {
		std::string keyword = GetStrUntilNonAlpha();
		if (isdigit(curChr)) {
			return Token(TOK_INT_VALUE, keyword, std::stoi(keyword), CLASS_LITERAL, line);
		}
		
		switch (GetKeywordId(keyword)) {
			case KEY_UNKNOWN:
				return Token(TOK_IDENTIFIER, keyword, CLASS_IDENTIFIER, line);
				break;
			case KEY_IF:
				return Token(TOK_IF, "if", CLASS_STATEMENT, line);
				break;
			case KEY_INCLUDE:
				return Token(TOK_INCLUDE, "inc", CLASS_STATEMENT, line);
				break;
			case KEY_ELSE:
				return Token(TOK_ELSE, "else", CLASS_STATEMENT, line);
				break;
			case KEY_PRIVATE:
				return Token(TOK_PRIVATE, "prv", CLASS_GENERIC, line);
				break;
			case KEY_PUBLIC:
				return Token(TOK_PUBLIC, "pub", CLASS_GENERIC, line);
				break;
			case KEY_TRUE:
				return Token(TOK_TRUE, "true", CLASS_GENERIC, line);
				break;
			case KEY_FALSE:
				return Token(TOK_FALSE, "false", CLASS_GENERIC, line);
				break;
			case KEY_INT:
				return Token(TOK_INT, "int", CLASS_TYPE, line);
				break;
			case KEY_STRING:
				return Token(TOK_STRING, "string", CLASS_TYPE, line);
				break;
			case KEY_VOID:
				return Token(TOK_VOID, "void", CLASS_TYPE, line);
				break;
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
			break;
		}
		case '(':
			return Token(TOK_LPAREN, "(", CLASS_GENERIC, line);
			break;
		case ')':
			return Token(TOK_RPAREN, ")", CLASS_GENERIC, line);
			break;
		case '{':
			return Token(TOK_LBRACE, "{", CLASS_GENERIC, line);
			break;
		case '}':
			return Token(TOK_RBRACE, "}", CLASS_GENERIC, line);
			break;
		case '=':
			if (contents[place + 1] == '=') { // then it is an equality sign
				++place; // skip other sign
				return Token(TOK_EQUALITY, "==", CLASS_EQUALITY, line);
			} else { // it's a regular assignment operator
				return Token(TOK_EQUALS, "=", CLASS_GENERIC, line);
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
			break;
		case '<': {
			if (contents[place + 1] == '=') {
				++place;
				return Token(TOK_LESS_EQ, "<=", CLASS_EQUALITY, line);
			} else {
				return Token(TOK_LESS, "<", CLASS_EQUALITY, line);
			}
			break;
		}
		case '+':
			// TODO: check for ++
			return Token(TOK_PLUS, "+", CLASS_MATH_OPERATION, line);
			break;
		case '[':
			return Token(TOK_LBRACKET, "[", CLASS_GENERIC, line);
			break;
		case ']':
			return Token(TOK_RBRACKET, "]", CLASS_GENERIC, line);
			break;
	}
	return Token(TOK_NONE, "---could not decode token (line " + std::to_string(line) + "!---", CLASS_GENERIC, line);
}
