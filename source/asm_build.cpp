#include "asm_build.h"

#include <iostream>
#include <fstream>

void AsmBuilder::BuildWasm(std::shared_ptr<TranslationUnit> file) {
	// generate .wat file
	GenerateWat(file);
	
	std::fstream fout("a.wat", std::ios::out);
	
	if (!fout.is_open()) {
		std::cout << "could not open file a.wat for writing!" << std::endl;
		return;
	}
	
	fout << out;
	
	fout.close();
	
	// build wasm file
#ifdef _WIN32
	system("wat2wasm a.wat -o a.wasm");
#else
	system("./wat2wasm a.wat -o a.wasm");
#endif
}

void AsmBuilder::GenerateWat(std::shared_ptr<TranslationUnit> file) {
	out += "(module";
	AddJsApi();
	
	for (auto global : file->childNodes) {
		switch (global->GetNodeType()) {
			default:
				std::cout << "only declarations are expected in global scope" << std::endl;
				exit(1);
				break;
			case NODE_DECLARATION:
				HandleGlobalDeclaration(std::static_pointer_cast<Declaration>(global));
				break;
		}
	}
	
	out += ")";
}

void AsmBuilder::AddJsApi() {
	out += "(import \"imports\" \"log\" (func $printInt (param i32)))"; // print a single integer
}

void AsmBuilder::HandleGlobalDeclaration(std::shared_ptr<Declaration> gDecl) {
	if (gDecl->isFunc) {
		out += "(func $" + gDecl->ident;
		if (gDecl->ident == "start") { // our entry point
			out += "(export \"" + gDecl->ident + "\")";
		}
		for (auto param : gDecl->func.params) {
			out += "(param $" + param.second + " ";
			switch (param.first) {
				default:
					std::cout << "unsupported parameter type" << std::endl;
					exit(1);
					break;
				case TYPE_INT:
					out += "i32";
					break;
			}
			out += ")";
		}
		if (gDecl->typ != TYPE_VOID) {
			out += "(result ";
			switch (gDecl->typ) {
				default:
					std::cout << "unsupported parameter type" << std::endl;
					exit(1);
					break;
				case TYPE_INT:
					out += "i32";
					break;
			}
			out += ")";
		}
		HandleBlockScope(gDecl->func.childNodes);
		if (gDecl->typ == TYPE_VOID) {
			for (int i = 0; i < curFuncVarsLeft; i++) {
				out += "(drop)";
			}
			curFuncVarsLeft = 0;
		}
		out += ")";
	} else {
		
	}
}

void AsmBuilder::HandleBlockScope(std::vector<std::shared_ptr<AstNode>> nodes) {
	for (auto node : nodes) {
		switch (node->GetNodeType()) {
			default:
				std::cout << "unsupported scoped statement" << std::endl;
				exit(1);
				break;
			case NODE_EXPRESSION:
				HandleExpression(std::static_pointer_cast<Expression>(node));
				break;
			case NODE_RETURN:
				HandleExpression(std::static_pointer_cast<Expression>(node)->child1);
				out += "(return)";
				break;
			case NODE_IF_STATEMENT:
				
				break;
		}
	}
}

void AsmBuilder::HandleExpression(std::shared_ptr<Expression> expr) {
	switch (expr->ExpType()) {
		default:
			std::cout << "unsupported expression" << std::endl;
			exit(1);
			break;	
		case EXP_FUNCTION_CALL: {
			// get params
			int numArgs = 0;
			for (auto arg : std::static_pointer_cast<FunctionCall>(expr)->args) {
				HandleExpression(arg);
				++numArgs;
			}
			curFuncVarsLeft -= numArgs;
			out += "(call $" + expr->idenName + ")";
			std::cout << "Func call " << expr->idenName << std::endl;
			break;
		}
		case EXP_PRIMARY:
			// const
			switch (expr->literalType) {
				default:
					std::cout << "unsupported type" << std::endl;
					break;
				case TYPE_INT:
					out += "(i32.const " + std::to_string(expr->iVal) + ")";
					curFuncVarsLeft += 1;
					break;
			}
			break;
		case EXP_VARIABLE:
			out += "(local.get $" + expr->idenName + ")";
			break;
		case EXP_ADDITION:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.add)";
			curFuncVarsLeft -= 1;
			break;
		case EXP_SUBTRACTION:
			break;
		case EXP_MULTIPLICATION:
			break;
		case EXP_DIVISION:
			break;
		case EXP_EXPONENTIATION:
			break;
		case EXP_ASSIGNMENT:
			break;
		case EXP_EQUALITY:
			break;
		case EXP_NOT_EQUALITY:
			break;
	}
}