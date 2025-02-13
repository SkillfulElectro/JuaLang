#include "JuaLang.h"
#include <iostream>

int main() {

	JuaLang instance;
	std::cout << instance.compile(
		R"(
hi = 3;
if(hi - 2)
{ 
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

	while (hi + 1) {
		hi = hi - 1;
		print(hi);
		if (hi + 2) {
			print("test");
			continue;
		}
	}
}

return hi;
)");


	return 0;
}
