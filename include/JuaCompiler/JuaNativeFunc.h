#pragma once
#ifndef JUANATIVE_FUNC
#define JUANATIVE_FUNC

#include "JuaLangTypes.h"

struct JuaCompileOprand {
	DFActionType op_type;
	std::string value;
};

class JuaCompileTimeLexer {
	DFMatcher lexer;

	JuaCompileOprand convert_DFMatcherRes(const DFMatcherRes& res) ;

public:
	JuaCompileTimeLexer();

	std::vector<JuaCompileOprand> insert_bytecode(const std::string& juax_code) ;
};


#endif // !JUANATIVE_FUNC