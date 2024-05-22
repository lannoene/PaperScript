#pragma once

#include <vector>
#include <memory>

#include "lexer.h"

enum AST_NODE_TYPES {
	NODE_UNKOWN,
	NODE_EXPRESSION,
	NODE_DECLARATION,
	NODE_TRANSLATION_UNIT,
	NODE_IF_STATEMENT,
	NODE_WHILE_LOOP,
	NODE_RETURN,
};

enum expression_types {
	EXP_ERROR,
	EXP_FUNCTION_CALL,
	EXP_PRIMARY,
	EXP_VARIABLE,
	EXP_ADDITION,
	EXP_SUBTRACTION,
	EXP_MULTIPLICATION,
	EXP_DIVISION,
	EXP_EXPONENTIATION,
	EXP_ASSIGNMENT,
	EXP_EQUALITY,
	EXP_NOT_EQUALITY,
	EXP_LESS,
	EXP_GREATER,
	EXP_REMAINDER,
	EXP_AND,
	EXP_OR,
	EXP_NEGATIVE,
};

enum VARIABLE_TYPES {
	TYPE_INT,
	TYPE_VOID,
	TYPE_BOOL,
	TYPE_FLOAT,
	TYPE_STRING,
};

class AstNode {
public:
	AstNode(enum AST_NODE_TYPES ntype) {
		type = ntype;
	}
	enum AST_NODE_TYPES GetNodeType() {return type;}
private:
	enum AST_NODE_TYPES type;
};

class TranslationUnit : public AstNode {
public:
	TranslationUnit() : AstNode(NODE_TRANSLATION_UNIT) {}
	void AddChildNode(std::shared_ptr<AstNode> c) {
		childNodes.push_back(c);
	}
	std::vector<std::shared_ptr<AstNode>> childNodes;
};

class Expression : public AstNode {
public:
	Expression(enum expression_types t) : AstNode(NODE_EXPRESSION) {
		expType = t;
	}
	Expression(enum expression_types t, std::shared_ptr<Expression> e1) : Expression(t) {
		child1 = e1;
	}
	Expression(enum expression_types t, std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) : Expression(t, e1) {
		child2 = e2;
	}
	Expression(enum expression_types t, std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2, std::shared_ptr<Expression> e3) : Expression(t, e1, e2) {
		child3 = e3;
	}
	std::shared_ptr<Expression> child1;
	std::shared_ptr<Expression> child2;
	std::shared_ptr<Expression> child3;
	std::string idenName;
	int iVal;
	float fVal;
	std::string sVal;
	enum VARIABLE_TYPES varType;
	enum VARIABLE_TYPES literalType;
	enum expression_types ExpType() {return expType;}
private:
	enum expression_types expType;
};

class Declaration : public AstNode {
public:
	enum VARIABLE_TYPES typ;
	std::string ident;
	bool isFunc, isPublic;
	// var
	Declaration(enum VARIABLE_TYPES t, std::string i, bool p) : AstNode(NODE_DECLARATION) {
		isFunc = false;
		typ = t;
		ident = i;
		isPublic = p;
	}
	struct {
		//std::vector<std::shared_ptr<Expression> initialExp;
		std::shared_ptr<Expression> initialValExpr = nullptr;
	} var;
	// func
	Declaration(enum VARIABLE_TYPES t, std::string i, bool p, std::vector<std::pair<enum VARIABLE_TYPES, std::string>> e, std::vector<std::shared_ptr<AstNode>> n) : Declaration(t, i, p) {
		func.params = e;
		func.childNodes = n;
		isFunc = true;
	}
	struct {
		std::vector<std::pair<enum VARIABLE_TYPES, std::string>> params;
		std::vector<std::shared_ptr<AstNode>> childNodes;
	} func;
	
private:
	
};

class FunctionCall : public Expression {
public:
	FunctionCall(std::vector<std::shared_ptr<Expression>> a) : Expression(EXP_FUNCTION_CALL) {
		args = a;
	}
	std::vector<std::shared_ptr<Expression>> args;
	bool valueUsed = true;
private:
};

class IfStatement : public AstNode {
public:
	IfStatement(std::shared_ptr<Expression> cond) : AstNode(NODE_IF_STATEMENT) {condition = cond;}
	
	bool hasElse = false;
	std::vector<std::shared_ptr<AstNode>> childNodes;
	std::shared_ptr<Expression> condition;
	std::vector<std::shared_ptr<AstNode>> elseChildNodes;
private:
};

class WhileLoop : public AstNode {
public:
	WhileLoop(std::shared_ptr<Expression> cond) : AstNode(NODE_WHILE_LOOP) {condition = cond;}
	
	std::vector<std::shared_ptr<AstNode>> childNodes;
	std::shared_ptr<Expression> condition;
};

class ReturnStatement : public AstNode {
public:
	ReturnStatement(std::shared_ptr<Expression> e) : AstNode(NODE_RETURN) {expr = e;}
	std::shared_ptr<Expression> expr;
private:
};

class Parser {
public:
	std::shared_ptr<TranslationUnit> ParseTokens(std::vector<Token> ntokens);
private:
	std::vector<Token> tokens;
	size_t place = 0;
	std::shared_ptr<TranslationUnit> ParseFileScope();
	std::shared_ptr<AstNode> ParseFunctionScope();
	std::shared_ptr<AstNode> ParseGlobalIdentifier();
	std::shared_ptr<Declaration> ParseFunctionDeclaration(enum VARIABLE_TYPES retTyp, std::string declName, bool pub);
	Token EatToken(enum token_types t);
	Token EatTokClass(enum token_class c);
	bool EatTokenOptional(enum token_types t);
	enum VARIABLE_TYPES TurnTokTypeToVarType(enum token_types t);
	std::shared_ptr<AstNode> ParseIfStatement();
	std::vector<std::shared_ptr<AstNode>> ParseBlockScope();
	std::vector<Token> GetTokensInsideDelimiters(enum token_types start, enum token_types ending);
	std::shared_ptr<Expression> ParseScopedIdentifier();
	std::shared_ptr<Expression> ParseExpression();
	std::shared_ptr<Expression> CreateExpression();
	std::shared_ptr<Expression> ParseAssignmentExpression();
	std::shared_ptr<Expression> ParsePrimaryExpression();
	std::shared_ptr<Expression> ParseAdditiveExpression();
	std::vector<std::shared_ptr<Expression>> ParseFunctionInputs();
	std::shared_ptr<Expression> ParseMultiplyExpression();
	std::shared_ptr<Expression> ParseEqualityExpression();
	std::shared_ptr<Expression> ParseInequalityExpression();
	std::shared_ptr<Expression> ParseAndExpression();
	std::shared_ptr<Expression> ParseUrnaryExpression();
	std::shared_ptr<Expression> ParseOrExpression();
	std::shared_ptr<AstNode> ParseWhileLoop();
	Token GetNextToken();
	void AstPrintAst(std::shared_ptr<AstNode> parNode, int vPlace);
	void AstPrintExpression(std::shared_ptr<Expression> parNode, int vPlace);
	void AstPrintDeclaration(std::shared_ptr<Declaration> parNode, int vPlace);
	void AstPrintIfStatement(std::shared_ptr<IfStatement> parNode, int vPlace);
};
