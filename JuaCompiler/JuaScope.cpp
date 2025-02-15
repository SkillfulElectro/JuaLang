#include "JuaScope.h"

JuaScope::JuaScope() {
	addr_counter = 0;
	func_style = false;
}

void JuaScope::activate_func_style() {
	func_style = true;
	func_style_start_ind = addr_counter;
}

void JuaScope::deactivate_func_style() {
	func_style = false;
}

void JuaScope::create_new_scope() {
	scopes.push_back(JuaAbstractScope());
}

void JuaScope::destroy_scope() {
	for (auto i : scopes.back().main_vars) {

		removed_addrs.push_back(i.second);
	}

	for (auto i : scopes.back().temp_addrs) {

		removed_addrs.push_back(i);
	}

	scopes.pop_back();
}

JuaScopeRes JuaScope::get_ident_name(std::string addr_s) {
	size_t addr;

	if (addr_s[0] == '$') {
		addr = std::stoull(addr_s.substr(1).c_str()) + func_style_start_ind;
	}
	else {
		addr = std::stoull(addr_s);
	}

	for (auto& scope : scopes) {

		auto& main_vars = scope.main_vars;

		for (auto elm : main_vars) {
			if (elm.second == addr) {
				main_vars.erase(elm.first);
				removed_addrs.push_back(elm.second);

				return { JuaScopeStatus::JSCOPE_SUCCESS, std::to_string(elm.second), elm.first };
			}
		}
	}

	return { JuaScopeStatus::JSCOPE_NOT_FOUND , "0" , "" };
}

JuaScopeRes JuaScope::get_new_tmp() {

	if (!removed_addrs.empty()) {
		auto addr = removed_addrs.back();
		removed_addrs.pop_back();
		scopes.back().temp_addrs.push_back(addr);

		return{ JuaScopeStatus::JSCOPE_SUCCESS , std::to_string(addr) , "" };
	}

	size_t addr_res = addr_counter++;
	scopes.back().temp_addrs.push_back(addr_res);

	std::string addr = std::to_string(addr_res);

	if (func_style) {
		addr = "$" + std::to_string(addr_res - func_style_start_ind);
	}

	return { JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
}

JuaScopeRes JuaScope::get_new_addr(const std::string& var) {
	for (auto scope : scopes) {

		auto& main_vars = scope.main_vars;

		if (func_style) {
			if (scope.main_vars == scopes.back().main_vars) {

				if (main_vars.find(var) != main_vars.end()) {
					return { JuaScopeStatus::JSCOPE_SUCCESS ,"$" + std::to_string(main_vars[var]) , "" };
				}
			}
		}

		if (main_vars.find(var) != main_vars.end()) {
			return { JuaScopeStatus::JSCOPE_SUCCESS ,std::to_string(main_vars[var]) , "" };
		}
	}


	if (!removed_addrs.empty()) {
		scopes.back().main_vars[var] = removed_addrs.back();
		removed_addrs.pop_back();
	}
	else {
		scopes.back().main_vars[var] = addr_counter++;
	}

	size_t addr_res = scopes.back().main_vars[var];
	std::string addr = std::to_string(scopes.back().main_vars[var]);

	if (func_style) {
		addr = "$" + std::to_string(addr_res - func_style_start_ind);
	}


	return { JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
}

//JuaScopeRes JuaScope::is_exists(const std::string& var) {
//	if (scopes.back().main_vars.find(var) != scopes.back().main_vars.end()) {
//		return { JuaScopeStatus::JSCOPE_SUCCESS , scopes.back().main_vars[var] , ""};
//	}

//	return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 , ""};
//}