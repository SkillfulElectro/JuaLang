#pragma once
#ifndef JUALANG
#define JUALANG

#include "JuaLangTypes.h"
#include "JuaNativeFunc.h"
#include "../JuaInterpter/JuaInterpter.h"
#include "JuaMacroScope.h"

using Preprocessor = std::function<std::string(std::vector<DFActionToken>&)>;

class JuaLang : public DFAction {

	/// <summary>
	/// types
	/// </summary>
private:
	std::vector<std::string> bytecode;

	std::vector<DFActionToken> stack;

	JuaScope scopes;

	DFMatcher lexer;
	DFA dfa;

	JuaCompileTimeLexer comp_lexer;

	JuaMacroScopeManager macros_code;

	JuaInterpter* interpter;

	std::vector<DFActionToken> preprocessor_tokens;
	bool preprocessing_time;
	std::string preprocessed_value;
public:
	std::unordered_map<std::string , Preprocessor> preprocessors;
	
	
	/// <summary>
	/// funcs
	/// </summary>
private:

	std::string get_dfval_str(DFActionVal val);

	double get_dfval_doub(DFActionVal val);

	size_t get_sizet(DFActionVal val);

	void init_dfaction();

	void init_lexer();


	DFActionFlow start_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow identer_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	/// <summary>
	/// Deprected but kept for future use cases
	/// </summary>
	/// <param name="index_in_tokens"></param>
	/// <param name="tokens"></param>
	/// <param name="go_next_index"></param>
	/// <returns></returns>
	DFActionFlow func_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow expr_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow expr_ops_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow expr_para_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow expr_func_router_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow func_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow dot_operator_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);


	DFActionFlow return_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow while_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow while_scope_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow if_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);


	DFActionFlow if_scope_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow else_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow elif_chain_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow scope_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow macro_def_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow macro_def_ident_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow macro_def_para_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow macro_para_vars_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow macro_def_scope_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow macro_call_handler_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

	DFActionFlow preprocessor_handler(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index);

protected:

	DFActionFlow action_function(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, DFActionState& current_state
		, bool& go_next_index
	) override;

public:
	JuaLang();

	void set_interpter(JuaInterpter* interpter) ;

	void unset_interpter();

	std::string compile(std::string& buffer);
};

#endif // !JUALANG