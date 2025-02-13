#include "JuaInterpter.h"
#include <cmath>

class zed : public JuaModule {
public:
	zed(const std::string& func_name, JuaInterpter* instance) : JuaModule(func_name , instance) {}
	JuaOprand jua_extension_func(std::vector<JuaStackVal>& oprands) override {
		
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
		

		return { VOID , 0.0 };
	}
};

int main() {
	std::string code = R"(= #0 ; 1
= 1 ; 0
= #2 ; 2
- 0 2 3
jmpf 3 24 ;
= "if state" ; 4
push 4 ; ;
call print 1 5
jmpf 0 23 ;
= #1 ; 6
- 0 6 7
= 7 ; 0
push 0 ; ;
call print 1 8
= #1 ; 9
- 0 9 10
jmpf 10 22 ;
= "goz" ; 11
push 11 ; ;
call print 1 12
jmp 23 ; ;
jmp 22 ; ;
jmp 8 ; ;
jmp 66 ; ;
= #1 ; 5
- 0 5 4
jmpf 4 46 ;
= "else if state" ; 10
push 10 ; ;
call print 1 9
jmpf 0 45 ;
= #1 ; 8
- 0 8 7
= 7 ; 0
push 0 ; ;
call print 1 6
= #1 ; 12
- 0 12 11
jmpf 11 44 ;
= "goz" ; 13
push 13 ; ;
call print 1 14
jmp 45 ; ;
jmp 44 ; ;
jmp 30 ; ;
jmp 66 ; ;
= "else state" ; 9
push 9 ; ;
call print 1 10
= #1 ; 11
+ 0 11 12
jmpf 12 66 ;
= #1 ; 6
- 0 6 7
= 7 ; 0
push 0 ; ;
call print 1 8
= #2 ; 14
+ 0 14 13
jmpf 13 65 ;
= "goz" ; 15
push 15 ; ;
call print 1 16
jmp 49 ; ;
jmp 65 ; ;
jmp 49 ; ;
push 0 ; ;
ret 1 ; ;
)";

	JuaInterpter instance(code);
	zed test2("print", &instance);
	
	std::cout << instance.run_instructions()[0].get_doub();

	return 0;
}
