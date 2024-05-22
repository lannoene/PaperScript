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
	
	out += "(memory (export \"memory\") 1)";
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
	out += "(import \"imports\" \"getTime\" (func $time(result i32)))"; // print a single integer
	out += "(import \"graphics\" \"draw\" (func $draw (param i32)(param i32)(param i32)))"; // draw sun
	out += "(import \"graphics\" \"reqNextFrame\" (func $finishDrawing))"; // req next frame
	out += "(import \"graphics\" \"clearScreen\" (func $clearScreen))"; // clear frame
	out += "(import \"graphics\" \"drawRectangle\" (func $__drawRectangle (param i32)(param i32)(param i32)(param i32)))"; // clear frame
	out += "(import \"graphics\" \"setDrawColor\" (func $__setDrawColor (param i32)(param i32)(param i32)))"; // clear frame
	out += "(import \"graphics\" \"drawLine\" (func $__drawLine (param i32)(param i32)(param i32)(param i32)))"; // clear frame
}

void AsmBuilder::HandleGlobalDeclaration(std::shared_ptr<Declaration> gDecl) {
	if (gDecl->isFunc) {
		out += "(func $" + gDecl->ident;
		if (gDecl->ident == "start" || gDecl->ident == "OnKeyDown" || gDecl->ident == "OnKeyUp" || gDecl->ident == "OnClick" || gDecl->ident == "MainLoop") { // our entry point
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
				case TYPE_FLOAT:
					out += "f32";
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
				case TYPE_FLOAT:
					out += "f32";
					break;
			}
			out += ")";
		}
		HandleBlockScope(gDecl->func.childNodes);
		out += ")";
	} else {
		out += "(global $" + gDecl->ident + " (mut ";
		switch (gDecl->typ) {
			default:
				std::cout << "unsupported global variable type" << std::endl;
				exit(1);
				break;
			case TYPE_INT:
				out += "i32";
				break;
			case TYPE_FLOAT:
				out += "f32";
				break;
		}
		out += ")";
		if (gDecl->var.initialValExpr) {
			HandleExpression(gDecl->var.initialValExpr);
		} else {
			out += "(i32.const 0)";
		}
		out += ")";
		globalVars[gDecl->ident] = true;
	}
}

std::string gen_random_str(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

void AsmBuilder::HandleBlockScope(std::vector<std::shared_ptr<AstNode>> nodes/*, std::vector<std::unordered_map<std::string, DeclInfo>> scopedVars*/) {
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
				if (std::static_pointer_cast<ReturnStatement>(node)->expr != nullptr)
					HandleExpression(std::static_pointer_cast<Expression>(node)->child1);
				out += "(return)";
				break;
			case NODE_IF_STATEMENT: {
				std::shared_ptr<IfStatement> ifStat = std::static_pointer_cast<IfStatement>(node);
				HandleExpression(ifStat->condition);
				out += "(if (then ";
				HandleBlockScope(ifStat->childNodes);
				out += ")(else ";
				HandleBlockScope(ifStat->elseChildNodes);
				out += "))";
				break;
			}
			case NODE_DECLARATION:
				out += "(local $" + std::static_pointer_cast<Declaration>(node)->ident + " i32)";
				break;
			case NODE_WHILE_LOOP: {
				std::string loopIdent = gen_random_str(10);
				HandleExpression(std::static_pointer_cast<WhileLoop>(node)->condition);
				out += "(if (then ";
				out += "(loop $" + loopIdent;
				HandleBlockScope(std::static_pointer_cast<WhileLoop>(node)->childNodes);
				HandleExpression(std::static_pointer_cast<WhileLoop>(node)->condition);
				out += "(br_if $" + loopIdent + "))";
				out += "))";
				break;
			}
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
			out += "(call $" + expr->idenName + ")";
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
					break;
			}
			break;
		case EXP_VARIABLE:
			if (globalVars[expr->idenName]) {
				out += "(global.get $" + expr->idenName + ")";
			} else {
				out += "(local.get $" + expr->idenName + ")";
			}
			break;
		case EXP_ADDITION:
			out += "(i32.add ";
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += ")";
			break;
		case EXP_SUBTRACTION:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.sub)";
			break;
		case EXP_MULTIPLICATION:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.mul)";
			break;
		case EXP_DIVISION:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.div_s)";
			break;
		case EXP_EXPONENTIATION:
			break;
		case EXP_ASSIGNMENT:
			if (!std::dynamic_pointer_cast<Expression>(expr->child1)) {
				std::cout << "Could not create assembly for assignment: left hand side is invalid." << std::endl;
				exit(1);
			}
			if (globalVars[std::dynamic_pointer_cast<Expression>(expr->child1)->idenName]) {
				out += "(global.set $" + std::dynamic_pointer_cast<Expression>(expr->child1)->idenName;
			} else {
				out += "(local.set $" + std::dynamic_pointer_cast<Expression>(expr->child1)->idenName;
			}
			HandleExpression(expr->child2);
			out += ")";
			break;
		case EXP_EQUALITY:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.eq)";
			break;
		case EXP_NOT_EQUALITY:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.ne)";
			break;
		case EXP_LESS:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.lt_s)";
			break;
		case EXP_GREATER:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.gt_s)";
			break;
		case EXP_REMAINDER:
			HandleExpression(expr->child1);
			HandleExpression(expr->child2);
			out += "(i32.rem_s)";
			break;
		case EXP_AND:
			HandleExpression(expr->child1);
			out += "(i32.const 0)";
			out += "(i32.ne)";
			HandleExpression(expr->child2);
			out += "(i32.const 0)";
			out += "(i32.ne)";
			out += "(i32.add)";
			out += "(i32.const 2)";
			out += "(i32.eq)";
			break;
		case EXP_OR:
			HandleExpression(expr->child1);
			out += "(i32.const 0)";
			out += "(i32.ne)";
			HandleExpression(expr->child2);
			out += "(i32.const 0)";
			out += "(i32.ne)";
			out += "(i32.add)";
			out += "(i32.const 0)";
			out += "(i32.ne)";
			break;
		case EXP_NEGATIVE:
			HandleExpression(expr->child1);
			out += "(i32.const -1)";
			out += "(i32.mul)";
	}
}