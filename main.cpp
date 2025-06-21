#include "JuaLang.h"

class zed : public JuaModule {
public:
	JuaOprand jua_extension_func(std::vector<JuaStackVal>& oprands) {

		auto& oprand = *oprands[0].get_ptr();

		switch (oprand.op_type)
		{
		case STRING:
			std::cout << oprand.get_str() << "\n";
			break;
		case DOUBLE:
			std::cout << oprand.get_doub() << "\n";
			break;
		}


		return { DOUBLE , 1.0 };
	}
};

int main() {
	JuaLang cinstance;
	JuaInterpter instance;
	zed test2;
	instance.add_extension("print", &zed::jua_extension_func, &test2);
	cinstance.set_interpter(&instance);

	std::cout << "starting to compile \n";

	std::string code = cinstance.compile(
		R"(
hi = 3;

macro goz(z) {
	hi = z + hi;
	print(hi);
}

goz(1);



return hi;
)");
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}