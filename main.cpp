#include "JuaLang.h"
#include "std/math/JuaStdMath.h"


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


int main() {
	JuaLang cinstance;
	JuaInterpter instance;
	instance.add_extension("print", jua_print_func);
	instance.add_extension("import_math" , jua_std_math_importer);
	cinstance.set_interpter(&instance);

	std::cout << "starting to compile \n";

	std::string code = cinstance.compile(
		R"(
math = import_math();




macro fib(index , ret) {
	if (math.equal(index , 0)) {
		ret = 0;
	} else if (math.equal(index , 1)) {
		ret = 1;
	} else if (math.greater_than(index , 1)) {
		first = 0;
		sec = 1;


		count = 0;
		while (math.less_than(count , index)) {
			tmp = first + sec;
			first = sec;
			sec = tmp;

			count = count + 1;

			print("first : " , first , " sec : " , sec , " count : " , count);
		}

		ret = first;
	}
}


ret = 0;
fib(3 , ret);

return ret;
)");
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}