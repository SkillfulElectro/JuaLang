<p align="center">
  <img alt="JuaLang Logo" src="JuaLang_logo.png" style="width:20vw;height:auto;">
</p>


<h1 align="center">JuaLang</h1> 

an experimental programming language which is written based on custom PDA style algorithm instead of algorithms like LALR(1) and etc

## Why Jua?
- the first reason is just for having fun ! :)
- well when i use compiled languages , if i want to extend my final product and let users extend it more -> i need to embed python , js or wasm but well they have so much features which is not needed and we dont want to allow the extension system have them because its so much and etc , that time Jua comes in . Jua is not general propose programming language -> its designed to be a basic language with basic functionality well its functionality is extendable as much as you need !!!

## Intro to Jua
- Jua supports if , while , ( * , - , + , / ) operators , JuaModule functions
```c
/*comment */
```
```c
while (EXPR) {
/*stmts*/
} 
```
```c
if (EXPR) {
/*stmts*/
}
```
```c
func_name(args ...);
```
```c
hi = "string data type"
numeric = 1.2 
```
```c
/*EXPR*/
hi = zed() + (4/2) * 3 ;
```
- unlike other programming languages Jua does not return immimmediately after calling return , it just keeps the values to return when interpreter finishes running Juax code
```jua
return x , 2 , ...;
```
- Jua only has this limited tools but how you can extend them?

## Jua Extensions 
- for extending Jua , you have two ways :

### Jua Functions
- Jua has enough functionality to be as a basic bridge between your binary program and user extension codes
- for creating functions in Jua first you need to include the Juax interpreter
```c
#include "JuaInterpter.h"
```
- after this you can now use JuaModule to create Jua functions which are callabe from Jua side , e.g.:
```cpp
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
```
- and then you use it in your Intreoter
```cpp
  JuaInterpter instance(code);
	zed test1("zed", &instance);
	zed test2("bye", &instance);
```
- the functions are now callable with that associated names
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

## TODO!
- [x] lexer init
- [x] function calls
- [x] function return type
- [x] mixing up functions and expr
- [x] scope creation
- [ ] adding error messages 
- [x] adding expr
- [x] adding parans to expr 
- [x] supporting other keywords (while , if)
- [x] bytecode runner
- [x] JuaLang Extensions def in bytecode runner
- [ ] code optimization

