#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

enum TOKEN_TYPES {
	TOK_UNKOWN,
	TOK_NONE,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_EQUALS,
	TOK_EQUALITY,
	TOK_GREATER,
	TOK_LESS,
	TOK_GREATER_EQ,
	TOK_LESS_EQ,
	TOK_PLUS,
	TOK_MINUS,
	TOK_INT,
	TOK_INT_VALUE,
	TOK_STRING,
	TOK_STRING_VALUE,
	TOK_VOID,
	TOK_COMMA,
	TOK_SEMICOLON,
	TOK_IF,
	TOK_INCLUDE,
	TOK_ELSE,
	TOK_IDENTIFIER,
	TOK_PRIVATE,
	TOK_PUBLIC,
	TOK_TRUE,
	TOK_FALSE,
	TOK_LBRACKET,
	TOK_RBRACKET,
};

enum KEYWORD_IDS {
	KEY_UNKNOWN,
	KEY_IF,
	KEY_ELSE,
	KEY_INCLUDE,
	KEY_PRIVATE,
	KEY_PUBLIC,
	KEY_TRUE,
	KEY_FALSE,
	KEY_INT,
	KEY_STRING,
	KEY_VOID
};

enum TOKEN_CLASS {
	CLASS_GENERIC,
	CLASS_EQUALITY,
	CLASS_LITERAL,
	CLASS_IDENTIFIER,
	CLASS_TYPE,
	CLASS_STATEMENT,
	CLASS_MATH_OPERATION,
};

class Token {
public:
	Token(enum TOKEN_TYPES ntype, std::string ident, enum TOKEN_CLASS c) {
		type = ntype;
		identifier = ident;
		tclass = c;
	}
	enum TOKEN_TYPES GetType() {return type;}
	std::string GetIdentifier() {return identifier;}
	void SetLiteral(bool l) {
		isLiteral = l;
	}
	enum TOKEN_CLASS GetClass() {return tclass;}
private:
	enum TOKEN_TYPES type;
	enum TOKEN_CLASS tclass;
	std::string identifier;
	bool isLiteral = false;
};

class Lexer {
public:
	Lexer(std::string sourceDir);
	std::vector<Token> ParseFile();
private:
	void Warning(std::string warning);
	size_t place = 0;
	int line = 1;
	std::string contents;
	enum KEYWORD_IDS GetKeywordId(std::string keyword);
	std::string GetNextString();
	Token GetNextToken();
	void SkipWhitespace();
	void AdvancePlace();
	std::string GetStrUntilNonAlpha();
};
