#pragma once

#include "parser.h"

class AsmBuilder {
public:
	void BuildWasm(std::shared_ptr<TranslationUnit> file);
private:
	void GenerateWat(std::shared_ptr<TranslationUnit> file);
	void AddJsApi();
	void HandleGlobalDeclaration(std::shared_ptr<Declaration> gDecl);
	void HandleBlockScope(std::vector<std::shared_ptr<AstNode>> nodes);
	void HandleExpression(std::shared_ptr<Expression> expr);
	int curFuncVarsLeft = 0;
	std::string out;
};