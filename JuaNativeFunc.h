#pragma once
#ifndef JUANATIVE_FUNC
#define JUANATIVE_FUNC

#include "JuaLangTypes.h"


struct JuaCompileOprand {
	DFActionType op_type;
	std::variant<size_t, double, std::string, void*> value;

	size_t get_sizet() {
		return std::get<size_t>(value);
	}

	double get_doub() {
		return std::get<double>(value);
	}

	std::string get_str() {
		return std::get<std::string>(value);
	}

	void* get_void_ptr() {
		return std::get<void*>(value);
	}
};

class JuaCompileTimeLexer {
	DFMatcher lexer;

	inline JuaCompileOprand convert_DFMatcherRes(const DFMatcherRes& res) {
		JuaCompileOprand oprand;

		switch (res.token_identifier)
		{
		default:
			oprand.op_type = res.token_identifier;
			oprand.value = res.value;

			break;
		}

		return oprand;
	}

public:
	JuaCompileTimeLexer() {
		TokenDFA ident_func;
		ident_func.start_state = "0";
		ident_func.token_identifier = FUNC_IDENT;
		ident_func.ignore = false;
		ident_func.add_ASCII_range("0", 'a', 'z', "1");
		ident_func.add_ASCII_range("0", 'A', 'Z', "1");
		ident_func.dfa["0"]['_'] = "1";
		ident_func.add_ASCII_range("1", 'a', 'z', "1");
		ident_func.add_ASCII_range("1", 'A', 'Z', "1");
		ident_func.add_ASCII_range("1", '0', '9', "1");
		ident_func.dfa["1"]['_'] = "1";
		ident_func.add_final_state("1");
		lexer.insert_token(ident_func);

		lexer.create_word_token("push", PUSH, false);
		lexer.create_word_token("pop", POP, false);
		lexer.create_word_token("call", CALL, false);
		lexer.create_word_token("=", ASSIGN, false);
		lexer.create_word_token("+", PLUS, false);
		lexer.create_word_token("-", NEGATIVE, false);
		lexer.create_word_token("/", DIVIDE, false);
		lexer.create_word_token("*", MULTI, false);

		TokenDFA addr;
		addr.start_state = "0";
		addr.token_identifier = ADDR;
		addr.ignore = false;
		addr.add_ASCII_range("0", '0', '9', "1");
		addr.add_ASCII_range("1", '0', '9', "1");;
		addr.dfa["1"]['.'] = "2";
		addr.add_ASCII_range("2", '0', '9', "3");
		addr.add_ASCII_range("3", '0', '9', "3");
		addr.add_final_state("1");
		addr.add_final_state("3");
		lexer.insert_token(addr);



		TokenDFA num;
		num.start_state = "num";
		num.token_identifier = DOUBLE;
		num.ignore = false;
		num.dfa["num"]['#'] = "0";
		num.add_ASCII_range("0", '0', '9', "1");
		num.add_ASCII_range("1", '0', '9', "1");;
		num.dfa["1"]['.'] = "2";
		num.add_ASCII_range("2", '0', '9', "3");
		num.add_ASCII_range("3", '0', '9', "3");
		num.add_final_state("1");
		num.add_final_state("3");
		lexer.insert_token(num);

		TokenDFA func_addr;
		func_addr.start_state = "num";
		func_addr.token_identifier = FUNC_ADDR;
		func_addr.ignore = false;
		func_addr.dfa["num"]['$'] = "0";
		func_addr.add_ASCII_range("0", '0', '9', "1");
		func_addr.add_ASCII_range("1", '0', '9', "1");;
		func_addr.dfa["1"]['.'] = "2";
		func_addr.add_ASCII_range("2", '0', '9', "3");
		func_addr.add_ASCII_range("3", '0', '9', "3");
		func_addr.add_final_state("1");
		func_addr.add_final_state("3");
		lexer.insert_token(func_addr);

		TokenDFA str;
		str.start_state = "0";
		str.ignore = false;
		str.token_identifier = STRING;
		str.dfa["0"]['\"'] = "1";
		str.add_ASCII_range("1", char(0), char(127), "1");
		str.dfa["1"]['\"'] = "2";
		str.dfa["1"]['\\'] = "3";
		str.add_ASCII_range("3", char(0), char(127), "1");
		str.add_final_state("2");
		lexer.insert_token(str);

		lexer.create_word_token(" ", IGNORES, true);
		lexer.create_word_token("\n", NEXT_INST, false);
		lexer.create_word_token(";", EMPTY, false);
		lexer.create_word_token("jmp", JUMP, false);
		lexer.create_word_token("jmpf", JUMPF, false);
		lexer.create_word_token("jmpt", JUMPT, false);
		lexer.create_word_token("ret", RETURN, false);

	}

	inline std::vector<JuaCompileOprand> insert_bytecode(const std::string& juax_code) {
		std::vector<JuaCompileOprand> instructions;

		size_t index = 0;
		DFMatcherRes res;



		do {
			res = lexer.get_token(juax_code, index);



			instructions.push_back(convert_DFMatcherRes(res));

		} while (res.status != END_OF_FILE);


		return instructions;
	}
};


#endif // !JUANATIVE_FUNC
