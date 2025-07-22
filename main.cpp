#include "JuaLang.h"
#include "std/math/JuaStdMath.h"

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
			std::cout << oprand->get_str() << "\n";
			break;
		case DOUBLE:
			std::cout << oprand->get_doub() << "\n";
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
	instance.add_extension("import_math" , jua_std_math_importer);
	cinstance.set_interpter(&instance);

	std::cout << "starting to compile \n";

	std::string code = cinstance.compile(
		R"(
math = import_math();

if (math.equal(1.1 , 1.1)) {
	print(math.pow(2 , 4));
}


return 1.0;
)");
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}