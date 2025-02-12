#pragma once
#ifndef JUA_OPRAND
#define JUA_OPRAND

#define DFACTION_TYPES

enum DFActionType {
	DOUBLE,
	PUSH,
	POP,
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
#include "../compiler_sys/DFAction_Types.h"

struct JuaOprand {
	DFActionType op_type;
	std::variant<size_t, double, std::string , void*> value;

	size_t get_sizet() {
		return std::get<size_t>(value);
	}

	double get_doub() {
		return std::get<double>(value);
	}

	std::string get_str() {
		return std::get<std::string>(value);
	}

	void* get_void_ptr() {
		return std::get<void*>(value);
	}
};

enum JuaStackType {
	REF,
	VALUE,
};

struct JuaStackVal {
	JuaStackType type;
	std::variant<JuaOprand, JuaOprand*> value;

	JuaOprand get_obj() {
		return std::get<JuaOprand>(value);
	}

	JuaOprand* get_ptr() {
		return std::get<JuaOprand*>(value);
	}
};

struct JuaInstruction {
	DFActionType job;
	JuaOprand oprand1;
	JuaOprand oprand2;
	JuaOprand result;
};

#include "../lexer_sys/DFMatcher.h"
#include "JuaModule.h"

#endif // !JUA_OPRAND
