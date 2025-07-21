#include "JuaLang.h"

JuaOprand jua_extension_func(std::vector<JuaStackVal>& oprands) {


	for (size_t i = 0 ; i < oprands.size() ; ++i) {
		auto& oprand = *oprands[i].get_ptr();

		switch (oprand.op_type)
		{
		case STRING:
			std::cout << oprand.get_str() << "\n";
			break;
		case DOUBLE:
			std::cout << oprand.get_doub() << "\n";
			break;
		}
	}

	
	JuaOprand ret {VOID , nullptr};
	ret.destructor = [](JuaOprand*) {
		std::cout << "cleaning ! \n";
	};
	




	return ret;
}

int main() {
	JuaLang cinstance;
	JuaInterpter instance;
	instance.add_extension("print", jua_extension_func);
	cinstance.set_interpter(&instance);

	std::cout << "starting to compile \n";

	std::string code = cinstance.compile(
		R"(
hi = 3;

macro doz(z) {
	hi = hi + z;

	macro boz() {
		hi = hi + 5;
	}
	
	boz();
}

doz(hi);
z = print(1 , 2 , 3 , 4 , 5);
z = print(2);

bye = hi;


return hi;
)");
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}