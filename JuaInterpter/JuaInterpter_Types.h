#pragma once
#ifndef JUA_OPRAND
#define JUA_OPRAND

#include "JuaLangTypes.h"


#include <variant>
#include <string>
#include <thread>
#include <mutex>

class JuaOprand {
public:
	DFActionType op_type;
	std::variant<size_t, double, std::string , void*> value;

	size_t get_sizet();

	double get_doub();

	std::string get_str();

	void* get_void_ptr();
};

enum JuaStackType {
	REF,
	VALUE,
};

struct JuaStackVal {
	JuaStackType type;
	std::variant<JuaOprand, JuaOprand*> value;

	JuaOprand get_obj();

	JuaOprand* get_ptr();
};

struct JuaInstruction {
	DFActionType job;
	JuaOprand oprand1;
	JuaOprand oprand2;
	JuaOprand result;
};

#include "DFMatcher.h"
class JuaModule {};

#endif // !JUA_OPRAND