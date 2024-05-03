#include "parser.h"

#include <iostream>

#include "lexer.h"

std::shared_ptr<AstNode> Parser::ParseTokens(std::vector<Token> ntokens) {
	tokens = ntokens;
	std::shared_ptr<TranslationUnit> translationUnit = std::make_shared<TranslationUnit>();;
	translationUnit->AddChildNode(ParseFileScope(translationUnit));
	return translationUnit;
}

std::shared_ptr<AstNode> Parser::ParseFileScope(std::shared_ptr<TranslationUnit> file) {
	if (tokens.size() == 0) {
		std::cout << "no tokens" << std::endl;
		return file;
	}
	while (place < tokens.size()) {
		switch (tokens[place].GetType()) {
			default:
				std::cout << "Error: unexpected token: " << tokens[place].GetIdentifier() << std::endl;
				exit(1);
				break;
			case TOK_PUBLIC:
			case TOK_PRIVATE:
			case TOK_INT:
			case TOK_VOID:
			case TOK_STRING:
				file->ChildNodes().push_back(ParseNewIdentifier());
				break;
		}
	}
	return file;
}

enum VARIABLE_TYPES Parser::TurnTokTypeToVarType(enum TOKEN_TYPES t) {
	enum VARIABLE_TYPES type;
	switch (t) {
		default:
			std::cout << "given type is not a type!" << std::endl;
			exit(1);
			break;
		case TOK_INT:
			type = TYPE_INT;
			break;
		case TOK_STRING:
			type = TYPE_STRING;
			break;
		case TOK_VOID:
			type = TYPE_VOID;
			break;
	}
	return type;
}

Token Parser::EatToken(enum TOKEN_TYPES t) {
	Token tok = tokens[place++];
	if (tok != t) {
		std::cout << "PARSER ERROR: token type does not match up. expected: " << t << " got " << tok.GetType() << " (line " << tok.GetLineNumber() << ")" << std::endl;
		exit(1);
	} else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok;
}

bool Parser::EatTokenOptional(enum TOKEN_TYPES t) {
	Token tok = tokens[place++];
	if (tok != t)
		--place;
	else
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	return tok == t;
}

Token Parser::EatTokClass(enum TOKEN_CLASS c) {
	Token tok = tokens[place++];
	if (tok.GetClass() != c) {
		std::cout << "PARSER ERROR: token class does not match up. expected: " << c << " got " << tok.GetClass() << " (line " << tok.GetLineNumber() << ")" << std::endl;
		exit(1);
	} else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok;
}

std::shared_ptr<AstNode> Parser::ParseNewIdentifier() {
	std::cout << "parsing new identifier: " << tokens[place].GetIdentifier() << std::endl;
	bool pub = false;
	if (EatTokenOptional(TOK_PRIVATE)) {
		pub = false;
	} else if (EatTokenOptional(TOK_PUBLIC)) {
		pub = true;
	}
	
	enum VARIABLE_TYPES typ = TurnTokTypeToVarType(EatTokClass(CLASS_TYPE).GetType());
	
	Token identifyName = EatTokClass(CLASS_IDENTIFIER);
	
	std::shared_ptr<AstNode> ret = nullptr;
	
	switch (tokens[place].GetType()) { // var decl with initalization is not supported yet
		default:
			std::cout << "PARSE ERROR: expected parthesis or semicolon after declaration" << std::endl;
			break;
		case TOK_LPAREN: // function
			ret = ParseFunctionDeclaration(typ, identifyName.GetIdentifier(), pub);
			break;
		case TOK_SEMICOLON: // variable declaration without initialization
			EatToken(TOK_SEMICOLON);
			ret = std::make_shared<VariableDeclaration>(typ, identifyName.GetIdentifier(), pub);
			break;
	}
	
	return ret;
}

std::shared_ptr<FunctionDefinition> Parser::ParseFunctionDeclaration(enum VARIABLE_TYPES retTyp, std::string declName) {
	// parse function params
	EatToken(TOK_LPAREN);
	std::vector<std::pair<enum VARIABLE_TYPES, std::string>> params;
	while (tokens[place].GetType() != TOK_RPAREN) {
		std::cout << "Getting new param" << std::endl;
		Token typeToka = EatTokClass(CLASS_TYPE);
		Token name = EatTokClass(CLASS_IDENTIFIER);
		params.push_back({TurnTokTypeToVarType(typeToka.GetType()), name.GetIdentifier()});
		
		if (!EatTokenOptional(TOK_COMMA))
			break;
	}
	EatToken(TOK_RPAREN);
	std::shared_ptr<FunctionDefinition> ret = std::make_shared<FunctionDefinition>(retTyp, declName, params);
	ParseFunctionScope(ret);
	return ret;
}

void Parser::ParseFunctionScope(std::shared_ptr<FunctionDefinition> func) {
	EatToken(TOK_LBRACE);
	while (!EatTokenOptional(TOK_RBRACE)) {
		Token tok = tokens[place++];
		switch (tok.GetType()) {
			default:
				std::cout << "PARSER ERROR: token '" << tok.GetIdentifier() << "' is not allowed in function scope" << std::endl;
				exit(1);
				break;
			case TOK_IF:
				ParseIfStatement(func);
				break;
			case TOK_IDENTIFIER:
				ParseIdentifier(func);
				break;
		}
	}
}

void Parser::ParseIfStatement(std::shared_ptr<AstNode> parNode) {
	EatToken(TOK_IF);
	
	//EatToken();
}

void Parser::ParseIdentifier(std::shared_ptr<AstNode> parNode) {
	
}