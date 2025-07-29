#include "JuaLang.h"

std::string JuaLang::get_dfval_str(DFActionVal val) {
	return std::get<std::string>(val);
}

double JuaLang::get_dfval_doub(DFActionVal val) {
	return std::get<double>(val);
}

size_t JuaLang::get_sizet(DFActionVal val) {
	return std::get<size_t>(val);
}

DFActionFlow JuaLang::action_function(
	size_t& index_in_tokens
	, const std::vector<DFActionToken>& tokens
	, DFActionState& current_state
	, bool& go_next_index
) {


	switch (current_state)
	{
	case START:
		return start_action(index_in_tokens, tokens, go_next_index);
	case IDENTER:
		return identer_action(index_in_tokens, tokens, go_next_index);

		/// deprected but kept for future use cases
	case FUNC:
		return func_action(index_in_tokens, tokens, go_next_index);
	case EXPR_OPS:
		return expr_ops_action(index_in_tokens, tokens, go_next_index);
	case EXPR:
		return expr_action(index_in_tokens, tokens, go_next_index);
	case EXPR_PARA:
		return expr_para_action(index_in_tokens, tokens, go_next_index);
	case EXPR_FUNC_ROUTER:
		return expr_func_router_action(index_in_tokens, tokens, go_next_index);
	case FUNC_HANDLER:
		return func_handler_action(index_in_tokens, tokens, go_next_index);
	case DOT_OPERATOR_HANDLER:
		return dot_operator_handler_action(index_in_tokens, tokens, go_next_index);
	case RETURN_HANDLER:
		return return_handler_action(index_in_tokens, tokens, go_next_index);
	case WHILE_HANDLER:
		return while_handler_action(index_in_tokens, tokens, go_next_index);
	case WHILE_SCOPE:
		return while_scope_handler_action(index_in_tokens, tokens, go_next_index);
	case IF_HANDLER:
		return if_handler_action(index_in_tokens, tokens, go_next_index);
	case IF_SCOPE:
		return if_scope_handler_action(index_in_tokens, tokens, go_next_index);
	case SCOPE_HANDLER:
		return scope_handler_action(index_in_tokens, tokens, go_next_index);
	case MACRO_DEF_HANDLER:
		return macro_def_handler_action(index_in_tokens, tokens, go_next_index);
	case MACRO_DEF_IDENT:
		return macro_def_ident_action(index_in_tokens, tokens, go_next_index);
	case MACRO_PARA_HANDLER:
		return macro_def_para_action(index_in_tokens, tokens, go_next_index);
	case MACRO_PARA_VARS:
		return macro_para_vars_action(index_in_tokens, tokens, go_next_index);
	case MACRO_DEF_SCOPE:
		return macro_def_scope_action(index_in_tokens, tokens, go_next_index);
	case MACRO_CALL_HANDLER:
		return macro_call_handler_action(index_in_tokens, tokens, go_next_index);
	case ELIF_CHAIN_HANLDER:
		return elif_chain_handler_action(index_in_tokens, tokens, go_next_index);
	case ELSE_HANDLER:
		return else_handler_action(index_in_tokens, tokens, go_next_index);
	case PREPROCESSOR_HANDLER:
		return preprocessor_handler(index_in_tokens, tokens, go_next_index);
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

std::string JuaLang::compile(std::string& buffer) {
	size_t index = 0;

	DFMatcherRes res;

	std::vector<DFActionToken> tokens;

	size_t index_in_tks = 0;
	auto ctx = this->get_default_context();

	DFActionReturnVal comp_res;

	macros_code.create_scope();
	scopes.create_new_scope();

	do {
		res = lexer.get_token(buffer, index);


		DFActionToken tok;
		tok.value = std::string(res.value);
		tok.type = DFActionType(res.token_identifier);

		tokens.push_back(tok);

		comp_res = this->run_dfa_on(tokens, START , &index_in_tks , &ctx);

		switch (comp_res.status)
		{
		case DFActionReturnCode::PANIC_WHILE_PROCESSING : 
			std::cout << "COMPILATION FAILED !";
		break;
		}

		if (preprocessing_time) {
			buffer.insert(index , preprocessed_value);
			preprocessing_time = false;
		}
 
	} while (res.status != END_OF_FILE);

	macros_code.destroy_scope();
	scopes.destroy_scope();
	

	if (comp_res.status == FAILED_TO_DO_ALL_REDUCTIONS) {
		std::cout << "COMPILATION FAILED !\n";
	}

	std::string result = "";

	for (auto val : bytecode) {
		result += val;
		result += '\n';
	}

	return result;
}