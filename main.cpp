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

	std::string code = cinstance.compile(
		R"(
function doz() {
	if (1) {
		print("test functions");
	}
}

hi = 3;
if(hi - 2)
{ 
	doz();
	print("if state");
	while (hi) {
		hi = hi - 1;
		print(hi);
		if (hi - 1) {
			print("test");
			break;
		}
	}
} else if ( hi - 1 ) {
	print("else if state");
	
	doz();
	while (hi) {
		hi = hi - 1;
		print(hi);
		if (hi - 1) {
			print("test");
			break;
		}
	}
} else {
	print("else state");

	doz();
	while (hi + 1) {
		hi = hi - 1;
		print(hi);
		if (hi + 2) {
			print("test");
			continue;
		}
	}
}

doz();

function bruh() {
	doz();
}


if(hi - 2)
{ 
	doz();
	print("if state");
	while (hi) {
		hi = hi - 1;
		print(hi);
		if (hi - 1) {
			print("test");
			break;
		}
	}
} else if ( hi - 1 ) {
	print("else if state");
	
	doz();
	while (hi) {
		hi = hi - 1;
		print(hi);
		if (hi - 1) {
			print("test");
			break;
		}
	}
} else {
	print("else state");
	doz();
	while (hi + 1) {
		hi = hi - 1;
		print(hi);
		if (hi + 2) {
			print("test");
			continue;
		}
	}
}

bruh();

return hi;
)");

	instance.insert_bytecode(code);

	std::cout << instance.run_instructions()[0].get_doub();

	return 0;
}