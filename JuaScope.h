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
	std::string ident_name;
};

class JuaScope {
	size_t addr_counter;

	std::unordered_map<std::string, size_t> main_vars;

	std::vector<size_t> removed_addrs;
public:
	JuaScope() {
		addr_counter = 0;
	}

	JuaScopeRes get_ident_name(size_t addr) {
		for (auto elm : main_vars) {
			if (elm.second == addr) {
				main_vars.erase(elm.first);
				removed_addrs.push_back(elm.second);

				return { JuaScopeStatus::JSCOPE_SUCCESS, elm.second, elm.first };
			}
		}

		return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 , "" };
	}

	JuaScopeRes get_new_tmp() {

		if (!removed_addrs.empty()) {
			auto addr = removed_addrs.back();
			removed_addrs.pop_back();

			return{ JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
		}

		return { JuaScopeStatus::JSCOPE_SUCCESS , addr_counter++ , "" };
	}

	JuaScopeRes get_new_addr(const std::string& var) {
		if (main_vars.find(var) == main_vars.end()) {
			if (!removed_addrs.empty()) {
				main_vars[var] = removed_addrs.back();
				removed_addrs.pop_back();
			}
			else {
				main_vars[var] = addr_counter++;
			}
		}

		return { JuaScopeStatus::JSCOPE_SUCCESS , main_vars[var] , ""};
	}

	//JuaScopeRes is_exists(const std::string& var) {
	//	if (main_vars.find(var) != main_vars.end()) {
	//		return { JuaScopeStatus::JSCOPE_SUCCESS , main_vars[var] , ""};
	//	}

	//	return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 , ""};
	//}
};

#endif // !JUASCOPE
