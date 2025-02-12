#pragma once
#ifndef JUA_MODULE
#define JUA_MODULE


#include "JuaOprand.h"

#include <vector>

class JuaInterpter;

class JuaModule {
public:
	JuaModule(const std::string& func_name, JuaInterpter* instance);

	virtual JuaOprand jua_extension_func(std::vector<JuaStackVal>& oprands) = 0;
};

#endif // !JUA_MODULE
