#pragma once
#ifndef JUALANG_TYPES
#define JUALANG_TYPES




#define DFACTION_TYPES
#define DFACTION_STATES

#include <string>
#include <variant>

enum DFActionState {
	START, EXPR, IDENTER, FUNC, IDENT_EXPR, EXPR_OPS, EXPR_PARA, FUNC_HANDLER, RETURN_HANDLER, WHILE_HANDLER,
	WHILE_SCOPE, IF_HANDLER, IF_SCOPE, SCOPE_HANDLER, FUNCTION_DEF_HANDLER, FUNCTION_DEF_IDENT, FUNCTION_PARA_HANDLER,
	FUNC_PARA_VARS, FUNCTION_DEF_SCOPE, N_FUNC_HANDLER, CHAIN_HANLDER, ELSE_HANDLER,
};

enum DFActionType {
	CONST_STRING,
	CONST_DOUBLE,
	OPERATOR,
	SEMICOLON,
	WHILE,
	IF,
	ELSE,
	IDENT,
	OP_BRACE,
	CLOSE_BRACE,
	OP_PARAN,
	CLOSE_PARAN,
	RETURN,
	IGNORES,
	CAMMA,
	ADDR,
	ASSIGN,
	SCOPE,
	FUNCTION,
	CONTINUE,
	BREAK,

	/// compile time lexer types

	DOUBLE,
	PUSH,
	POP,
	JUMP,
	CALL,
	JUMPF,
	JUMPT,
	PLUS,
	NEGATIVE,
	MULTI,
	DIVIDE,
	FUNC_IDENT,
	STRING,

	EMPTY,
	NEXT_INST,

	FUNC_ADDR,

	VOID,
};

using DFActionVal = std::variant<std::string, double, size_t>;


#include "DFMatcher.h"
#include "DFAction.h"
#include "JuaScope.h"

struct JuaFuncDef {
	std::string func_code;
	std::vector<std::string> paras_addrs;
};

#endif // !JUALANG_TYPES