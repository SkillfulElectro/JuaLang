#include "JuaLang.h"




int main() {
	JuaLang cinstance;
	cinstance.preprocessors["import"] = import;
	cinstance.preprocessors["include"] = import;
	
	JuaInterpter instance;
	instance.add_extension("print", jua_print_func);
	instance.add_extension("input" , jua_input_func);
	instance.add_extension("import_math" , jua_std_math_importer);
	instance.add_extension("str" , jua_create_string_util);
	instance.add_extension("vector" , jua_create_vector);
	instance.add_extension("hashmap" , jua_create_hash_map);
	cinstance.set_interpter(&instance);

	std::cout << "starting to compile \n";

	std::string jua_code = R"(
#import("../main.jua");
)";
	std::string code = cinstance.compile(jua_code);
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}