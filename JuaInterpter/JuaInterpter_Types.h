#pragma once
#ifndef JUA_OPRAND
#define JUA_OPRAND

#include "JuaLangTypes.h"

#include <variant>
#include <string>
#include <functional>
#include <iostream>

class JuaOprand
{
public:
	DFActionType op_type;
	std::variant<size_t, double, std::string, void *> value;

public:
	std::function<void(JuaOprand *)> destructor = nullptr;
	std::function<JuaOprand &(JuaOprand *, JuaOprand &&)> move = nullptr;
	std::function<JuaOprand &(JuaOprand *, JuaOprand &)> copy = nullptr;

public:
	JuaOprand() = default;
	JuaOprand(DFActionType op_type , std::variant<size_t, double, std::string, void *> value);
	JuaOprand(const JuaOprand &) = default;
	JuaOprand(JuaOprand &&) noexcept = default;

	size_t get_sizet();

	double get_doub();

	std::string get_str();

	void *get_void_ptr();

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

#include "DFMatcher.h"



// void type will be converted to JuaModule
class JuaModule
{
};

#endif // !JUA_OPRAND