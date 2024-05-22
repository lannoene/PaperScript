#include "lexer.h"
#include "parser.h"
#include "asm_build.h"

#include <iostream>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cout << "please specify a single start file" << std::endl;
		exit(1);
	}
	Lexer lex(argv[1]);
	std::vector<Token> lexTokens = lex.ParseFile();
	Parser parser;
	std::shared_ptr<TranslationUnit> baseNode = parser.ParseTokens(lexTokens);
	AsmBuilder builder;
	builder.BuildWasm(baseNode);
	std::cout << "build completed successfully" << std::endl;
}