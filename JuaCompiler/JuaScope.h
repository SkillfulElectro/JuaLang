#pragma once
#ifndef JUASCOPE
#define JUASCOPE

#include <string>
#include <unordered_map>

#include <iostream>

enum JuaScopeStatus {
	JSCOPE_NOT_FOUND,
	JSCOPE_SUCCESS,
};

struct JuaScopeRes {
	JuaScopeStatus status;
	std::string addr;
	std::string ident_name;
};

struct JuaAbstractScope {
	std::unordered_map<std::string, size_t> main_vars;
	std::vector<size_t> temp_addrs;
};

class JuaScope {
	size_t addr_counter;

	std::vector<JuaAbstractScope> scopes;

	std::vector<size_t> removed_addrs;

	bool func_style = false;
	size_t func_style_start_ind = 0;
public:
	JuaScope();

	void activate_func_style();

	void deactivate_func_style();

	void create_new_scope();

	void destroy_scope();

	JuaScopeRes get_ident_name(std::string addr_s);

	JuaScopeRes get_new_tmp();

	JuaScopeRes get_new_addr(const std::string& var);

	//JuaScopeRes is_exists(const std::string& var) ;
};

#endif // !JUASCOPE