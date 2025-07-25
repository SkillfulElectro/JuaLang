#include "JuaLang.h"
#include "std/math/JuaStdMath.h"
#include "std/console/io.h"
#include "std/type/jua_str.h"
#include "std/structures/vector.h"




int main() {
	JuaLang cinstance;
	JuaInterpter instance;
	instance.add_extension("print", jua_print_func);
	instance.add_extension("input" , jua_input_func);
	instance.add_extension("import_math" , jua_std_math_importer);
	instance.add_extension("import_str_util" , jua_create_string_util);
	instance.add_extension("create_vector" , jua_create_vector);
	cinstance.set_interpter(&instance);

	std::cout << "starting to compile \n";

	std::string code = cinstance.compile(
		R"(
math = import_math();
str_util = import_str_util();



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
fib(input("insert index : ") , ret);

print(str_util.get_from_index(123 , 0 ));
print(str_util.len("ewufhwehf"));
alpha = "hi im xd";
str_util.set_to_index(alpha , 1 , "b");
print(alpha);

vec = create_vector(1 , 2 , 3 , 4);
print("vec size : " , vec.size());

return ret;
)");
	
	std::cout << code << '\n';

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub() << "\n";

	return 0;
}