#include "JuaInterpter.h"

class zed : public JuaModule {
public:
	zed(const std::string& func_name, JuaInterpter* instance) : JuaModule(func_name , instance) {}
	JuaOprand jua_extension_func(std::vector<JuaOprand> oprands) override {
		std::cout << "its test";

		for (size_t i{ 0 }; i < oprands.size(); ++i) {
			std::cout << oprands[i].get_doub();
		}

		std::cout << '\n';

		return JuaOprand(DFActionType(0), 0.0);
	}
};

int main() {
	std::string code = R"(= #10 ; 1
= 1 ; 0
= #1 ; 2
- 0 2 3
jmpf 3 11 ;
push 0 ; ;
call bye 1 4
= #1 ; 5
- 0 5 6
= 6 ; 0
jmp 2 ; ;
push 0 ; ;
ret 1 ; ;
)";

	JuaInterpter instance(code);
	zed test1("zed", &instance);
	zed test2("bye", &instance);

	instance.run_instructions();

	return 0;
}
