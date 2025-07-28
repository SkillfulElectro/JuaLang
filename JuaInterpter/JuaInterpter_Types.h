#pragma once
#ifndef JUA_OPRAND
#define JUA_OPRAND

#include "JuaLangTypes.h"

#include <variant>
#include <string>
#include <functional>
#include <iostream>

class JuaVoidType;

class JuaOprand
{
public:
	DFActionType op_type;
	std::variant<size_t, double, std::string, JuaVoidType* > value;

public:
	std::function<void(JuaOprand *)> destructor = nullptr;
	std::function<JuaOprand &(JuaOprand *, JuaOprand &&)> move = nullptr;
	std::function<JuaOprand &(JuaOprand *, JuaOprand &)> copy = nullptr;

public:
	JuaOprand() = default;
	JuaOprand(DFActionType op_type , std::variant<size_t, double, std::string, JuaVoidType *> value);
	JuaOprand(const JuaOprand &) = default;
	JuaOprand(JuaOprand &&) noexcept = default;

	size_t& get_sizet();

	double& get_doub();

	std::string& get_str();

	JuaVoidType* get_void_ptr();

	~JuaOprand();

	JuaOprand &operator=(JuaOprand &other);
	JuaOprand &operator=(JuaOprand &&other) noexcept;
};

enum JuaStackType
{
	REF,
	VALUE,
};

struct JuaStackVal
{
	JuaStackType type;
	std::variant<JuaOprand, JuaOprand *> value;

	JuaOprand get_obj();

	JuaOprand *get_ptr();
};



struct JuaInstruction
{
	DFActionType job;
	JuaOprand oprand1;
	JuaOprand oprand2;
	JuaOprand result;
};


class JuaVoidType
{
public:
	virtual JuaOprand run_func_by_symbol(const std::string& , std::vector<JuaStackVal>&) = 0;
	virtual bool is_copyable() {
		return false;
	}

	virtual JuaOprand copy() {
		return {DOUBLE , 0.0};
	}

	virtual bool is_equatable() {
		return false;
	}

	virtual bool equals(JuaVoidType*) {
		return false;
	}

	virtual bool is_hashable() {
		return false;
	}

	virtual size_t hash() {
		return 0;
	}
};

#include "DFMatcher.h"





#endif // !JUA_OPRAND