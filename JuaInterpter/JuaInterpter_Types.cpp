#include "JuaInterpter_Types.h"

JuaOprand::JuaOprand(DFActionType op_type , std::variant<size_t, double, std::string, JuaVoidType *> value) {
	this->op_type = op_type;
	this->value = value;
}

JuaOprand::~JuaOprand() {
	if (destructor) {
		destructor(this);
	}
}

JuaOprand& JuaOprand::operator=(JuaOprand& other) {
	if (copy) {
		return copy(this , other);
	} 

	op_type = other.op_type;
	value = other.value;
	copy = other.copy;
	destructor = other.destructor;
	move = other.move;


	return *this;
}

JuaOprand& JuaOprand::operator=(JuaOprand&& other) noexcept {
	if (move) {
		return move(this, std::move(other));
	} 
	this->~JuaOprand();

	op_type = other.op_type;
	value = other.value;
	copy = other.copy;
	destructor = other.destructor;
	move = other.move;

	other.value = nullptr;
	other.copy = nullptr;
	other.destructor = nullptr;
	other.move = nullptr;

	return *this;
}

size_t& JuaOprand::get_sizet() {
	return std::get<size_t>(value);
}

double& JuaOprand::get_doub() {
	return std::get<double>(value);
}

std::string& JuaOprand::get_str() {
	return std::get<std::string>(value);
}

JuaVoidType* JuaOprand::get_void_ptr() {
	return std::get<JuaVoidType*>(value);
}


JuaOprand JuaStackVal::get_obj() {
	return std::get<JuaOprand>(value);
}

JuaOprand* JuaStackVal::get_ptr() {
	return std::get<JuaOprand*>(value);
}