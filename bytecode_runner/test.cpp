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
	std::string code = R"(call zed 0 1
= #4 ; 2
= #2 ; 3
/ 2 3 4
= #3 ; 5
* 4 5 6
+ 1 6 7
= 7 ; 0
push 0 ; ;
= #2 ; 8
push 8 ; ;
= #4 ; 9
push 9 ; ;
call bye 3 10
)";

	JuaInterpter instance(code);
	zed test1("zed", &instance);
	zed test2("bye", &instance);

	instance.run_instructions();

	return 0;
}
