#pragma once
#ifndef JUA_OPRAND
#define JUA_OPRAND

#define DFACTION_TYPES

enum DFActionType {
	DOUBLE,
	PUSH,
	JUMP,
	CALL,
	JUMPF,
	JUMPT,
	ASSIGN,
	PLUS,
	NEGATIVE,
	MULTI,
	DIVIDE,
	FUNC_IDENT,
	STRING,

	ADDR,
	IGNORES,
	EMPTY,
	NEXT_INST,

	RETURN,

	VOID,
};


#include <variant>
#include <string>
#include "DFAction_Types.h"

struct JuaOprand {
	DFActionType op_type;
	std::variant<size_t, double, std::string> value;

	size_t get_sizet() {
		return std::get<size_t>(value);
	}

	double get_doub() {
		return std::get<double>(value);
	}

	std::string get_str() {
		return std::get<std::string>(value);
	}
};

#endif // !JUA_OPRAND