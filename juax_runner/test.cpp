#include "JuaInterpter.h"
#include <cmath>

class zed : public JuaModule {
public:
	zed(const std::string& func_name, JuaInterpter* instance) : JuaModule(func_name , instance) {}
	JuaOprand jua_extension_func(std::vector<JuaStackVal>& oprands) override {
		std::cout << (*oprands[0].get_ptr()).get_doub() << "\n";

		return { VOID , 0.0 };
	}
};

int main() {
	std::string code = R"(= #1 ; 1
= 1 ; 0
jmpf 0 10 ;
= #1 ; 2
+ 0 2 3
= 3 ; 0
push 0 ; ;
call print 1 4
jmp 2 ; ;
jmp 2 ; ;
push 0 ; ;
ret 1 ; ;
)";

	JuaInterpter instance(code);
	zed test2("print", &instance);
	
	std::cout << instance.run_instructions()[0].get_doub();

	return 0;
}
