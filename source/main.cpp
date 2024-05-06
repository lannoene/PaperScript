#include "lexer.h"
#include "parser.h"
#include "asm_build.h"

int main() {
	Lexer lex("../tests/main.ppr");
	std::vector<Token> lexTokens = lex.ParseFile();
	Parser parser;
	std::shared_ptr<TranslationUnit> baseNode = parser.ParseTokens(lexTokens);
	AsmBuilder builder;
	builder.BuildWasm(baseNode);
}