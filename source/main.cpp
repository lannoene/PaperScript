#include "lexer.h"
#include "parser.h"

int main() {
	Lexer lex("../tests/main.s");
	std::vector<Token> lexTokens = lex.ParseFile();
	Parser parser;
	std::shared_ptr<AstNode> baseNodes = parser.ParseTokens(lexTokens);
}
