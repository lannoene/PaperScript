#pragma once

#include <unordered_map>

#include "parser.h"

struct __StackVarInfo {
	
};

struct DeclInfo {
	enum VARIABLE_TYPES retType;
	std::string name;
	std::vector<std::pair<enum VARIABLE_TYPES, std::string>> params;
	bool isFunc;
	bool isPublic;
	bool isGlobal;
	bool exists = false; // gets set to true
};

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
	std::unordered_map<std::string, bool> globalVars;
};