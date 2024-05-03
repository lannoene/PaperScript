#pragma once

#include <vector>
#include <memory>

#include "lexer.h"

enum AST_NODE_TYPES {
	NODE_UNKOWN,
	NODE_EXPRESSION,
	NODE_VARIABLE_DECLARATION,
	NODE_VARIABLE_INITIALIZATION,
	NODE_FUNCTION_DECLARATION,
	NODE_FUNCTION_DEFINITION,
	NODE_TRANSLATION_UNIT,
	NODE_IF_STATEMENT,
};

enum EXPRESSION_TYPES {
	EXP_FUNCTION_CALL,
	EXP_LITERAL,
	EXP_VARIABLE,
	EXP_ADDITION,
	EXP_SUBTRACTION,
	EXP_MULTIPLICATION,
	EXP_DIVISION,
	EXP_EXPONENTIATION,
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
	void AddChildNode(std::shared_ptr<AstNode> node) {
		childNodes.push_back(node);
	}
	enum AST_NODE_TYPES GetNodeType() {return type;}
	std::vector<std::shared_ptr<AstNode>>& ChildNodes() {return childNodes;}
private:
	enum AST_NODE_TYPES type;
	std::vector<std::shared_ptr<AstNode>> childNodes;
};

class TranslationUnit : public AstNode {
public:
	TranslationUnit() : AstNode(NODE_TRANSLATION_UNIT) {}
};

/*class FunctionDeclaration : public AstNode {
public:
	FunctionDeclaration(std::vector<enum VARIABLE_TYPES> argTypes) : AstNode(NODE_FUNCTION_DECLARATION) {}
};*/

class FunctionDefinition : public AstNode {
public:
	FunctionDefinition(enum VARIABLE_TYPES t, std::string name, std::vector<std::pair<enum VARIABLE_TYPES, std::string>> args, bool p) : AstNode(NODE_FUNCTION_DEFINITION) {}
private:
	enum VARIABLE_TYPES returnType;
};

class VariableDeclaration : public AstNode {
public:
	VariableDeclaration(enum VARIABLE_TYPES t, std::string name, bool p) : AstNode(NODE_FUNCTION_DEFINITION) {}
private:
	
};

class Expression : public AstNode {
public:
	Expression() : AstNode(NODE_EXPRESSION) {}
	virtual int Evaluate() {return 0;};
private:
};

class IfStatement : public AstNode {
public:
	IfStatement(std::shared_ptr<Expression> cond) : AstNode(NODE_IF_STATEMENT) {condition = cond;}
private:
	std::shared_ptr<Expression> condition;
};

class IntLiteral : public Expression {
public:
	IntLiteral(int i) {value = i;}
	virtual int Evaluate() {return value;};
private:
	int value;
};

class AddExpression : public Expression {
public:
	AddExpression(Expression exp1, Expression exp2) {
		childExp1 = exp1;
		childExp2 = exp2;
	}
	virtual int Evaluate() {return childExp1.Evaluate() + childExp2.Evaluate();};
private:
	Expression childExp1;
	Expression childExp2;
};

class SubtractExpression : public Expression {
public:
	SubtractExpression(Expression exp1, Expression exp2) {
		childExp1 = exp1;
		childExp2 = exp2;
	}
	virtual int Evaluate() {return childExp1.Evaluate() - childExp2.Evaluate();};
private:
	Expression childExp1;
	Expression childExp2;
};

class Token;

class Parser {
public:
	std::shared_ptr<AstNode> ParseTokens(std::vector<Token> ntokens);
private:
	std::vector<Token> tokens;
	size_t place = 0;
	std::shared_ptr<AstNode> ParseFileScope(std::shared_ptr<TranslationUnit>);
	std::shared_ptr<AstNode> ParseFunctionScope();
	std::shared_ptr<AstNode> ParseNewIdentifier();
	std::shared_ptr<FunctionDefinition> ParseFunctionDeclaration(enum VARIABLE_TYPES retTyp, std::string declName);
	Token EatToken(enum TOKEN_TYPES t);
	Token EatTokClass(enum TOKEN_CLASS c);
	bool EatTokenOptional(enum TOKEN_TYPES t);
	enum VARIABLE_TYPES TurnTokTypeToVarType(enum TOKEN_TYPES t);
	void ParseFunctionScope(std::shared_ptr<FunctionDefinition> func);
	void ParseIfStatement(std::shared_ptr<AstNode> parNode);
};