#include "parser.h"

#include <iostream>

#include "lexer.h"

std::shared_ptr<TranslationUnit> Parser::ParseTokens(std::vector<Token> ntokens) {
	tokens = ntokens;
	std::shared_ptr<TranslationUnit> translationUnit = ParseFileScope();
	std::cout << "finished parsing" << std::endl;
	
	for (auto d : translationUnit->childNodes) {
		AstPrintAst(d, 0);
	}
	return translationUnit;
}

std::shared_ptr<TranslationUnit> Parser::ParseFileScope() {
	std::shared_ptr<TranslationUnit> file = std::make_shared<TranslationUnit>();
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
				file->AddChildNode(ParseGlobalIdentifier());
				break;
			case TOK_INCLUDE: {
				EatToken(TOK_INCLUDE);
				std::string incPath = EatToken(TOK_STRING_VALUE).GetIdentifier();
				Lexer lex(incPath);
				std::vector<Token> lexTokens = lex.ParseFile();
				Parser parser;
				std::shared_ptr<TranslationUnit> baseNode = parser.ParseTokens(lexTokens);
				for (auto node : baseNode->childNodes) {
					file->AddChildNode(node);
				}
				EatToken(TOK_SEMICOLON);
				break;
			}
		}
	}
	return file;
}

enum VARIABLE_TYPES Parser::TurnTokTypeToVarType(enum token_types t) {
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
		case TOK_FLOAT:
			type = TYPE_FLOAT;
			break;
	}
	return type;
}

Token Parser::EatToken(enum token_types t) {
	Token tok = tokens[place++];
	if (tok != t) {
		std::cout << "PARSER ERROR: token type does not match up. expected: " << t << " ('" << tokIden[t] << "')" << " got " << tok.GetType() << " (found tok: " << tok.GetIdentifier() << " line " << tok.GetLineNumber() << ")" << std::endl;
		exit(1);
	} else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok;
}

bool Parser::EatTokenOptional(enum token_types t) {
	Token tok = tokens[place++];
	if (tok != t)
		--place;
	else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok == t;
}

Token Parser::EatTokClass(enum token_class c) {
	Token tok = tokens[place++];
	if (tok.GetClass() != c) {
		std::cout << "PARSER ERROR: token class does not match up. expected: " << c << " got " << tok.GetClass() << " (line " << tok.GetLineNumber() << ")" << std::endl;
		exit(1);
	} else {
		std::cout << "Eating token " << tok.GetIdentifier() << std::endl;
	}
	return tok;
}

Token Parser::GetNextToken() {
	return tokens[place++];
}

std::shared_ptr<AstNode> Parser::ParseGlobalIdentifier() {
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
			std::cout << "PARSE ERROR: expected parthesis, semicolon, or equals after declaration" << std::endl;
			break;
		case TOK_LPAREN: // function
			ret = ParseFunctionDeclaration(typ, identifyName.GetIdentifier(), pub);
			break;
		case TOK_SEMICOLON: // variable declaration without initialization
			EatToken(TOK_SEMICOLON);
			ret = std::make_shared<Declaration>(typ, identifyName.GetIdentifier(), pub);
			break;
		case TOK_EQUALS:
			EatToken(TOK_EQUALS);
			ret = std::make_shared<Declaration>(typ, identifyName.GetIdentifier(), pub);
			std::static_pointer_cast<Declaration>(ret)->var.initialValExpr = ParseExpression();
			EatToken(TOK_SEMICOLON);
			break;
	}
	
	return ret;
}

std::shared_ptr<Declaration> Parser::ParseFunctionDeclaration(enum VARIABLE_TYPES retTyp, std::string declName, bool pub) {
	// parse function params
	EatToken(TOK_LPAREN);
	std::vector<std::pair<enum VARIABLE_TYPES, std::string>> params;
	while (tokens[place] != TOK_RPAREN) {
		std::cout << "Getting new param" << std::endl;
		Token typeToka = EatTokClass(CLASS_TYPE);
		Token name = EatTokClass(CLASS_IDENTIFIER);
		params.push_back({TurnTokTypeToVarType(typeToka.GetType()), name.GetIdentifier()});
		
		if (!EatTokenOptional(TOK_COMMA))
			break;
	}
	EatToken(TOK_RPAREN);
	std::shared_ptr<Declaration> ret = std::make_shared<Declaration>(retTyp, declName, pub, params, ParseBlockScope());
	return ret;
}

std::shared_ptr<AstNode> Parser::ParseIfStatement() {
	EatToken(TOK_IF);
	
	EatToken(TOK_LPAREN);
	
	std::shared_ptr<Expression> ifExp = ParseExpression();
	
	EatToken(TOK_RPAREN);
	
	std::shared_ptr<IfStatement> ifStat = std::make_shared<IfStatement>(ifExp);
	
	if (EatTokenOptional(TOK_LBRACE)) {
		place--;
		ifStat->childNodes = ParseBlockScope();
	} else {
		if (EatTokenOptional(TOK_IF)) {
			place--;
			ifStat->childNodes = std::vector<std::shared_ptr<AstNode>>{ParseIfStatement()};
		} else {
			ifStat->childNodes = std::vector<std::shared_ptr<AstNode>>{ParseExpression()};
			EatToken(TOK_SEMICOLON);
		}
	}
	
	if (EatTokenOptional(TOK_ELSE)) {
		ifStat->hasElse = true;
		if (EatTokenOptional(TOK_LBRACE)) {
			place--;
			ifStat->elseChildNodes = ParseBlockScope();
		} else {
			if (EatTokenOptional(TOK_IF)) {
				place--;
				ifStat->elseChildNodes = std::vector<std::shared_ptr<AstNode>>{ParseIfStatement()};
			} else {
				ifStat->elseChildNodes = std::vector<std::shared_ptr<AstNode>>{ParseExpression()};
				EatToken(TOK_SEMICOLON);
			}
		}
	}
	
	return ifStat;
}

std::shared_ptr<AstNode> Parser::ParseWhileLoop() {
	EatToken(TOK_WHILE);
	EatToken(TOK_LPAREN);
	
	std::shared_ptr<Expression> condExp = ParseExpression();
	
	EatToken(TOK_RPAREN);
	
	std::shared_ptr<WhileLoop> wLoop = std::make_shared<WhileLoop>(condExp);
	
	wLoop->childNodes = ParseBlockScope();
	
	return wLoop;
}

std::vector<Token> Parser::GetTokensInsideDelimiters(enum token_types start, enum token_types ending) {
	EatTokenOptional(start);
	
	std::vector<Token> toks;
	int numEndingToksNeeded = 0;
	
	while (true) {
		if (tokens[place] == start) {
			++numEndingToksNeeded;
		} else if (tokens[place] == ending) {
			if (numEndingToksNeeded > 0)
				--numEndingToksNeeded;
			else
				break;
		}
		toks.push_back(tokens[place]);
		++place;
	}
	EatToken(ending);
	
	return toks;
}

std::vector<std::shared_ptr<AstNode>> Parser::ParseBlockScope() {
	std::vector<std::shared_ptr<AstNode>> ret;
	EatToken(TOK_LBRACE);
	while (!EatTokenOptional(TOK_RBRACE)) {
		Token tok = tokens[place];
		switch (tok.GetType()) {
			default:
				std::cout << "PARSER ERROR: token '" << tok.GetIdentifier() << "' is not allowed in block scope (line: " << tok.GetLineNumber() << ")" << std::endl;
				exit(1);
				break;
			case TOK_IF:
				std::cout << "found if" << std::endl;
				ret.push_back(ParseIfStatement());
				
				//std::cout << "stat len: " << std::static_pointer_cast<IfStatement>(ret.back())->elseChildNodes.size() << std::endl;
				//exit(1);
				break;
			case TOK_IDENTIFIER: {
				std::shared_ptr<Expression> expr = ParseExpression();
				if (!expr) {
					std::cout << "PARSER ERROR: could not parse expression. The best error info I can give you is this: (line: " << tok.GetLineNumber() << std::endl;
					exit(1);
				} else {
					ret.push_back(expr);
				}
				EatToken(TOK_SEMICOLON);
				break;
			}
			case TOK_INT:
			case TOK_STRING: {
				enum VARIABLE_TYPES typ = TurnTokTypeToVarType(EatTokClass(CLASS_TYPE).GetType());
				std::string varName = EatToken(TOK_IDENTIFIER).GetIdentifier();
				ret.insert(ret.begin(), std::make_shared<Declaration>(typ, varName, false));
				if (EatTokenOptional(TOK_EQUALS)) {
					//std::static_pointer_cast<Declaration>(ret.front())->var.initialValExpr = ParseExpression();
					std::shared_ptr<Expression> var = std::make_shared<Expression>(EXP_VARIABLE);
					var->idenName = varName;
					ret.push_back(std::make_shared<Expression>(EXP_ASSIGNMENT, var, ParseExpression()));
				}
				EatToken(TOK_SEMICOLON);
				
				
				break;
			}
			case TOK_RETURN:
				EatToken(TOK_RETURN);
				ret.push_back(std::make_shared<ReturnStatement>(ParseExpression()));
				EatToken(TOK_SEMICOLON);
				break;
			case TOK_WHILE:
				ret.push_back(ParseWhileLoop());
				break;
		}
		std::cout << "curTok " << tok.GetIdentifier() << std::endl;
	}
	return ret;
}

std::shared_ptr<Expression> Parser::ParsePrimaryExpression() {
	std::cout << "in prim" << std::endl;
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_INT_VALUE: {
			EatToken(TOK_INT_VALUE);
			std::shared_ptr<Expression> e = std::make_shared<Expression>(EXP_PRIMARY);
			e->iVal = std::stoi(tok.GetIdentifier());
			e->literalType = TYPE_INT;
			return e;
		}
		case TOK_STRING_VALUE: {
			std::shared_ptr<Expression> expr = std::make_shared<Expression>(EXP_PRIMARY);
			expr->sVal = tok.GetIdentifier();
			expr->literalType = TYPE_STRING;
			EatToken(TOK_STRING_VALUE);
			return expr;
		}
		case TOK_IDENTIFIER: {
			std::string ident = tok.GetIdentifier();
			EatToken(TOK_IDENTIFIER);
			if (EatTokenOptional(TOK_LPAREN)) { // func call
				// parse inputs
				std::vector<std::shared_ptr<Expression>> params = ParseFunctionInputs();
				std::shared_ptr<FunctionCall> fCall = std::make_shared<FunctionCall>(params);
				fCall->idenName = ident;
				return fCall;
			} else if (EatTokenOptional(TOK_LBRACKET)) { // array index
				//std::shared_ptr<Expression> idxExpr = ParseExpression();
				
			} else { // var call
				std::shared_ptr<Expression> var = std::make_shared<Expression>(EXP_VARIABLE);
				var->idenName = ident;
				return var;
			}
		}
		case TOK_LPAREN: {
			EatToken(TOK_LPAREN);
			std::shared_ptr<Expression> expr = ParseExpression();
			EatToken(TOK_RPAREN);
			return expr;
		}
		default: {
			std::cout << "RETURNING NULL" << std::endl;
			return nullptr;
		}
	}
}

std::shared_ptr<Expression> Parser::ParseUrnaryExpression() {
	switch (tokens[place].GetType()) {
		default:
			return ParsePrimaryExpression();
		case TOK_MINUS:
			EatToken(TOK_MINUS);
			return std::make_shared<Expression>(EXP_NEGATIVE, ParsePrimaryExpression());
		case TOK_NOT: {
			EatToken(TOK_NOT);
			std::shared_ptr<Expression> e = std::make_shared<Expression>(EXP_PRIMARY);
			e->iVal = 0;
			e->literalType = TYPE_INT;
			return std::make_shared<Expression>(EXP_EQUALITY, ParsePrimaryExpression(), e);
		}
	}
}

std::shared_ptr<Expression> Parser::ParseMultiplyExpression() {
	std::shared_ptr<Expression> lnode = ParseUrnaryExpression();
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_STAR:
			EatToken(TOK_STAR);
			return std::make_shared<Expression>(EXP_MULTIPLICATION, lnode, ParseMultiplyExpression());
		case TOK_FSLASH:
			EatToken(TOK_FSLASH);
			return std::make_shared<Expression>(EXP_DIVISION, lnode, ParseMultiplyExpression());
		case TOK_REMAINDER:
			EatToken(TOK_REMAINDER);
			return std::make_shared<Expression>(EXP_REMAINDER, lnode, ParseMultiplyExpression());
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseAdditiveExpression() {
	std::shared_ptr<Expression> lnode = ParseMultiplyExpression();
	
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_PLUS:
			EatToken(TOK_PLUS);
			return std::make_shared<Expression>(EXP_ADDITION, lnode, ParseAdditiveExpression());
		case TOK_MINUS:
			EatToken(TOK_MINUS);
			return std::make_shared<Expression>(EXP_SUBTRACTION, lnode, ParseAdditiveExpression());
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseInequalityExpression() {
	std::shared_ptr<Expression> lnode = ParseAdditiveExpression();
	
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_GREATER:
			EatToken(TOK_GREATER);
			return std::make_shared<Expression>(EXP_GREATER, lnode, ParseAdditiveExpression());
		case TOK_LESS:
			EatToken(TOK_LESS);
			return std::make_shared<Expression>(EXP_LESS, lnode, ParseAdditiveExpression());
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseEqualityExpression() {
	std::shared_ptr<Expression> lnode = ParseInequalityExpression();
	
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_EQUALITY:
			EatToken(TOK_EQUALITY);
			return std::make_shared<Expression>(EXP_EQUALITY, lnode, ParseInequalityExpression());
		case TOK_NOT_EQUALITY:
			EatToken(TOK_NOT_EQUALITY);
			return std::make_shared<Expression>(EXP_NOT_EQUALITY, lnode, ParseInequalityExpression());
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseAndExpression() {
	std::shared_ptr<Expression> lnode = ParseEqualityExpression();
	
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_AND:
			EatToken(TOK_AND);
			return std::make_shared<Expression>(EXP_AND, lnode, ParseAndExpression());
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseOrExpression() {
	std::shared_ptr<Expression> lnode = ParseAndExpression();
	
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_OR:
			EatToken(TOK_OR);
			return std::make_shared<Expression>(EXP_OR, lnode, ParseOrExpression());
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseAssignmentExpression() {
	std::shared_ptr<Expression> lnode = ParseOrExpression();
	
	if (!lnode)
		return nullptr;
	
	Token tok;
	switch ((tok = tokens[place]).GetType()) {
		case TOK_EQUALS:
			EatToken(TOK_EQUALS);
			return std::make_shared<Expression>(EXP_ASSIGNMENT, lnode, ParseAssignmentExpression());
		case TOK_PLUS_EQ:
			EatToken(TOK_PLUS_EQ);
			return std::make_shared<Expression>(EXP_ASSIGNMENT, lnode, std::make_shared<Expression>(EXP_ADDITION, lnode, ParseOrExpression()));
		default:
			return lnode;
	}
}

std::shared_ptr<Expression> Parser::ParseExpression() {
	std::shared_ptr<Expression> e = ParseAssignmentExpression();
	//EatTokenOptional(TOK_SEMICOLON);
	return e;
}

std::vector<std::shared_ptr<Expression>> Parser::ParseFunctionInputs() {
	EatTokenOptional(TOK_LPAREN);
	
	std::vector<std::shared_ptr<Expression>> ret;
	while (!EatTokenOptional(TOK_RPAREN)) {
		ret.push_back(ParseExpression());
		if (!EatTokenOptional(TOK_COMMA))
			break;
	}
	if (tokens[place] == TOK_RPAREN)
		EatToken(TOK_RPAREN);
	return ret;
}

void VPlacePrint(int vPlace) {
	for (int i = 0; i < vPlace; i++) {
		printf("-");
	}
	printf("|");
}

void Parser::AstPrintAst(std::shared_ptr<AstNode> parNode, int vPlace) {
	switch (parNode->GetNodeType()) {
		default:
			printf("Cannot print node! Type: %d\n", parNode->GetNodeType());
			break;
		case NODE_EXPRESSION:
			AstPrintExpression(std::static_pointer_cast<Expression>(parNode), vPlace + 1);
			break;
		case NODE_IF_STATEMENT:
			AstPrintIfStatement(std::static_pointer_cast<IfStatement>(parNode), vPlace + 1);
			break;
		case NODE_DECLARATION:
			AstPrintDeclaration(std::static_pointer_cast<Declaration>(parNode), vPlace + 1);
			break;
		case NODE_RETURN:
			VPlacePrint(vPlace);
			printf("Return:\n");
			if (std::static_pointer_cast<ReturnStatement>(parNode)->expr != nullptr)
				AstPrintExpression(std::static_pointer_cast<ReturnStatement>(parNode)->expr, vPlace + 1);
			break;
		case NODE_WHILE_LOOP:
			
			break;
	}
}

void Parser::AstPrintExpression(std::shared_ptr<Expression> parNode, int vPlace) {
	switch (parNode->ExpType()) {
		default:
			printf("Cannot print expression! type: %d\n", parNode->ExpType());
			break;
		case EXP_ADDITION:
			VPlacePrint(vPlace);
			printf("Addition:(\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			VPlacePrint(vPlace);
			printf(")\n");
			break;
		case EXP_SUBTRACTION:
			VPlacePrint(vPlace);
			printf("Subtraction:\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			VPlacePrint(vPlace);
			printf(")\n");
			break;
		case EXP_MULTIPLICATION:
			VPlacePrint(vPlace);
			printf("Multiplication:(\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			VPlacePrint(vPlace);
			printf(")\n");
			break;
		case EXP_DIVISION:
			VPlacePrint(vPlace);
			printf("Division:(\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			VPlacePrint(vPlace);
			printf(")\n");
			break;
		case EXP_ASSIGNMENT:
			VPlacePrint(vPlace);
			printf("Assignment:(\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			VPlacePrint(vPlace);
			printf(")\n");
			break;
		case EXP_PRIMARY:
			switch (parNode->literalType) {
				case TYPE_INT:
					VPlacePrint(vPlace);
					printf("Int: %d\n", parNode->iVal);
					break;
				case TYPE_STRING:
					VPlacePrint(vPlace);
					std::cout << "Str: " << parNode->sVal << std::endl;
					break;
			}
			break;
		case EXP_EQUALITY:
			VPlacePrint(vPlace);
			printf("Equality:\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			break;
		case EXP_FUNCTION_CALL:
			VPlacePrint(vPlace);
			printf("fcall:\n");
			VPlacePrint(vPlace);
			std::cout << parNode->idenName << std::endl;
			VPlacePrint(vPlace);
			printf("args:\n");
			for (auto arg : std::static_pointer_cast<FunctionCall>(parNode)->args) {
				AstPrintExpression(arg, vPlace + 1);
			}
			break;
		case EXP_VARIABLE:
			VPlacePrint(vPlace);
			std::cout << "Variable: " << parNode->idenName << std::endl;
			break;
		case EXP_LESS:
			VPlacePrint(vPlace);
			printf("Less than:\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			break;
		case EXP_GREATER:
			VPlacePrint(vPlace);
			printf("Greater than:\n");
			AstPrintExpression(parNode->child1, vPlace + 1);
			AstPrintExpression(parNode->child2, vPlace + 1);
			break;
		case EXP_NEGATIVE:
			std::cout << "neg expression" << std::endl;
			AstPrintExpression(parNode->child1, vPlace + 1);
			break;
	}
}

void Parser::AstPrintDeclaration(std::shared_ptr<Declaration> parNode, int vPlace) {
	if (parNode->isFunc) {
		VPlacePrint(vPlace);
		printf("Function decl:\n");
		VPlacePrint(vPlace);
		std::cout << parNode->ident << " (";
		for (auto param : parNode->func.params) {
			std::cout << param.second << ", ";
		}
		std::cout << ")" << std::endl;
		for (auto childNode : parNode->func.childNodes) {
			AstPrintAst(std::static_pointer_cast<AstNode>(childNode), vPlace);
		}
	} else {
		VPlacePrint(vPlace);
		printf("Variable decl:\n");
		VPlacePrint(vPlace);
		std::cout << parNode->ident << std::endl;
	}
}

void Parser::AstPrintIfStatement(std::shared_ptr<IfStatement> parNode, int vPlace) {
	VPlacePrint(vPlace);
	printf("If cond:\n");
	AstPrintExpression(parNode->condition, vPlace + 1);
	VPlacePrint(vPlace);
	printf("Then:\n");
	for (auto child : parNode->childNodes) {
		AstPrintAst(child, vPlace);
	}
	
	if (parNode->hasElse) {
		VPlacePrint(vPlace);
		printf("Else:\n");
		std::cout << "l length: " << parNode->elseChildNodes.size() << std::endl;
		for (auto child : parNode->elseChildNodes) {
			AstPrintAst(child, vPlace);
		}
	}
}