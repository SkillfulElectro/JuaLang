#include "JuaLang.h"

JuaScope::JuaScope() {
	addr_counter = 0;
	func_style = false;
}

void JuaScope::activate_func_style() {
	func_style = true;
	func_style_start_addr = addr_counter;
	func_style_scope_index = scopes.size();
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
		addr = std::stoull(addr_s.substr(1).c_str()) + func_style_start_addr;
	}
	else {
		addr = std::stoull(addr_s);
	}

	for (auto& scope{--scopes.end()} ; ; --scope) {

		auto& main_vars = scope->main_vars;

		for (auto elm : main_vars) {
			if (elm.second == addr) {
				main_vars.erase(elm.first);
				removed_addrs.push_back(elm.second);

				return { JuaScopeStatus::JSCOPE_SUCCESS, std::to_string(elm.second), elm.first };
			}
		}

		if (scope == scopes.begin()) {
			break;
		}
	}


	return { JuaScopeStatus::JSCOPE_NOT_FOUND , "0" , "" };
}

JuaScopeRes JuaScope::get_new_tmp() {
	std::string addr;
	if (!func_style) {
		if (!removed_addrs.empty()) {
			auto addr_res = removed_addrs.back();
			removed_addrs.pop_back();
			scopes.back().temp_addrs.push_back(addr_res);

			return{ JuaScopeStatus::JSCOPE_SUCCESS , std::to_string(addr_res) , "" };
		}

		size_t addr_res = addr_counter++;
		scopes.back().temp_addrs.push_back(addr_res);

		addr = std::to_string(addr_res);
	} else {
		size_t addr_res = addr_counter++;
		scopes.back().temp_addrs.push_back(addr_res);

		addr = std::to_string(addr_res);


		addr = "$" + std::to_string(addr_res - func_style_start_addr);
	}




	return { JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
}

JuaScopeRes JuaScope::get_new_addr(const std::string& var) {

	if (func_style) {
		for (size_t scope_index = func_style_scope_index ;
			 scope_index < scopes.size() ; ++scope_index) {
			
			auto& main_vars = scopes[scope_index].main_vars;

			if (main_vars.find(var) != main_vars.end()) {
				return { JuaScopeStatus::JSCOPE_SUCCESS ,"$" + std::to_string(main_vars[var] - func_style_start_addr) , "" };
			}
		}
	}

	
	for (auto scope : scopes) {

		auto& main_vars = scope.main_vars;
		
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
		addr = "$" + std::to_string(addr_res - func_style_start_addr);
	}


	return { JuaScopeStatus::JSCOPE_SUCCESS , addr , "" };
}

//JuaScopeRes JuaScope::is_exists(const std::string& var) {
//	if (scopes.back().main_vars.find(var) != scopes.back().main_vars.end()) {
//		return { JuaScopeStatus::JSCOPE_SUCCESS , scopes.back().main_vars[var] , ""};
//	}

//	return { JuaScopeStatus::JSCOPE_NOT_FOUND , 0 , ""};
//}