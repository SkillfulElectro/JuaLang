#pragma once
#ifndef JUA_INTERPTER
#define JUA_INTERPTER


#include <iostream>
#include "JuaInterpter_Types.h"
#include <functional>

using JuaFunc = std::function<JuaOprand(std::vector<JuaStackVal>&)>;


class JuaLang;

class JuaInterpter {
	friend class JuaLang;

	std::vector<JuaInstruction> instructions;

	DFMatcher lexer;

	std::vector<JuaStackVal> stack;

	std::unordered_map<size_t, JuaOprand> v_mem;

	std::vector<JuaFunc> extensions;

	std::unordered_map<std::string, size_t> ext_table;

	JuaOprand convert_DFMatcherRes(const DFMatcherRes& res) ;
public:

	void add_extension(const std::string& name, JuaFunc func);

	std::vector<JuaOprand> run_instructions();

	void insert_bytecode(const std::string& juax_code);

	JuaInterpter(const std::string& juax_code = "");

};

#endif // !JUA_INTERPTER
