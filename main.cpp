#include "JuaLang.h"

class TestVoid : public JuaVoidType {
public:
	JuaOprand run_func_by_symbol(const std::string& name 
		, std::vector<JuaStackVal>& oprands) override {
		

		std::cout << "void type func \n";
		JuaOprand ret {DOUBLE , 1.1};

		return ret;
	}
};

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
		default:
			continue;
			break;
		}
	}

	
	JuaOprand ret {VOID , new TestVoid};
	ret.destructor = [](JuaOprand* obj) {
		delete obj->get_void_ptr();
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
z = print(1);

hi = z.smth() + 2;


return hi;
)");
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}