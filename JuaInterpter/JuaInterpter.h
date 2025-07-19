#pragma once
#ifndef JUA_INTERPTER
#define JUA_INTERPTER


#include <iostream>
#include "JuaInterpter_Types.h"
#include <functional>

typedef JuaOprand(JuaModule::* JuaFunc)(std::vector<JuaStackVal>& oprands);



struct JuaExtension {
	JuaFunc func;
	JuaModule* obj;
};

class JuaLang;

class JuaInterpter {
	friend class JuaLang;

	std::vector<JuaInstruction> instructions;

	DFMatcher lexer;

	std::vector<JuaStackVal> stack;

	std::unordered_map<size_t, JuaOprand> v_mem;

	std::vector<JuaExtension> extensions;

	std::unordered_map<std::string, size_t> ext_table;

	JuaOprand convert_DFMatcherRes(const DFMatcherRes& res) ;
public:
	template<typename T>
	inline void add_extension(const std::string& name, JuaOprand(T::* func)(std::vector<JuaStackVal>&), JuaModule* obj) {

		ext_table[name] = extensions.size();

		extensions.push_back({ static_cast<JuaFunc>(func) , obj });
	}

	std::vector<JuaOprand> run_instructions();

	void insert_bytecode(const std::string& juax_code);

	JuaInterpter(const std::string& juax_code = "");

};

#endif // !JUA_INTERPTER
