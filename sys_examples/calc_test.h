#pragma once
#ifndef SAMPLE_CALC
#define SAMPLE_CALC


#define DFACTION_TYPES
#define DFACTION_STATES

enum DFActionState {
	q0, q1, q2, q3, q4, q5, q6,
};

enum DFActionType {
	CHAR,
	INT,
	OPERATOR,
	SEMICOLON,
	DIVIDE,
	WHILE,
	IF,
	ELSE,
};

union DFActionVal {
	char ch;
	int int_num;
};

#include "DFAction.h"
#include <iostream>

class CalcDFA : public DFAction {

	std::vector<DFActionToken> notebook;

protected:
	DFActionFlow action_function(
		size_t& index_in_tokens
		, const std::vector<DFActionToken>& tokens
		, DFActionState& current_state
		, bool& go_next_index
	) override {

		const DFActionToken& current_token = tokens[index_in_tokens];

		switch (current_state) {
		case q0:
			if (current_token.type != INT) {
				return DFACTION_PANIC;
			}
			else {
				notebook.push_back(current_token);
			}

			go_next_index = true;
			return DFACTION_SAFE;

		case q1:
			if (current_token.type != OPERATOR) {
				return DFACTION_PANIC;
			}
			else {
				notebook.push_back(current_token);
			}

			go_next_index = true;
			return DFACTION_SAFE;

		case q2:
			if (current_token.type != INT) {
				return DFACTION_PANIC;
			}
			else {
				notebook.push_back(current_token);
			}

			go_next_index = true;
			return DFACTION_SAFE;

		case q3:
			// NO chain
			if (current_token.type == SEMICOLON) {

				int result = 0;
				for (; 0 < notebook.size();) {
					auto& top = notebook.back();

					if (top.type == INT) {
						result = top.value.int_num;
						notebook.pop_back();
					}

					if (top.type == OPERATOR) {
						notebook.pop_back();
						auto& next_top = notebook.back();

						switch (top.value.ch)
						{
						case '+':
							result += next_top.value.int_num;
							break;

						case '-':
							result -= next_top.value.int_num;
							break;

						case '*':
							result *= next_top.value.int_num;
							break;

						case '/':
							result /= next_top.value.int_num;
							break;

						default:
							return DFACTION_PANIC;
						}


						notebook.pop_back();

						DFActionToken tmp;
						tmp.type = INT;
						tmp.value.int_num = result;

						notebook.push_back(tmp);

					}
				}

				std::cout << "calc res : " << result << '\n';

				return DFACTION_SAFE;
			}
			else if (current_token.type == OPERATOR) {

				auto do_op = [](DFActionToken& first, DFActionToken& op, DFActionToken& second) {
					DFActionToken result;
					result.type = INT;

					switch (op.value.ch)
					{
					case '+':
						result.value.int_num = first.value.int_num + second.value.int_num;
						return result;
					case '-':
						result.value.int_num = first.value.int_num - second.value.int_num;
						return result;
					case '*':
						result.value.int_num = first.value.int_num * second.value.int_num;
						return result;
					case '/':
						result.value.int_num = first.value.int_num / second.value.int_num;
						return result;
					}
					};

				auto top_index = notebook.size() - 1;

				switch (current_token.value.ch)
				{
				case '+':
					switch (notebook[top_index - 1].value.ch)
					{
					case '+':
					case '-':
					case '*':
					case '/':
						auto oprand_2 = notebook.back();
						notebook.pop_back();
						auto op = notebook.back();
						notebook.pop_back();
						auto oprand_1 = notebook.back();
						notebook.pop_back();

						notebook.push_back(do_op(oprand_1, op, oprand_2));
						break;
					default:
						return DFACTION_PANIC;
					}

					break;
				case '-':
					switch (notebook[top_index - 1].value.ch)
					{
					case '+':
					case '-':
					case '*':
					case '/':
						auto oprand_2 = notebook.back();
						notebook.pop_back();
						auto op = notebook.back();
						notebook.pop_back();
						auto oprand_1 = notebook.back();
						notebook.pop_back();

						notebook.push_back(do_op(oprand_1, op, oprand_2));
						break;
					default:
						return DFACTION_PANIC;
					}

					break;
				case '*':
					switch (notebook[top_index - 1].value.ch)
					{
					case '+':
					case '-':
						break;

					case '*':
					case '/':
						auto oprand_2 = notebook.back();
						notebook.pop_back();
						auto op = notebook.back();
						notebook.pop_back();
						auto oprand_1 = notebook.back();
						notebook.pop_back();

						notebook.push_back(do_op(oprand_1, op, oprand_2));
						break;
					default:
						return DFACTION_PANIC;
					}

					break;
				case '/':
					switch (notebook[top_index - 1].value.ch)
					{
					case '+':
					case '-':
						break;

					case '*':
					case '/':
						auto oprand_2 = notebook.back();
						notebook.pop_back();
						auto op = notebook.back();
						notebook.pop_back();
						auto oprand_1 = notebook.back();
						notebook.pop_back();

						notebook.push_back(do_op(oprand_1, op, oprand_2));
						break;
					default:
						return DFACTION_PANIC;
					}

					break;
				default:
					return DFACTION_PANIC;
				}

				notebook.push_back(current_token);
				return DFACTION_SAFE;
			}
			else {
				return DFACTION_PANIC;
			}
		}
	}

public:
	CalcDFA(DFA* states) : DFAction(states) {

	}
};

int test() {
	DFA states = {
		{q0 , {{INT , q1}}},
		{q1 , {{OPERATOR , q2} , {OPERATOR , q2} , {OPERATOR , q2} , {OPERATOR , q2}}},
		{q2 , {{INT , q3}}},
		{q3 , {{SEMICOLON , q0} ,{OPERATOR , q2}}},
	};

	CalcDFA test(&states);

	std::vector<DFActionToken> tokens = {
		{INT , 2},
		{OPERATOR , '*'},
		{INT , 2},
		{OPERATOR , '+'},
		{INT , 3},
		{SEMICOLON , 0},
	};

	test.run_dfa_on(tokens, q0);
}

#endif
