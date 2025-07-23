#include "JuaLang.h"

DFActionFlow JuaLang::start_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case IDENT: {
		stack.push_back(token);
		return {DFACTION_GO_TO_SP_DFA , IDENTER};
	}
	case RETURN:
		go_next_index = false;
		return { DFACTION_GO_TO_SP_DFA , RETURN_HANDLER };
	case CLOSE_BRACE:
		go_next_index = false;
		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	case WHILE:
		go_next_index = false;
		return { DFACTION_GO_TO_SP_DFA , WHILE_HANDLER };
	case IF:
		go_next_index = false;
		return { DFACTION_GO_TO_SP_DFA , ELIF_CHAIN_HANLDER };
	case OP_BRACE:
		go_next_index = false;
		return { DFACTION_GO_TO_SP_DFA , SCOPE_HANDLER };
	case MACRO: // inline functions are macros from now on !
	case FUNCTION: // this case is not implemented yet !
		go_next_index = false;
		return { DFACTION_GO_TO_SP_DFA , MACRO_DEF_HANDLER };
	case CONTINUE:
		stack.push_back({ CONTINUE , bytecode.size() });
		bytecode.push_back("");

		break;
	case BREAK:
		stack.push_back({ BREAK , bytecode.size() });
		bytecode.push_back("");

		break;
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::identer_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case OP_PARAN: {
		go_next_index = false;

		auto ident = stack.back();

		if (macros_code.key_exists(get_dfval_str(ident.value))) {
			return { DFACTION_GO_TO_SP_DFA , MACRO_CALL_HANDLER };
		}
		else {

			return { DFACTION_GO_TO_SP_DFA , FUNC_HANDLER };
		}
	}

	case DOT_OPERATOR : {
		auto iden = stack.back();
		auto addr = scopes.get_new_addr(get_dfval_str(iden.value));
		stack.pop_back();
		stack.push_back({ADDR , addr.addr});
		stack.push_back({DOT_OPERATOR , "."});

		return { DFACTION_GO_TO_SP_DFA , DOT_OPERATOR_HANDLER };
	}

	case ASSIGN: {
		auto identi = stack.back();
		stack.pop_back();

		std::string addr = scopes.get_new_addr(get_dfval_str(identi.value)).addr;

		stack.push_back({ ADDR , addr });

		stack.push_back({
			ASSIGN , "="
			});
		return { DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}

	case SEMICOLON: {
		std::ostringstream code;

		auto token1 = stack.back();
		stack.pop_back();

		if (stack.size() > 1) {
			auto token2 = stack.back();
			stack.pop_back();

			switch (token2.type)
			{
			case ASSIGN: {
				auto main_addr = stack.back();
				stack.pop_back();

				code << "=" << " " << get_dfval_str(token1.value) << " " << ";" << " " << get_dfval_str(main_addr.value);

				bytecode.push_back(code.str());


				break;
			}
			default:
				stack.push_back(token2);
				break;
			}
		}

		return {DFACTION_BACK_TO_PREV , DFActionState(0)};
	}
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::dot_operator_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];
	
	switch (token.type)
	{
	case IDENT:
		stack.push_back(token);
		return {DFACTION_BACK_TO_PREV , IDENTER};
	}

	return {DFACTION_PANIC , DFActionState(0)};
}

/// <summary>
/// Deprected but kept for future use cases
/// </summary>
/// <param name="index_in_tokens"></param>
/// <param name="tokens"></param>
/// <param name="go_next_index"></param>
/// <returns></returns>
DFActionFlow JuaLang::func_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	std::ostringstream code;


	switch (token.type)
	{
	case OP_PARAN:
	{
		stack.push_back({ OP_PARAN , 0.0 });
		stack.push_back({ ASSIGN , "=" });
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case CAMMA:
	{
		auto expr_val = stack.back();
		stack.pop_back();
		stack.pop_back();

		auto counter = stack.back();
		stack.pop_back();

		code << "push" << " " << get_dfval_str(expr_val.value) << " " << ";" << " " << ";";
		bytecode.push_back(code.str());

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);
		stack.push_back({ ASSIGN , "=" });

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case CLOSE_PARAN: {
		auto expr_val = stack.back();
		stack.pop_back();
		stack.pop_back();

		auto counter = stack.back();
		stack.pop_back();

		code << "push" << " " << get_dfval_str(expr_val.value) << " " << ";" << " " << ";";
		bytecode.push_back(code.str());

		counter.value = get_dfval_doub(counter.value) + 1;

		code.str("");

		auto identi = stack.back();
		stack.pop_back();

		code << "call" << " " << get_dfval_doub(counter.value) << " " << ";" << " " << get_dfval_str(identi.value);
		bytecode.push_back(code.str());
		break;
	}
	default:
		break;
	}

	/*switch (token.type)
	{
	case CONST_DOUBLE: {
		code << "push" << " " << "#" << get_dfval_str(token.value) << " " << ";" << " " << ";";

		auto counter = stack.back();
		stack.pop_back();

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);

		bytecode.push_back(code.str());
		break;
	}
	case CONST_STRING: {
		code << "push" << " " << get_dfval_str(token.value) << " " << ";" << " " << ";";

		auto counter = stack.back();
		stack.pop_back();

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);

		bytecode.push_back(code.str());
		break;
	}

	case IDENT: {
		auto res = scopes.back().is_exists(get_dfval_str(token.value));

		if (res.status != JuaScopeStatus::JSCOPE_SUCCESS) {
			return { DFACTION_PANIC , DFActionState(0) };
		}

		code << "push" << " " << res.addr << " " << ";" << " " << ";";

		auto counter = stack.back();
		stack.pop_back();

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);

		bytecode.push_back(code.str());
		break;
	}
	case CLOSE_PARAN: {
		auto len = stack.back();
		stack.pop_back();
		auto identi = stack.back();
		stack.pop_back();

		code << "call" << " " << get_dfval_doub(len.value) << " " << ";"  << " " << get_dfval_str(identi.value);
		bytecode.push_back(code.str());
		break;
	}
	default:
		break;
	}*/



	return { DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::expr_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];
	auto& prev_op_token = stack[stack.size() - 2];

	if (prev_op_token.type == ASSIGN && token.type == OPERATOR) {
		stack.push_back(token);

		return { DFACTION_SAFE , DFActionState(0) };
	}

	std::ostringstream code;

	switch (token.type)
	{
	case OP_PARAN: {
		auto iden = stack.back();
		stack.pop_back();

		go_next_index = false;
		if (stack.back().type != DOT_OPERATOR) {
			auto res = scopes.get_ident_name(get_dfval_str(iden.value));

			if (res.status == JuaScopeStatus::JSCOPE_NOT_FOUND) {
				return { DFACTION_PANIC , DFActionState(0) };
			}

			stack.push_back({
				IDENT , res.ident_name
				});
		} else {
			stack.push_back(iden);
		}

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_FUNC_ROUTER };
	}

	case DOT_OPERATOR : {
		go_next_index = true;
		stack.push_back({DOT_OPERATOR , "."});

		return { DFACTION_GO_TO_SP_DFA , DOT_OPERATOR_HANDLER };
	}

	case OPERATOR: {
		switch (get_dfval_str(token.value)[0])
		{
		case '+':
			switch (get_dfval_str(prev_op_token.value)[0])
			{
			case '-':
			case '*':
			case '/':
			case '+': {
				std::string addr = scopes.get_new_tmp().addr;

				auto oprand2 = stack.back();
				stack.pop_back();
				auto op = stack.back();
				stack.pop_back();
				auto oprand1 = stack.back();
				stack.pop_back();

				code << get_dfval_str(op.value) << " " << get_dfval_str(oprand1.value) << " " << get_dfval_str(oprand2.value) << " " << addr;
				bytecode.push_back(code.str());
				code.str("");

				stack.push_back({ ADDR , addr });

				break;
			}
			default:
				break;
			}

			break;
		case '-':
			switch (get_dfval_str(prev_op_token.value)[0])
			{
			case '-':
			case '*':
			case '/':
			case '+': {
				std::string addr = scopes.get_new_tmp().addr;

				auto oprand2 = stack.back();
				stack.pop_back();
				auto op = stack.back();
				stack.pop_back();
				auto oprand1 = stack.back();
				stack.pop_back();

				code << get_dfval_str(op.value) << " " << get_dfval_str(oprand1.value) << " " << get_dfval_str(oprand2.value) << " " << addr;
				bytecode.push_back(code.str());
				code.str("");

				stack.push_back({ ADDR , addr });
				stack.push_back(token);

				break;
			}
			default:
				break;
			}

			break;
		case '*':
			switch (get_dfval_str(prev_op_token.value)[0])
			{
			case '*':
			case '/': {
				std::string addr = scopes.get_new_tmp().addr;

				auto oprand2 = stack.back();
				stack.pop_back();
				auto op = stack.back();
				stack.pop_back();
				auto oprand1 = stack.back();
				stack.pop_back();

				code << get_dfval_str(op.value) << " " << get_dfval_str(oprand1.value) << " " << get_dfval_str(oprand2.value) << " " << addr;
				bytecode.push_back(code.str());
				code.str("");

				stack.push_back({ ADDR , addr });
				stack.push_back(token);

				break;
			}
			case '-':
			case '+': {
				stack.push_back(token);

				break;
			}
			default:
				break;
			}

			break;
		case '/':
			switch (get_dfval_str(prev_op_token.value)[0])
			{
			case '*':
			case '/': {
				std::string addr = scopes.get_new_tmp().addr;

				auto oprand2 = stack.back();
				stack.pop_back();
				auto op = stack.back();
				stack.pop_back();
				auto oprand1 = stack.back();
				stack.pop_back();

				code << get_dfval_str(op.value) << " " << get_dfval_str(oprand1.value) << " " << get_dfval_str(oprand2.value) << " " << addr;
				bytecode.push_back(code.str());
				code.str("");

				stack.push_back({ ADDR , addr });
				stack.push_back(token);

				break;
			}
			case '+':
			case '-': {
				stack.push_back(token);

				break;
			}
			default:
				break;
			}

			break;
		default:
			break;
		}
	}

	break;
	default: {
		DFActionToken tmp_keeper;
		auto top = stack.back();
		stack.pop_back();

		for (; top.type == ADDR || top.type == OPERATOR;) {
			switch (top.type)
			{
			case ADDR:
				tmp_keeper = top;
				top = stack.back();
				stack.pop_back();
				break;

			case OPERATOR: {
				std::string addr = scopes.get_new_tmp().addr;

				auto oprand1 = stack.back();
				stack.pop_back();

				code << get_dfval_str(top.value) << " " << get_dfval_str(oprand1.value) << " " << get_dfval_str(tmp_keeper.value) << " " << addr;
				bytecode.push_back(code.str());
				code.str("");

				tmp_keeper = { ADDR , addr };

				top = stack.back();
				stack.pop_back();

				break;
			}
			}


		}

		stack.push_back(top);
		stack.push_back(tmp_keeper);



		go_next_index = false;
		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::expr_ops_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	std::ostringstream code;

	switch (token.type)
	{
	case CLOSE_PARAN:
	{
		auto last_elm = stack.back();

		if (last_elm.type == ASSIGN) {
			go_next_index = false;
			return { DFACTION_BACK_TO_PREV , DFActionState(0) };
		}
		else {
			break;
		}
	}



	case OP_PARAN:
		return { DFACTION_GO_TO_SP_DFA , EXPR_PARA };
	case CONST_DOUBLE: {
		std::string addr = scopes.get_new_tmp().addr;

		code << "=" << " " << "#" << get_dfval_str(token.value) << " " << ";" << " " << addr;
		bytecode.push_back(code.str());

		stack.push_back({ ADDR , addr });

		break;
	}
	case CONST_STRING: {
		std::string addr = scopes.get_new_tmp().addr;

		code << "=" << " " << get_dfval_str(token.value) << " " << ";" << " " << addr;
		bytecode.push_back(code.str());

		stack.push_back({ ADDR , addr });

		break;
	}
	case IDENT: {
		
		std::string addr = scopes.get_new_addr(get_dfval_str(token.value)).addr;


		stack.push_back({ ADDR , addr });

		break;
	}
	default:
		go_next_index = false;
		return { DFACTION_BACK_TO_PREV ,  DFActionState(0) };
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::expr_para_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case CLOSE_PARAN: {
		auto res = stack.back();
		stack.pop_back();
		stack.pop_back();

		stack.push_back(res);

		go_next_index = false;
		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	default:

		stack.push_back({ ASSIGN , "=" });
		go_next_index = false;
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };

		break;
	}
}

DFActionFlow JuaLang::expr_func_router_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	if (token.type == OP_PARAN) {
		go_next_index = false;

		auto ident = stack.back();

		if (macros_code.key_exists(get_dfval_str(ident.value))) {
			return { DFACTION_GO_TO_SP_DFA , MACRO_CALL_HANDLER };
		}
		else {

			return { DFACTION_GO_TO_SP_DFA , FUNC_HANDLER };
		}
	}

	go_next_index = false;

	return { DFACTION_BACK_TO_PREV , DFActionState(0) };
}

DFActionFlow JuaLang::func_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	std::ostringstream code;


	switch (token.type)
	{
	case OP_PARAN:
	{
		if (stack.size() >= 3) {
			auto identer = stack.back();
			stack.pop_back();
			auto dot = stack.back();

			if (dot.type == DOT_OPERATOR) {
				stack.pop_back();

				auto ident = stack.back();
				stack.pop_back();

				code << "push" << " " << get_dfval_str(ident.value) << " " << ";" << " " << ";";
				bytecode.push_back(code.str());

				stack.push_back(identer);
				stack.push_back({ OP_PARAN , 1.0 });
				stack.push_back({ ASSIGN , "=" });
			} else {
				stack.push_back(identer);

				stack.push_back({ OP_PARAN , 0.0 });
				stack.push_back({ ASSIGN , "=" });
			}
		} else {
			stack.push_back({ OP_PARAN , 0.0 });
			stack.push_back({ ASSIGN , "=" });
		}



		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case CAMMA:
	{
		auto expr_val = stack.back();
		stack.pop_back();
		stack.pop_back();

		auto counter = stack.back();
		stack.pop_back();

		code << "push" << " " << get_dfval_str(expr_val.value) << " " << ";" << " " << ";";
		bytecode.push_back(code.str());

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);
		stack.push_back({ ASSIGN , "=" });

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case CLOSE_PARAN: {
		auto expr_val = stack.back();


		stack.pop_back();

		if (expr_val.type != ASSIGN) {
			stack.pop_back();

			auto counter = stack.back();
			stack.pop_back();

			code << "push" << " " << get_dfval_str(expr_val.value) << " " << ";" << " " << ";";
			bytecode.push_back(code.str());

			counter.value = get_dfval_doub(counter.value) + 1;

			code.str("");

			auto identi = stack.back();
			stack.pop_back();

			std::string addr = scopes.get_new_tmp().addr;

			std::string func_name = get_dfval_str(identi.value);

			if (interpter != nullptr) {
				if (interpter->ext_table.find(func_name) != interpter->ext_table.end()) {
					func_name = std::to_string(interpter->ext_table[func_name]);
				}
			}

			code << "call" << " " << func_name << " " << get_dfval_doub(counter.value) << " " << addr;
			bytecode.push_back(code.str());

			stack.push_back({
				ADDR , addr
				});
		}
		else {
			auto counter = stack.back();
			stack.pop_back();

			code.str("");

			auto identi = stack.back();
			stack.pop_back();

			std::string addr = scopes.get_new_tmp().addr;

			std::string func_name = get_dfval_str(identi.value);

			if (interpter != nullptr) {
				if (interpter->ext_table.find(func_name) != interpter->ext_table.end()) {
					func_name = std::to_string(interpter->ext_table[func_name]);
				}
			}

			code << "call" << " " << func_name << " " << get_dfval_doub(counter.value) << " " << addr;
			bytecode.push_back(code.str());

			stack.push_back({
				ADDR , addr
				});
		}
		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	default:
		break;
	}

	/*switch (token.type)
	{
	case CONST_DOUBLE: {
		code << "push" << " " << "#" << get_dfval_str(token.value) << " " << ";" << " " << ";";

		auto counter = stack.back();
		stack.pop_back();

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);

		bytecode.push_back(code.str());
		break;
	}
	case CONST_STRING: {
		code << "push" << " " << get_dfval_str(token.value) << " " << ";" << " " << ";";

		auto counter = stack.back();
		stack.pop_back();

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);

		bytecode.push_back(code.str());
		break;
	}

	case IDENT: {
		auto res = scopes.back().is_exists(get_dfval_str(token.value));

		if (res.status != JuaScopeStatus::JSCOPE_SUCCESS) {
			return { DFACTION_PANIC , DFActionState(0) };
		}

		code << "push" << " " << res.addr << " " << ";" << " " << ";";

		auto counter = stack.back();
		stack.pop_back();

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);

		bytecode.push_back(code.str());
		break;
	}
	case CLOSE_PARAN: {
		auto len = stack.back();
		stack.pop_back();
		auto identi = stack.back();
		stack.pop_back();

		code << "call" << " " << get_dfval_doub(len.value) << " " << ";"  << " " << get_dfval_str(identi.value);
		bytecode.push_back(code.str());
		break;
	}
	default:
		break;
	}*/



	return { DFACTION_SAFE , DFActionState(0) };
}


DFActionFlow JuaLang::return_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	std::ostringstream code;

	switch (token.type)
	{
	case RETURN:
		stack.push_back({ RETURN , 0.0 });
		stack.push_back({ ASSIGN , "=" });

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	case CAMMA:
	{
		auto expr_val = stack.back();
		stack.pop_back();
		stack.pop_back();

		auto counter = stack.back();
		stack.pop_back();

		code << "push" << " " << get_dfval_str(expr_val.value) << " " << ";" << " " << ";";
		bytecode.push_back(code.str());

		counter.value = get_dfval_doub(counter.value) + 1;

		stack.push_back(counter);
		stack.push_back({ ASSIGN , "=" });

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case SEMICOLON:
	{
		auto expr_val = stack.back();


		stack.pop_back();

		if (expr_val.type != ASSIGN) {
			stack.pop_back();

			auto counter = stack.back();
			stack.pop_back();

			code << "push" << " " << get_dfval_str(expr_val.value) << " " << ";" << " " << ";";
			bytecode.push_back(code.str());

			counter.value = get_dfval_doub(counter.value) + 1;

			code.str("");


			code << "ret" << " " << get_dfval_doub(counter.value) << " " << ";" << " " << ";";
			bytecode.push_back(code.str());

		}
		else {
			auto counter = stack.back();
			stack.pop_back();

			code.str("");



			code << "ret" << " " << get_dfval_doub(counter.value) << " " << ";" << " " << ";";
			bytecode.push_back(code.str());


		}
		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	default:
		break;
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::while_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case WHILE:
		stack.push_back({ WHILE , bytecode.size() });
		return { DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	case OP_BRACE:
		scopes.create_new_scope();
		go_next_index = false;
		stack.push_back({ SCOPE , bytecode.size() });
		bytecode.push_back("");

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , WHILE_SCOPE };
	case CLOSE_BRACE:
		scopes.destroy_scope();
		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	default:
		break;
	}

	return { DFActionFlowCode::DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::while_scope_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case OP_BRACE:
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , START };
	case CLOSE_BRACE: {
		std::ostringstream code;

		std::vector<DFActionToken> breaks;
		std::vector<DFActionToken> continues;

		auto jmpf_i = stack.back();
		while (jmpf_i.type == BREAK || jmpf_i.type == CONTINUE) {
			if (jmpf_i.type == BREAK) {
				breaks.push_back(jmpf_i);
			}
			else {
				continues.push_back(jmpf_i);
			}

			stack.pop_back();
			jmpf_i = stack.back();
		}
		stack.pop_back();

		auto expr_addr = stack.back();
		stack.pop_back();

		auto jmp_i = stack.back();
		stack.pop_back();

		code << "jmp" << " " << get_sizet(jmp_i.value) << " " << ";" << " " << ";";
		bytecode.push_back(code.str());

		for (auto& continuee : continues) {
			bytecode[get_sizet(continuee.value)] = code.str();
		}

		code.str("");

		code << "jmpf" << " " << get_dfval_str(expr_addr.value) << " " << bytecode.size() << " " << ";";

		bytecode[get_sizet(jmpf_i.value)] = code.str();

		code.str("");

		code << "jmp" << " " << bytecode.size() << " " << ";" << " " << ";";

		for (auto& breakk : breaks) {
			bytecode[get_sizet(breakk.value)] = code.str();
		}

		go_next_index = false;

		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	default:
		break;
	}

}

DFActionFlow JuaLang::if_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case IF:
		stack.push_back({ IF , bytecode.size() });
		return { DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	case OP_BRACE: {
		go_next_index = false;
		auto result = stack.back();
		stack.pop_back();
		stack.pop_back();

		stack.push_back(result);
		stack.push_back({ SCOPE , bytecode.size() });
		bytecode.push_back("");

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , IF_SCOPE };
	}
	default:
		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
}


DFActionFlow JuaLang::if_scope_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case OP_BRACE:
		scopes.create_new_scope();
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , START };
	case CLOSE_BRACE: {
		std::ostringstream code;

		std::vector<DFActionToken> will_be_pushed_back;

		auto jmpf_i = stack.back();
		while (jmpf_i.type == BREAK || jmpf_i.type == CONTINUE) {
			will_be_pushed_back.push_back(jmpf_i);
			stack.pop_back();
			jmpf_i = stack.back();
		}
		stack.pop_back();

		auto expr_addr = stack.back();

		while (expr_addr.type == BREAK || expr_addr.type == CONTINUE) {
			will_be_pushed_back.push_back(expr_addr);
			stack.pop_back();
			expr_addr = stack.back();
		}
		stack.pop_back();

		stack.push_back({ ELSE , bytecode.size() });
		bytecode.push_back("");

		code << "jmpf" << " " << get_dfval_str(expr_addr.value) << " " << bytecode.size() << " " << ";";

		bytecode[get_sizet(jmpf_i.value)] = code.str();

		go_next_index = false;

		scopes.destroy_scope();

		for (auto& val : will_be_pushed_back) {
			stack.push_back(val);
		}

		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	default:
		break;
	}
}

DFActionFlow JuaLang::elif_chain_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case IF:
		go_next_index = false;
		return { DFACTION_GO_TO_SP_DFA , IF_HANDLER };
	case ELSE:
		return { DFActionFlowCode::DFACTION_SAFE , DFActionState(0) };
	default: {
		go_next_index = false;

		std::vector<DFActionToken> will_be_pushed_back;
		std::vector<DFActionToken> elses;

		auto back = stack.back();

		bool loop_con = true;
		while (loop_con) {
			switch (back.type)
			{
			case CONTINUE:
			case BREAK:
				will_be_pushed_back.push_back(back);
				stack.pop_back();
				break;
			case ELSE:
				elses.push_back(back);
				stack.pop_back();
				break;
			default:
				loop_con = false;
				break;
			}



			if (!stack.empty()) {
				back = stack.back();
			}
			else {
				break;
			}
		}

		for (auto& val : will_be_pushed_back) {
			stack.push_back(val);
		}

		std::ostringstream code;

		code << "jmp" << " " << bytecode.size() << " " << ";" << " " << ";";

		for (auto& val : elses) {
			bytecode[get_sizet(val.value)] = code.str();
		}

		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	}
}

DFActionFlow JuaLang::else_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case IF:
		go_next_index = false;
		return { DFActionFlowCode::DFACTION_SAFE , DFActionState(0) };
	case OP_BRACE:
		go_next_index = false;
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , SCOPE_HANDLER };
	default: {
		go_next_index = false;

		std::vector<DFActionToken> will_be_pushed_back;
		std::vector<DFActionToken> elses;

		auto back = stack.back();

		bool loop_con = true;
		while (loop_con) {
			switch (back.type)
			{
			case CONTINUE:
			case BREAK:
				will_be_pushed_back.push_back(back);
				stack.pop_back();
				break;
			case ELSE:
				elses.push_back(back);
				stack.pop_back();
				break;
			default:
				loop_con = false;
				break;
			}



			if (!stack.empty()) {
				back = stack.back();
			}
			else {
				break;
			}
		}

		for (auto& val : will_be_pushed_back) {
			stack.push_back(val);
		}

		std::ostringstream code;

		code << "jmp" << " " << bytecode.size() << " " << ";" << " " << ";";

		for (auto& val : elses) {
			bytecode[get_sizet(val.value)] = code.str();
		}

		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	}
}

DFActionFlow JuaLang::scope_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case OP_BRACE:
		scopes.create_new_scope();
		macros_code.create_scope();
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , START };
	case CLOSE_BRACE:
		scopes.destroy_scope();
		macros_code.destroy_scope();
		return { DFACTION_BACK_TO_PREV , DFActionState(0) };
	}

	return { DFActionFlowCode::DFACTION_PANIC , DFActionState(0) };
}

DFActionFlow JuaLang::macro_def_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];


	switch (token.type)
	{
	case MACRO:
	case FUNCTION:
		scopes.activate_func_style();
		scopes.create_new_scope();


		stack.push_back({ FUNCTION , bytecode.size() });

		break;
	default:
		return { DFActionFlowCode::DFACTION_PANIC , DFActionState(0) };
	}

	return { DFActionFlowCode::DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::macro_def_ident_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case IDENT:
		macros_code.get_by_key(get_dfval_str(token.value));
		stack.push_back({ IDENT ,token.value });

		break;
	default:
		return { DFActionFlowCode::DFACTION_PANIC , DFActionState(0) };
	}

	return { DFActionFlowCode::DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::macro_def_para_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	std::ostringstream code;


	switch (token.type)
	{
	case OP_PARAN:
	{
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , MACRO_PARA_VARS };
	}
	case CAMMA:
	{
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , MACRO_PARA_VARS };
	}
	case CLOSE_PARAN: {
		break;
	}
	default:
		return { DFActionFlowCode::DFACTION_PANIC , DFActionState(0) };
	}

	return { DFActionFlowCode::DFACTION_SAFE , DFActionState(0) };
}

DFActionFlow JuaLang::macro_para_vars_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	/*std::ostringstream code;*/

	switch (token.type)
	{
	case IDENT: {
		auto& back = stack.back();
		std::string addr = scopes.get_new_addr(get_dfval_str(token.value)).addr;
		macros_code.get_by_key(get_dfval_str(back.value)).paras_addrs.push_back(addr);

		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	case CLOSE_PARAN:
		go_next_index = false;
		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	default:
		return { DFActionFlowCode::DFACTION_PANIC , DFActionState(0) };
	}
}

DFActionFlow JuaLang::macro_def_scope_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	switch (token.type)
	{
	case OP_BRACE:
		macros_code.create_scope();
		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , START };
	case CLOSE_BRACE: {
		scopes.deactivate_func_style();
		scopes.destroy_scope();
		macros_code.destroy_scope();

		auto identi = stack.back();
		stack.pop_back();

		auto index = stack.back();
		stack.pop_back();

		std::string code = "";

		for (size_t i{ get_sizet(index.value) }; i < bytecode.size(); ++i) {
			auto test_jmp = comp_lexer.insert_bytecode(bytecode[i]);

			std::string val = bytecode[i];

			switch (test_jmp[0].op_type)
			{
			case JUMP:
				test_jmp[1].value = std::to_string(std::stoull(test_jmp[1].value) - get_sizet(index.value));

				val = test_jmp[0].value + " " + test_jmp[1].value + " " + test_jmp[2].value + " " + test_jmp[3].value;
				break;
			case JUMPF:
				test_jmp[2].value = std::to_string(std::stoull(test_jmp[2].value) - get_sizet(index.value));

				val = test_jmp[0].value + " " + test_jmp[1].value + " " + test_jmp[2].value + " " + test_jmp[3].value;
				break;
			default:
				break;
			}

			code += val;
			code += '\n';
		}

		macros_code.get_by_key(get_dfval_str(identi.value)).macro_code = code;

		for (size_t bytes{ bytecode.size() }; bytes > get_sizet(index.value); --bytes) {
			bytecode.pop_back();
		}

		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	}
}

DFActionFlow JuaLang::macro_call_handler_action(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, bool& go_next_index) {

	auto& token = tokens[index_in_tokens];

	std::ostringstream code;


	switch (token.type)
	{
	case OP_PARAN:
	{
		stack.push_back({ ASSIGN , "=" });

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case CAMMA:
	{
		auto expr_val = stack.back();

		stack.pop_back();
		stack.pop_back();

		stack.push_back(expr_val);
		stack.push_back({ ASSIGN , "=" });

		return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , EXPR_OPS };
	}
	case CLOSE_PARAN: {
		scopes.create_new_scope();

		std::vector<DFActionToken> expr_vals;
		expr_vals.push_back(stack.back());


		stack.pop_back();

		if (expr_vals.back().type != ASSIGN) {
			stack.pop_back();

			auto& expr_val = stack.back();
			for (; expr_val.type != IDENT;) {
				expr_vals.push_back(expr_val);
				stack.pop_back();
				expr_val = stack.back();
			}

			auto& func = macros_code.get_by_key(get_dfval_str(expr_val.value));


			std::unordered_map<std::string, std::string> addrs;

			if (expr_vals.size() != func.paras_addrs.size()) {
				std::cout << "more args or less args passed to the native Jua function !";
				return { DFACTION_PANIC , DFActionState(0) };
			}

			for (size_t i{ 0 }, j{ expr_vals.size() - 1 }; i < expr_vals.size(); ++i, --j) {
				addrs[func.paras_addrs[i]] = get_dfval_str(expr_vals[j].value);
			}

			auto instructions = comp_lexer.insert_bytecode(func.macro_code);



			size_t func_start = bytecode.size();

			std::string instruction;

			for (size_t i{ 0 }; i < instructions.size(); ++i) {

				auto& instruct = instructions[i];

				switch (instruct.op_type)
				{
				case JUMP: {
					instruction += instruct.value;
					instruction += " ";

					auto& val = instructions[i + 1];

					val.value = std::to_string(std::stoull(val.value) + func_start);

					break;
				}
				case JUMPF: {
					instruction += instruct.value;
					instruction += " ";
					auto& val = instructions[i + 2];


					val.value = std::to_string(std::stoull(val.value) + func_start);


					break;
				}
				case FUNC_ADDR: {
					if (addrs.find(instruct.value) != addrs.end()) {
						instruction += addrs[instruct.value];
					}
					else {
						std::string addr = scopes.get_new_tmp().addr;
						addrs[instruct.value] = addr;

						instruction += addr;
					}

					instruction += " ";
					break;
				}
				case NEXT_INST:
					bytecode.push_back(instruction);
					instruction = "";

					break;
				default:
					instruction += instruct.value;
					instruction += " ";
					break;
				}
			}



		}
		else {
			auto expr_val = stack.back();
			stack.pop_back();

			auto& func = macros_code.get_by_key(get_dfval_str(expr_val.value));

			std::unordered_map<std::string, std::string> addrs;

			if (expr_vals.size() - 1 != func.paras_addrs.size()) {
				std::cout << "more args or less args passed to the native Jua function !";
				return { DFACTION_PANIC , DFActionState(0) };
			}

			auto instructions = comp_lexer.insert_bytecode(func.macro_code);

			std::string instruction;

			size_t func_start = bytecode.size();

			for (size_t i{ 0 }; i < instructions.size(); ++i) {

				auto& instruct = instructions[i];
				switch (instruct.op_type)
				{
				case JUMP: {
					instruction += instruct.value;
					instruction += " ";

					auto& val = instructions[i + 1];

					val.value = std::to_string(std::stoull(val.value) + func_start);

					break;
				}
				case JUMPF: {
					instruction += instruct.value;
					instruction += " ";

					auto& val = instructions[i + 2];

					val.value = std::to_string(std::stoull(val.value) + func_start);

					break;
				}
				case FUNC_ADDR: {
					if (addrs.find(instruct.value) != addrs.end()) {
						instruction += addrs[instruct.value];
					}
					else {
						std::string addr = scopes.get_new_tmp().addr;
						addrs[instruct.value] = addr;

						instruction += addr;
					}

					instruction += " ";
					break;
				}
				case NEXT_INST:
					bytecode.push_back(instruction);
					instruction = "";

					break;
				default:
					instruction += instruct.value;
					instruction += " ";
					break;
				}
			}

			stack.push_back(expr_val);
		}

		scopes.destroy_scope();

		return { DFActionFlowCode::DFACTION_BACK_TO_PREV , DFActionState(0) };
	}
	default:
		break;
	}
}