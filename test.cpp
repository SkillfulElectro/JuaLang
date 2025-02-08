#include "JuaLang.h"
#include <iostream>

int main() {

	JuaLang instance;
	std::cout << instance.compile("hi = 10; while (hi - 1) { bye(hi); hi = hi - 1; } return hi;");
}
