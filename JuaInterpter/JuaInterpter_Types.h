#pragma once
#ifndef JUA_OPRAND
#define JUA_OPRAND

#include "JuaLangTypes.h"


#include <variant>
#include <string>
#include <functional>
#include <iostream>

class JuaOprand {
public:
	// do not use it with void types , you will leak mem
	JuaOprand();

	JuaOprand(std::function<void(void*)> destructor);

	JuaOprand(DFActionType type , 
		std::variant<size_t, double, std::string , void*> data ,
		std::function<void(void*)> destructor = nullptr );

	DFActionType op_type;
	std::variant<size_t, double, std::string , void*> value;
	// if set to void , it cleaner must be set or memory leak will happen
	std::function<void(void*)> cleaner;

	size_t get_sizet();

	double get_doub();

	std::string get_str();

	void* get_void_ptr();

	~JuaOprand();

  JuaOprand(const JuaOprand&) = default;
  JuaOprand(JuaOprand&&) noexcept = default;
  JuaOprand& operator=(const JuaOprand&) = default;
  JuaOprand& operator=(JuaOprand&&) noexcept = default;

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

	JuaInstruction() : oprand1{nullptr} , oprand2{nullptr} , result{nullptr} {}
};

#include "DFMatcher.h"
class JuaModule {};

#endif // !JUA_OPRAND