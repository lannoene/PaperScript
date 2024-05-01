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

enum TOKENS {
	TOK_UNKOWN,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
};

Lexer::Lexer(std::string sourceDir) {
	std::fstream file(sourceDir);
	if (!file.is_open()) {
		std::cout << "could not ope file: " << sourceDir << std::endl;
		return;
	} else {
		std::cout << "opened file" << std::endl;
	}
	while (std::getline(file, line)) {
		contents += line + '\n';
	}
	contents.pop_back();
	
	ParseFile();
	std::cout << "finished parseing file" << std::endl;
}

void SkipWhitespace() {
	while (contents[])
}