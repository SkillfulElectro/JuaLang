#pragma once
#ifndef JUALANG
#define JUALANG

#define DFACTION_TYPES
#define DFACTION_STATES

#include <string>
#include <variant>

enum DFActionState {
	START , EXPR , ASSIGN , IDENTER , FUNC
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
};

using DFActionVal = std::variant<std::string, double>;


#include "compiler_sys/DFMatcher.h"
#include "lexer_sys/DFAction.h"
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
		dfa[IDENTER][OP_PARAN] = FUNC;
		dfa[FUNC][CAMMA] = FUNC;
		dfa[FUNC][IDENT] = FUNC;
		dfa[FUNC][CONST_DOUBLE] = FUNC;
		dfa[FUNC][CONST_STRING] = FUNC;
		dfa[FUNC][CLOSE_PARAN] = FUNC;
		dfa[FUNC][SEMICOLON] = START;

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
		lexer.create_word_token("=", OPERATOR, false);
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
			stack.push_back({ OP_PARAN , 0.0 });
			break;
		}
		default:
			break;
		}

		return { DFACTION_SAFE , DFActionState(0) };
	}

	DFActionFlow func_action(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, bool& go_next_index) {

		auto& token = tokens[index_in_tokens];

		std::ostringstream code;

		switch (token.type)
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
		}

		

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
		case FUNC:
			return func_action(index_in_tokens, tokens, go_next_index);
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

	void compile(const std::string& buffer) {
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
	}
};

#endif // !JUALANG
