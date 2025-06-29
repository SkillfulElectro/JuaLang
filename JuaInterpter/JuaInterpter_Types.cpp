#include "JuaInterpter_Types.h"

JuaOprand::JuaOprand() {}

JuaOprand::JuaOprand(std::function<void(void*)> destructor) {
	this->cleaner = cleaner;
}

JuaOprand::JuaOprand(DFActionType type , std::variant<size_t, double, std::string , void*> data ,std::function<void(void*)> destructor) {
	op_type = type;
	value = data;
	cleaner = destructor;
}

JuaOprand::~JuaOprand() {
	if (op_type == VOID) {
		if (cleaner) {
			cleaner(std::get<void*>(value));
		}
	}
}

size_t JuaOprand::get_sizet() {
	return std::get<size_t>(value);
}

double JuaOprand::get_doub() {
	return std::get<double>(value);
}

std::string JuaOprand::get_str() {
	return std::get<std::string>(value);
}

void* JuaOprand::get_void_ptr() {
	return std::get<void*>(value);
}


JuaOprand JuaStackVal::get_obj() {
	return std::get<JuaOprand>(value);
}

JuaOprand* JuaStackVal::get_ptr() {
	return std::get<JuaOprand*>(value);
}