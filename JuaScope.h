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
	size_t addr;
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
public:
	JuaScope() {
		addr_counter = 0;
	}

	void create_new_scope() {
		scopes.push_back(JuaAbstractScope());
	}

	void destroy_scope() {
		for (auto i : scopes.back().main_vars) {

			removed_addrs.push_back(i.second);
		}

		for (auto i : scopes.back().temp_addrs) {

			removed_addrs.push_back(i);
		}

		scopes.pop_back();
	}

	JuaScopeRes get_ident_name(size_t addr) {
		for (auto& scope : scopes) {

			auto& main_vars = scope.main_vars;

			for (auto elm : main_vars) {
				if (elm.second == addr) {
					main_vars.erase(elm.first);
					removed_addrs.push_back(elm.second);

					return { JuaScopeStatus::JSCOPE_SUCCESS, elm.second, elm.first };
				}
			}
		}

		return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 , "" };
	}

	JuaScopeRes get_new_tmp() {

		if (!removed_addrs.empty()) {
			auto addr = removed_addrs.back();
			removed_addrs.pop_back();
			scopes.back().temp_addrs.push_back(addr);

			return{ JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
		}

		size_t addr = addr_counter++;
		scopes.back().temp_addrs.push_back(addr);

		return { JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
	}

	JuaScopeRes get_new_addr(const std::string& var) {
		for (auto scope : scopes) {
			auto& main_vars = scope.main_vars;

			if (main_vars.find(var) != main_vars.end()) {
				return { JuaScopeStatus::JSCOPE_SUCCESS ,main_vars[var] , "" };
			}
		}


		if (!removed_addrs.empty()) {
			scopes.back().main_vars[var] = removed_addrs.back();
			removed_addrs.pop_back();
		}
		else {
			scopes.back().main_vars[var] = addr_counter++;
		}


		return { JuaScopeStatus::JSCOPE_SUCCESS , scopes.back().main_vars[var] , ""};
	}

	//JuaScopeRes is_exists(const std::string& var) {
	//	if (scopes.back().main_vars.find(var) != scopes.back().main_vars.end()) {
	//		return { JuaScopeStatus::JSCOPE_SUCCESS , scopes.back().main_vars[var] , ""};
	//	}

	//	return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 , ""};
	//}
};

#endif // !JUASCOPE
