#include "parser.h"

#include <iostream>

#include "lexer.h"

std::vector<std::shared_ptr<AstNode>> Parser::ParseTokens(std::vector<Token> ntokens) {
	tokens = ntokens;
	std::vector<std::shared_ptr<AstNode>> baseNode;
	ParseFileScope(baseNode);
	return baseNode;
}

void Parser::ParseFileScope(std::vector<std::shared_ptr<AstNode>> baseNode) {
	if (tokens.size() == 0) {
		std::cout << "no tokens" << std::endl;
		return;
	}
	while (place < tokens.size()) {
		switch (tokens[place].GetType()) {
			default:
				std::cout << "Error: unexpected token: " << tokens[place].GetIdentifier() << std::endl;
				break;
			case TOK_INT: // intrinsic private
				ParseNewIdentifier(baseNode);
				break;
			case TOK_VOID:
				ParseNewIdentifier(baseNode);
				break;
			case TOK_STRING: // intrinsic private
				ParseNewIdentifier(baseNode);
				break;
			case TOK_PUBLIC:
				ParseNewIdentifier(baseNode);
				break;
			case TOK_PRIVATE:
				ParseNewIdentifier(baseNode);
				break;
		}
		place++;
	}
}

enum TYPES {
	INT,
	STRING,
	VOID,
};

enum TYPES TurnTokTypeToVarType(enum TOKEN_TYPES t) {
	enum TYPES type;
	switch (t) {
		default:
			std::cout << "given type is not a type!" << std::endl;
			exit(1);
			break;
		case TOK_INT:
			type = INT;
			break;
		case TOK_STRING:
			type = STRING;
			break;
		case TOK_VOID:
			type = VOID;
			break;
	}
	return type;
}

Token Parser::EatToken(enum TOKEN_TYPES t) {
	Token tok = tokens[++place];
	if (tok.GetType() != t) {
		std::cout << "PARSER ERROR: token type does not match up. expected: " << t << " got " << tok.GetType() << std::endl;
		exit(1);
	} else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok;
}

Token Parser::EatTokClass(enum TOKEN_CLASS c) {
	Token tok = tokens[++place];
	if (tok.GetClass() != c) {
		std::cout << "PARSER ERROR: token class does not match up. expected: " << c << " got " << tok.GetClass() << std::endl;
		exit(1);
	} else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok;
}

void Parser::ParseNewIdentifier(std::vector<std::shared_ptr<AstNode>> baseNode) {
	std::shared_ptr<AstNode> identifier;
	bool external = false;
	bool type = INT;
	switch (tokens[place].GetType()) {
		default:
			std::cout << "COULDNT" << std::endl;
			break;
		case TOK_INT: // intrinsic private
			type = INT;
			break;
		case TOK_STRING: // intrinsic private
			type = STRING;
			break;
		case TOK_VOID:
			type = VOID;
			break;
		case TOK_PUBLIC:
			external = true;
		case TOK_PRIVATE: {
			Token typeToka = EatTokClass(CLASS_TYPE);
			type = TurnTokTypeToVarType(typeToka.GetType());
			break;
		}
	}
	Token typeTok = EatToken(TOK_IDENTIFIER);
	std::string idenName = typeTok.GetIdentifier();
	std::cout << "new variable decl (" << idenName << ")" << std::endl;
	if (tokens[place + 1].GetType() == TOK_LPAREN) {
		std::cout << "is func" << std::endl;
		place++;
		// parse function params
		std::vector<std::pair<enum TYPES, std::string>> params;
		if (tokens[place + 1].GetType() != TOK_RPAREN) {
			do {
				std::cout << "Getting new param" << std::endl;
				Token typeToka = EatTokClass(CLASS_TYPE);
				Token name = EatTokClass(CLASS_IDENTIFIER);
				params.push_back({TurnTokTypeToVarType(typeToka.GetType()), name.GetIdentifier()});
			} while (tokens[place + 1].GetType() != TOK_RPAREN && EatToken(TOK_COMMA).GetType());
		}
		EatToken(TOK_RPAREN);
		EatToken(TOK_LBRACE);
		//std::cout << "TOKEN " << tokens[place].GetIdentifier() << std::endl;
		ParseNewScope(baseNode);
	} else {
		EatToken(TOK_SEMICOLON);
	}
}

void Parser::ParseNewScope(std::vector<std::shared_ptr<AstNode>> baseNode) {
	while (place < tokens.size()) {
		switch (tokens[place].GetType()) {
			default:
				std::cout << "Error: unexpected token (func scope): " << tokens[place].GetIdentifier() << std::endl;
				break;
			case TOK_INT: // intrinsic private
				ParseNewIdentifier(baseNode);
				break;
			case TOK_VOID:
				ParseNewIdentifier(baseNode);
				break;
			case TOK_STRING: // intrinsic private
				ParseNewIdentifier(baseNode);
				break;
			case TOK_PUBLIC:
				ParseNewIdentifier(baseNode);
				break;
			case TOK_PRIVATE:
				ParseNewIdentifier(baseNode);
				break;
			case TOK_RBRACE:
				std::cout << "leaving scope" << std::endl;
				place++;
				return;
			case TOK_IDENTIFIER:
				switch (tokens[place + 1].GetType()) {
					case TOK_LPAREN: {
						EatToken(TOK_LPAREN);
						std::vector<Token> callParams;
						while (tokens[place + 1].GetType() != TOK_RPAREN) {
							if (tokens[place + 1].GetClass() == CLASS_IDENTIFIER || tokens[place + 1].GetClass() == CLASS_LITERAL) {
								std::cout << "Function input: " << tokens[place + 1].GetIdentifier() << std::endl;
								place++;
							}
							if (tokens[place + 1].GetType() != TOK_RPAREN)
								EatToken(TOK_COMMA);
						}
						EatToken(TOK_RPAREN);
						EatToken(TOK_SEMICOLON);
						break;
					}
					case TOK_EQUALS:
						EatToken(TOK_EQUALS);
						ParseNextExpression(baseNode);
						break;
				}
				break;
			case TOK_IF: {
				EatToken(TOK_LPAREN);
				std::vector<Token> ifExpression;
				while (token[++place].GetType() != TOK_RPAREN) {
					ifExpression.push_back(token[place]);
				}
				AstEvalExpression(ifExpression, baseNode);
				
				EatToken(TOK_RPAREN);
				EatToken(TOK_LBRACE);
				ParseNewScope(baseNode);
				break;
			}
		}
		place++;
	}
}

void Parser::ParseBlockScope(std::vector<std::shared_ptr<AstNode>> baseNode) {
	
}

void Parser::ParseNextExpression(std::vector<std::shared_ptr<AstNode>> baseNode) {
	
}