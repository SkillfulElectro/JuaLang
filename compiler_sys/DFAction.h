#pragma once
#ifndef MUTEXIS_DFACTION
#define MUTEXIS_DFACTION

#include <unordered_map>

#include "DFAction_types.h"

enum DFActionFlow {
	DFACTION_SAFE = 0,
	DFACTION_DO_NOT_CHANGE_STATE = -1,
	DFACTION_PANIC = -2,
};

struct DFActionToken {
	DFActionType type;
	DFActionVal value;
};

typedef std::unordered_map<DFActionState, std::unordered_map<DFActionType, DFActionState>> DFA;

class DFAction {
	
protected:
	DFA* machine;

	virtual DFActionFlow action_function(
		size_t& index_in_tokens 
		, const std::vector<DFActionToken>& tokens
		, DFActionState& current_state
		, bool& go_next_index
	) = 0;

public:
	DFAction(DFA* machine = nullptr) {
		this->machine = machine;
	}

	void new_dfa(DFA* states) {
		this->machine = states;
	}

	

	void run_dfa_on(const std::vector<DFActionToken>& tokens 
		, const DFActionState& start_state) {

		if (machine == nullptr) {
			return;
		}

		auto& dfa = *machine;

		DFActionState state = start_state;

		for (size_t index{ 0 }; index < tokens.size();) {

			bool next_index = true;

			DFActionFlow change_state = action_function(index , tokens , state, next_index);

			if (change_state == DFACTION_PANIC) {
				break;
			}
			else if (change_state != DFACTION_DO_NOT_CHANGE_STATE) {
				state =  dfa[state][tokens[index].type];
			}

			if (next_index)
				++index;
		}
	}
};

#endif