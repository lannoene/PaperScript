#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

enum TokenType {
	TOKEN_UNKNOWN,
	TOKEN_DECL_TYPE,
	TOKEN_EXPRESSION,
	TOKEN_DECL_FUNC,
	TOKEN_DECL_VAR,
	TOKEN_LITERAL,
};

class BaseToken {
public:
	int type;
private:
};

class VariableType { // declare a new variable type
public:
	VariableType(std::string typeName) {
		_typeName = typeName;
	}
private:
	bool isGroup = false;
	bool isPrimitive = true;
	std::string _typeName;
	int typeIndex = 0;
};

class Variable : public BaseToken {
public:
	Variable(int type, std::string name);
private:
	
};

class DeclFunction : public BaseToken {
public:
	DeclFunction(std::vector<int> args, std::vector<std::unique_ptr<BaseToken>> contents) {
		argTypes = args;
		tokens = std::move(contents);
	}
	//std::vector<std::unique_ptr<BaseToken>> GetTokens();
private:
	std::vector<std::unique_ptr<BaseToken>> tokens;
	std::vector<int> argTypes;
};

class FunctionCall : public BaseToken {
	
};

class IncludeToken : public BaseToken {
public:
	IncludeToken(std::string inputFile) {
		_inputFile = inputFile;
	}
private:
	std::string _inputFile;
};

/*

#include "file_tree.h"

enum PrimitiveTypes {
	TYPE_INT,
	TYPE_STRING,
	TYPE_BOOL,
	TYPE_FLOAT
};

enum InequalityType {
	INEQ_GREATER,
	INEQ_LESS,
	INEQ_GREATER_EQ,
	INEQ_LESS_EQ,
};

class Group {
public:
	Group(std::string groupName);
	AddMethod();
	AddMemberVar();
private:
	std::vector<Function> methods;
	std::vector<Variable> vars;
};

class Literal : public BaseToken {
	
};

class StringLiteral : public Literal {
public:
	StringLiteral(std::string contents);
};

class IntLiteral : public Literal  {
public:
	IntLiteral(int contents);
};

class FloatLiteral : public Literal  {
public:
	FloatLiteral(float contents);
private:
	
};

class BoolLiteral : public Literal  {
public:
	BoolLiteral(bool contents);
private:
	bool value;
};

// tokens
class Operand {
public:
	Operand(Variable var);
	Operand(Literal var);
private:
	bool isLiteral = false;
	Variable var;
	Literal lit;
};

class Expression : public BaseToken { // has two operands, which evaluate to a prvalue
public:
	
private:
};

class EqualsExpression : public Expression {
	EqualsExpression(Operand var1, Operand var2);
};

class AssignmentExpression : public Expression {
public:
	AssignmentExpression(Operand var1, Operand var2);
};

class InequalityExpression : public Expression {
public:
	InequalityExpression(Operand var1, Operand var2, enum InequalityType type);
};

class AdditionExpression : public Expression {
public:
	AdditionExpression(Operand opr1, Operand opr2);
};*/

class Lexer {
public:
	Lexer(std::string sourceDir);
	void ParseFile();
private:
	size_t place = 0;
	std::string contents;
	int GetKeywordId(std::string keyword);
	std::string FindNextStringLiteral(std::string line, int& start);
	std::shared_ptr<BaseToken> FindNextToken();
	void SkipWhitespace();
};
