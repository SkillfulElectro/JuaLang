#include "JuaLang.h"
#include <iostream>

int main() {

	JuaLang instance;
	instance.compile("hi = (2 + 1) / ( 3 - 1) * 3 ; bye(hi,2,4);");
}
