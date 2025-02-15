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
	case FUNCTION_DEF_HANDLER:
		return function_def_handler_action(index_in_tokens, tokens, go_next_index);
	case FUNCTION_DEF_IDENT:
		return function_def_ident_action(index_in_tokens, tokens, go_next_index);
	case FUNCTION_PARA_HANDLER:
		return function_def_para_action(index_in_tokens, tokens, go_next_index);
	case FUNC_PARA_VARS:
		return function_para_vars_action(index_in_tokens, tokens, go_next_index);
	case FUNCTION_DEF_SCOPE:
		return function_def_scope_action(index_in_tokens, tokens, go_next_index);
	case N_FUNC_HANDLER:
		return n_func_handler_action(index_in_tokens, tokens, go_next_index);
	case CHAIN_HANLDER:
		return chain_handler_action(index_in_tokens, tokens, go_next_index);
	case ELSE_HANDLER:
		return else_handler_action(index_in_tokens, tokens, go_next_index);
	default:
		break;
	}

	return { DFACTION_SAFE , DFActionState(0) };
}

std::string JuaLang::compile(const std::string& buffer) {
	size_t index = 0;

	DFMatcherRes res;

	std::vector<DFActionToken> tokens;

	do {
		res = lexer.get_token(buffer, index);


		DFActionToken tok;
		tok.value = res.value;
		tok.type = res.token_identifier;

		tokens.push_back(tok);
	} while (res.status != END_OF_FILE);

	auto comp_res = this->run_dfa_on(tokens, START);

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