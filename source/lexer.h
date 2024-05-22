#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

enum token_types {
	TOK_UNKOWN,
	TOK_NONE,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_EQUALS,
	TOK_EQUALITY,
	TOK_NOT_EQUALITY,
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
	TOK_WHILE,
	TOK_INCLUDE,
	TOK_ELSE,
	TOK_IDENTIFIER,
	TOK_PRIVATE,
	TOK_PUBLIC,
	TOK_TRUE,
	TOK_FALSE,
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_STAR,
	TOK_FSLASH,
	TOK_NOT,
	TOK_BIT_AND,
	TOK_AND,
	TOK_RETURN,
	TOK_BREAK,
	TOK_REMAINDER,
	TOK_FLOAT,
	TOK_FLOAT_VALUE,
	TOK_OR,
	TOK_BIT_OR,
	TOK_PLUS_EQ,
};

inline const char *tokIden[] = {
	"[[unkown]]",
	"[[none]]",
	"(",
	")",
	"{",
	"}",
	"=",
	"==",
	"!=",
	">",
	"<",
	">=",
	"<=",
	"+",
	"-",
	"int",
	"[[int value]]",
	"str",
	"[[str value]]",
	"void",
	",",
	";",
	"if",
	"while",
	"inc",
	"else",
	"[[identifier]]",
	"prv",
	"pub",
	"true",
	"false",
	"[",
	"]",
	"*",
	"/",
	"!",
	"&",
	"&&",
	"ret",
	"break",
	"%",
	"float",
	"[[float value]]",
	"||",
	"|",
	"+=",
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
	KEY_VOID,
	KEY_RETURN,
	KEY_WHILE,
	KEY_FLOAT,
};

enum token_class {
	CLASS_GENERIC,
	CLASS_EQUALITY,
	CLASS_LITERAL,
	CLASS_IDENTIFIER,
	CLASS_TYPE,
	CLASS_STATEMENT,
	CLASS_MATH_OPERATION,
};

union VALUE {
	int vInt;
	float vFloat;
};

class Token {
public:
	Token(enum token_types ntype, std::string ident, enum token_class c, int line) {
		type = ntype;
		identifier = ident;
		tclass = c;
		lineNum = line;
	}
	Token(enum token_types ntype, std::string ident, int intLiteralValue, enum token_class c, int line) : Token(ntype, ident, c, line) {
		value.vInt = intLiteralValue;
	}
	Token(enum token_types ntype, std::string ident, float fltLiteralValue, enum token_class c, int line) : Token(ntype, ident, c, line) {
		value.vFloat = fltLiteralValue;
	}
	Token() = default;
	enum token_types GetType() {return type;}
	std::string GetIdentifier() {return identifier;}
	void SetLiteral(bool l) {
		isLiteral = l;
	}
	enum token_class GetClass() {return tclass;}
	union VALUE GetLiteralValue() {return value;}
	bool operator==(enum token_types t) {
		return GetType() == t;
	}
	bool operator==(enum token_class c) {
		return GetClass() == c;
	}
	bool operator!=(enum token_types t) {
		return GetType() != t;
	}
	int GetLineNumber() {return lineNum;}
private:
	enum token_types type;
	enum token_class tclass;
	std::string identifier;
	bool isLiteral = false;
	union VALUE value;
	int lineNum;
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
	Token FormatToken(enum token_types t, enum token_class c);
	Token FormatToken(enum token_types t, enum token_class c, std::string iden);
};