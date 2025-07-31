<p align="center">
  <img alt="JuaLang Logo" src="JuaLogo.jpg" style="width:auto;height:auto;">
</p>


<h1 align="center">JuaLang</h1> 

-  a programming language which does not try to rebuild the wheel , it uses the wheel . based on custom algorithms which tries to be simple , fast , extensible and efficient with Goal of creating a language which can be anything from config language to general purpose programming language .

## Why Jua?
- fast runtime
- simple syntax
- extensible
- dependency free
- memory safety at its core without garbage collection !

## How to use
- build :
```sh
git clone https://github.com/SkillfulElectro/JuaLang.git
mkdir build
cmake -S . -B build
cmake --build build --config Release
```
- just include the JuaLang.h file from include directory and link your program to shared lib in the build directory .

## Intro to Jua
- JuaLang has very similar syntax to other programming languages like JavaScript , the functionality of the below JuaLang codes are same as JavaScript

- preprocessors : preprocessors are extensions to language syntax ! anything between # and ; is a code block which will be preprocessed .
```c
#import("main.jua");
```

- comments :
```c
/*comment */
```

- loops (only while loop is supported) :
```c
while (condition) {
continue;
break;
/*code*/
} 
```

- if else chain :
```c
if (condition) {
/*code*/
} else if (condition) {
/*code*/
} else {
/* code */
}
```

- macro definition : 
```JuaLang
macro func_name(args) {
/* code */
}
```
in future updates , function keyword will be used for defining functions instead of macros .
```javascript
function func_name(args) {
/* code */
}
```

- function / macro calling :
```cpp
func_name(args ...);
```
- member function calling (only objects of JuaVoidType) :
```cpp
/*if x is JuaVoidType the below code is valid*/
x.func_name(args ...);
```
- variable definition : JuaLang only supports two data types numeric and strings ! 
```c
hi = "string data type"
numeric = 1.2 
```
- expressions : 
```cpp
/*you can use macro functions  like below because they do not return values !*/
value = 2;
squared = 0;
square(value , squared);

hi = zed() + (4/2) * 3 / (x.beta(3) - 6) + squared;
```
- return : unlike other languages , in JuaLang return is global keyword which is used for adding values to final result of running code .
```cpp
return x , 2 , ...;
```

## JuaLang : extensible language
- for extending Jua , you have two ways :

### JuaFunc
- the easiest way to extend Jua is by using JuaFunc .
- you only have to define a function with same signature as the below example and then in your code you can use it like other built-in functions . (unlike macros , they return values which)
```cpp
#include "JuaLang.h"

JuaOprand jua_extension_func(std::vector<JuaStackVal>& oprands) {


	for (size_t i = 0 ; i < oprands.size() ; ++i) {
		auto& oprand = *oprands[i].get_ptr();

		switch (oprand.op_type)
		{
		case STRING:
			std::cout << oprand.get_str() << "\n";
			break;
		case DOUBLE:
			std::cout << oprand.get_doub() << "\n";
			break;
		}
	}

	
	JuaOprand ret {VOID , nullptr};
	ret.destructor = [](JuaOprand*) {
		std::cout << "cleaning ! \n";
	};
	




	return ret;
}
```
- below code adds print function to JuaLang interpreter : (it is suggested to use set_interpter on your JuaLang compiler instance before running compilation task !)
```cpp

	JuaLang cinstance;
	JuaInterpter instance;
	instance.add_extension("print", jua_extension_func);
	cinstance.set_interpter(&instance);
```
- also set_interpter is used to check function availability in the interpreter side .

### JuaVoidType
- if you want to add objects to JuaLang , you can create classes which inherit from JuaVoidType and override virtual functions  .
- JuaVoidType class :
```cpp
class JuaVoidType
{
public:
	// params[0] is JuaVoidType* which ref to class itself
	virtual JuaOprand run_func_by_symbol(const std::string& , std::vector<JuaStackVal>&) = 0;
	virtual bool is_copyable() {
		return false;
	}

	virtual JuaOprand copy() {
		return {DOUBLE , 0.0};
	}

	virtual bool is_equatable() {
		return false;
	}

	virtual bool equals(JuaVoidType*) {
		return false;
	}

	virtual bool is_hashable() {
		return false;
	}

	virtual size_t hash() {
		return 0;
	}
};
```
- after creating your class , now you can expose your class objects using JuaFunc .
- for example below function is used to create instance of JuaStdMath class in JuaLang :
```cpp
// importer: pushes a void‑typed math object onto the stack
inline JuaOprand jua_std_math_importer(std::vector<JuaStackVal>& oprands) {
    JuaOprand ret{ VOID, new JuaStdMath };
    ret.destructor = [](JuaOprand* obj) {
        delete obj->get_void_ptr();
    };
    return ret;
}
```

### Jua Preprocessors :
- are same as JuaFunc but they run on compile time !
- for example import is a preprocessor which is defined by Jua standard library :
```cpp
#import("../main.jua");
```
- you create them by defining a function in signature same as below function : 
```cpp
std::string import(const std::vector<DFActionToken>& tokens) {
	// ...
}
```
- by the way , it must return valid JuaLang code !
- so overall it gets the tokens directly and creates special behavior for them in compile time !
- you add your preprocessor to your compiler instance like below example :
```cpp
	JuaLang cinstance;
	cinstance.preprocessors["import"] = import;
	cinstance.preprocessors["include"] = import;
```
- now its possible to use them in JuaLang code .


### Jua compiler
- Jua compiler for
```jua
hi = 10;
while (hi - 1) { bye(hi); hi = hi - 1; }
return hi;
```
- generates juax code like below
```juax
= #10 ; 1
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
```
- but you can use other functions of the JuaLang instance to introduce new functionality
- sample compilation cpp code :
```cpp
	JuaLang instance;
	std::cout << instance.compile("hi = 10; while (hi - 1) { bye(hi); hi = hi - 1; } return hi;");
```
- when you use set_interpter function , it produces faster Juax code for available interpreter , btw if you want to produce general Juax code do not use this function

## Contributions
- all contributions are welcome .

## TODO!
- [x] lexer init
- [x] Jua native function creation
- [x] Jua native function calls
- [x] function calls
- [x] function return type
- [x] mixing up functions and expr
- [x] scope creation
- [x] local scope creation
- [ ] adding error messages 
- [x] adding expr
- [x] adding parans to expr 
- [x] supporting other keywords (while , if)
- [x] bytecode runner
- [x] JuaLang Extensions def in bytecode runner
- [ ] code optimization
- [x] standard library
- [x] preprocessors
- [ ] error JuaVoidType impl
