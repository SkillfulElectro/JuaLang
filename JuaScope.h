#pragma once
#ifndef JUASCOPE
#define JUASCOPE

#include <string>
#include <unordered_map>

enum JuaScopeStatus {
	JSCOPE_NOT_FOUND,
	JSCOPE_SUCCESS,
};

struct JuaScopeRes {
	JuaScopeStatus status;
	size_t addr;
};

class JuaScope {
	size_t addr_counter;

	std::unordered_map<std::string, size_t> main_vars;
public:
	JuaScope() {
		addr_counter = 0;
	}

	JuaScopeRes get_new_tmp() {
		return { JuaScopeStatus::JSCOPE_SUCCESS , ++addr_counter };
	}

	JuaScopeRes get_new_addr(const std::string& var) {
		return { JuaScopeStatus::JSCOPE_SUCCESS , main_vars[var] };
	}

	JuaScopeRes is_exists(const std::string& var) {
		if (main_vars.find(var) != main_vars.end()) {
			return { JuaScopeStatus::JSCOPE_SUCCESS , main_vars[var] };
		}

		return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 };
	}
};

#endif // !JUASCOPE
