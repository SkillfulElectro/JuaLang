#include "JuaLang.h"
#include <iostream>

int main() {

	JuaLang instance;
	std::cout << instance.compile(
		R"(
hi = 1;
while(hi)
{ 
	hi = hi + 1;
	print(hi);
	continue;
} 
return hi;
)");


	return 0;
}
