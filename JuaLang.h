#pragma once
#ifndef JUALANG
#define JUALANG

#define DFACTION_TYPES
#define DFACTION_STATES

#include <string>
#include <variant>


enum DFActionState {
	START , EXPR , IDENTER , FUNC , IDENT_EXPR , EXPR_OPS , EXPR_PARA , FUNC_HANDLER
};

enum DFActionType {
	CONST_STRING,
	CONST_DOUBLE,
	OPERATOR,
	SEMICOLON,
	DIVIDE,
	WHILE,
	IF,
	ELSE,
	IDENT,
	OP_BRACE,
	CLOSE_BRACE,
	OP_PARAN,
	CLOSE_PARAN,
	RETURN,
	IGNORES,
	CAMMA,
	ADDR,
	ASSIGN,
};

using DFActionVal = std::variant<std::string, double>;


#include "lexer_sys/DFMatcher.h"
#include "compiler_sys/DFAction.h"
#include "JuaScope.h"


class JuaLang : public DFAction {
	std::vector<std::string> bytecode;

	std::vector<DFActionToken> stack;
	
	std::vector<JuaScope> scopes;

	DFMatcher lexer;
	DFA dfa;

	std::string get_dfval_str(DFActionVal val) {
		return std::get<std::string>(val);
	}
	
	double get_dfval_doub(DFActionVal val) {
		return std::get<double>(val);
	}

	void init_dfaction() {
		dfa[START][IDENT] = IDENTER;
		dfa[IDENTER][SEMICOLON] = START;

		dfa[FUNC][CLOSE_PARAN] = FUNC;
		dfa[FUNC][SEMICOLON] = START;

		DFA func_handler;
		this->add_special_dfa(FUNC_HANDLER, func_handler);

		DFA expr;
		expr[EXPR][OPERATOR] = EXPR_OPS;
		expr[EXPR_OPS][IDENT] = EXPR;
		expr[EXPR_OPS][CLOSE_PARAN] = EXPR;
		expr[EXPR_OPS][CONST_DOUBLE] = EXPR;
		expr[EXPR_OPS][CONST_STRING] = EXPR;
		this->add_special_dfa(EXPR_OPS, expr);
		
		DFA expr_para;
		this->add_special_dfa(EXPR_PARA, expr_para);

		new_dfa(&dfa);
	}

	void init_lexer() {
		TokenDFA ident;
		ident.start_state = "0";
		ident.token_identifier = IDENT;
		ident.ignore = false;
		ident.add_ASCII_range("0", 'a', 'z', "1");
		ident.add_ASCII_range("0", 'A', 'Z', "1");
		ident.dfa["0"]['_'] = "1";
		ident.add_ASCII_range("1", 'a', 'z', "1");
		ident.add_ASCII_range("1", 'A', 'Z', "1");
		ident.add_ASCII_range("1", '0', '9', "1");
		ident.dfa["1"]['_'] = "1";
		ident.add_final_state("1");
		lexer.insert_token(ident);

		TokenDFA str;
		str.start_state = "0";
		str.ignore = false;
		str.token_identifier = CONST_STRING;
		str.dfa["0"]['\"'] = "1";
		str.add_ASCII_range("1", char(0), char(127), "1");
		str.dfa["1"]['\"'] = "2";
		str.dfa["1"]['\\'] = "3";
		str.add_ASCII_range("3", char(0), char(127), "1");
		str.add_final_state("2");
		lexer.insert_token(str);

		TokenDFA num;
		num.start_state = "0";
		num.token_identifier = CONST_DOUBLE;
		num.ignore = false;
		num.add_ASCII_range("0", '0', '9', "1");
		num.add_ASCII_range("1", '0', '9', "1");;
		num.dfa["1"]['.'] = "2";
		num.add_ASCII_range("2", '0', '9', "3");
		num.add_ASCII_range("3", '0', '9', "3");
		num.add_final_state("1");
		num.add_final_state("3");
		lexer.insert_token(num);


		TokenDFA comment;
		comment.ignore = true;
		comment.start_state = "0";
		comment.dfa["0"]['/'] = "1";
		comment.dfa["1"]['*'] = "2";
		comment.add_ASCII_range("2", char(0), char(127), "2");
		comment.dfa["2"]['*'] = "3";
		comment.add_ASCII_range("3", char(0), char(127), "2");
		comment.dfa["3"]['/'] = "4";
		comment.add_final_state("4");
		comment.token_identifier = IGNORES;

		lexer.create_word_token(" ", IGNORES, true);
		lexer.create_word_token("\n", IGNORES, true);
		lexer.create_word_token("\r", IGNORES, true);
		lexer.create_word_token("\t", IGNORES, true);

		lexer.insert_token(comment);


		lexer.create_word_token("return", RETURN, false);
		lexer.create_word_token("if", IF, false);
		lexer.create_word_token("else", ELSE, false);
		lexer.create_word_token("/", OPERATOR, false);
		lexer.create_word_token("*", OPERATOR, false);
		lexer.create_word_token("+", OPERATOR, false);
		lexer.create_word_token("-", OPERATOR, false);
		lexer.create_word_token("=", ASSIGN, false);
		lexer.create_word_token("while", WHILE, false);
		lexer.create_word_token("(", OP_PARAN, false);
		lexer.create_word_token(")", CLOSE_PARAN, false);
		lexer.create_word_token("{", OP_BRACE, false);
		lexer.create_word_token("}", CLOSE_BRACE, false);
		lexer.create_word_token(";", SEMICOLON, false);
		lexer.create_word_token(",", CAMMA, false);
	}


	DFActionFlow start_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index) {

		auto& token = tokens[index_in_tokens];

		switch (token.type)
		{
		case IDENT: {
			stack.push_back(token);
			break;
		}
		default:
			break;
		}

		return { DFACTION_SAFE , DFActionState(0) };
	}

	DFActionFlow identer_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index) {

		auto& token = tokens[index_in_tokens];

		switch (token.type)
		{
		case OP_PARAN: {
			go_next_index = false;

			return { DFACTION_GO_TO_SP_DFA , FUNC_HANDLER };
		}

		case ASSIGN: {
			auto identi = stack.back();
			stack.pop_back();

			std::string addr = std::to_string(scopes.back().get_new_addr(get_dfval_str(identi.value)).addr);

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
		}
		default:
			break;
		}

		return { DFACTION_SAFE , DFActionState(0) };
	}

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
	
	DFActionFlow expr_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index) {

		auto& token = tokens[index_in_tokens];
		auto& prev_op_token = stack[stack.size() - 2];

		if (prev_op_token.type ==  ASSIGN && token.type == OPERATOR) {
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

			auto res = scopes.back().get_ident_name(std::stoull(get_dfval_str(iden.value)));

			if (res.status == JuaScopeStatus::JSCOPE_NOT_FOUND) {
				return { DFACTION_PANIC , DFActionState(0) };
			}

			stack.push_back({
				IDENT , res.ident_name
				});

			return { DFActionFlowCode::DFACTION_GO_TO_SP_DFA , FUNC_HANDLER };
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
					std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

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
					std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

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
					std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

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
					std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

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

			for (; top.type != ASSIGN;) {
				switch (top.type)
				{
				case ADDR:
					tmp_keeper = top;
					top = stack.back();
					stack.pop_back();
					break;

				case OPERATOR: {
					std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

					auto oprand1 = stack.back();
					stack.pop_back();

					code << get_dfval_str(top.value) << " " << get_dfval_str(oprand1.value) << " " << get_dfval_str(tmp_keeper.value) << " " << addr;
					bytecode.push_back(code.str());
					code.str("");
					
					tmp_keeper = {ADDR , addr};

					top = stack.back();
					stack.pop_back();

					break;
				}

				
				default:
					break;
				}

				
			}

			stack.push_back({ ASSIGN , "="});
			stack.push_back(tmp_keeper);
			


			go_next_index = false;
			return { DFACTION_BACK_TO_PREV , DFActionState(0) };
		}
		}

		return { DFACTION_SAFE , DFActionState(0) };
	}

	DFActionFlow expr_ops_action(
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
			std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

			code << "=" << " " << "#" << get_dfval_str(token.value) << " " << ";"  << " " << addr;
			bytecode.push_back(code.str());

			stack.push_back({ADDR , addr});

			break;
		}
		case CONST_STRING: {
			std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

			code << "=" << " " << get_dfval_str(token.value) << " " << ";" << " " << addr;
			bytecode.push_back(code.str());

			stack.push_back({ ADDR , addr });

			break;
		}
		case IDENT: {
			std::string addr = std::to_string(scopes.back().get_new_addr(get_dfval_str(token.value)).addr);

			stack.push_back({ ADDR , addr });

			break;
		}
		default:
			break;
		}

		return { DFACTION_SAFE , DFActionState(0) };
	}

	DFActionFlow expr_para_action(
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

	DFActionFlow func_handler_action(
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

				std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

				code << "call" << " " << get_dfval_str(identi.value) << " " << get_dfval_doub(counter.value) << " " << addr;
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

				std::string addr = std::to_string(scopes.back().get_new_tmp().addr);

				code << "call" << " " << get_dfval_str(identi.value) << " " << get_dfval_doub(counter.value) << " " << addr;
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

protected:
	DFActionFlow action_function(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, DFActionState& current_state
		, bool& go_next_index
	) override {
		

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
		case FUNC_HANDLER:
			return func_handler_action(index_in_tokens, tokens, go_next_index);
		default:
			break;
		}

		return { DFACTION_SAFE , DFActionState(0) };
	}

public:
	JuaLang() {
		init_lexer();
		init_dfaction();
		scopes.push_back(JuaScope());
	}

	~JuaLang() {
		for (auto code : bytecode) {
			std::cout << code << '\n';
		}
	}

	std::vector<std::string> compile(const std::string& buffer) {
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

		this->run_dfa_on(tokens, START);

		return bytecode;
	}
};

#endif // !JUALANG
