#ifndef JUA_CONSOLE_IO
#define JUA_CONSOLE_IO

#include "JuaLang.h"

JuaOprand jua_print_func(std::vector<JuaStackVal>& oprands) {


	for (size_t i = 0 ; i < oprands.size() ; ++i) {

		JuaOprand* oprand;
		JuaOprand tmp;
		if (oprands[i].type == REF) {
			oprand = oprands[i].get_ptr();
		} else {
			tmp = oprands[i].get_obj();
			oprand = &tmp;
		}

		switch (oprand->op_type)
		{
		case STRING:
			std::cout << oprand->get_str();
			break;
		case DOUBLE:
			std::cout << oprand->get_doub();
			break;
		default:
			continue;
			break;
		}
	}

	std::cout << "\n";

	
	JuaOprand ret {VOID , nullptr};
	




	return ret;
}

JuaOprand jua_input_func(std::vector<JuaStackVal>& oprands) {
    jua_print_func(oprands);
    JuaOprand result;
    result.op_type = STRING;
    
    std::string input;
    std::getline(std::cin , input);
    result.value = input;

    return result;
}

#endif