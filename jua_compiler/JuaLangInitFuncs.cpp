#include "JuaLang.h"

void JuaLang::init_dfaction() {
	dfa[START][IDENT] = IDENTER;
	dfa[START][CONTINUE] = START;
	dfa[START][BREAK] = START;
	dfa[IDENTER][SEMICOLON] = START;

	this->add_special_dfa(START, dfa);

	DFA function_def_handler;
	function_def_handler[FUNCTION_DEF_HANDLER][FUNCTION] = FUNCTION_DEF_IDENT;
	function_def_handler[FUNCTION_DEF_IDENT][IDENT] = FUNCTION_PARA_HANDLER;
	function_def_handler[FUNCTION_PARA_HANDLER][CLOSE_PARAN] = FUNCTION_DEF_SCOPE;
	this->add_special_dfa(FUNCTION_DEF_HANDLER, function_def_handler);

	DFA func_para_vars;
	this->add_special_dfa(FUNC_PARA_VARS, func_para_vars);

	DFA while_handler;
	while_handler[WHILE_HANDLER][CLOSE_PARAN] = WHILE_SCOPE;
	this->add_special_dfa(WHILE_HANDLER, while_handler);

	DFA if_handler;
	if_handler[IF_HANDLER][CLOSE_PARAN] = IF_SCOPE;
	this->add_special_dfa(IF_HANDLER, if_handler);

	DFA if_scope_handler;
	this->add_special_dfa(IF_SCOPE, if_scope_handler);

	DFA chain_handler;
	chain_handler[CHAIN_HANLDER][ELSE] = ELSE_HANDLER;
	chain_handler[ELSE_HANDLER][IF] = CHAIN_HANLDER;
	this->add_special_dfa(CHAIN_HANLDER, chain_handler);


	DFA n_func_handler;
	this->add_special_dfa(N_FUNC_HANDLER, n_func_handler);

	DFA func_handler;
	this->add_special_dfa(FUNC_HANDLER, func_handler);

	DFA scope_handler;
	this->add_special_dfa(SCOPE_HANDLER, scope_handler);

	DFA expr;
	expr[EXPR][OPERATOR] = EXPR_OPS;
	expr[EXPR_OPS][IDENT] = EXPR;
	expr[EXPR_OPS][CLOSE_PARAN] = EXPR;
	expr[EXPR_OPS][CONST_DOUBLE] = EXPR;
	expr[EXPR_OPS][CONST_STRING] = EXPR;
	this->add_special_dfa(EXPR_OPS, expr);

	DFA ret;
	this->add_special_dfa(RETURN_HANDLER, ret);

	DFA expr_para;
	this->add_special_dfa(EXPR_PARA, expr_para);

	new_dfa(&dfa);
}

void JuaLang::init_lexer() {

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

	lexer.create_word_token("function", FUNCTION, false);

	lexer.create_word_token("if", IF, false);
	lexer.create_word_token("else", ELSE, false);
	lexer.create_word_token("/", OPERATOR, false);
	lexer.create_word_token("*", OPERATOR, false);
	lexer.create_word_token("+", OPERATOR, false);
	lexer.create_word_token("-", OPERATOR, false);
	lexer.create_word_token("=", ASSIGN, false);
	lexer.create_word_token("while", WHILE, false);
	lexer.create_word_token("continue", CONTINUE, false);
	lexer.create_word_token("break", BREAK, false);
	lexer.create_word_token("(", OP_PARAN, false);
	lexer.create_word_token(")", CLOSE_PARAN, false);
	lexer.create_word_token("{", OP_BRACE, false);
	lexer.create_word_token("}", CLOSE_BRACE, false);
	lexer.create_word_token(";", SEMICOLON, false);
	lexer.create_word_token(",", CAMMA, false);
}