#include "JuaLang.h"
#include <iostream>

int main() {

	JuaLang instance;
	instance.compile("hi = zed() + (4/2) * 3 ; bye(hi,2,4);");
}
