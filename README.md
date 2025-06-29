<p align="center">
  <img alt="JuaLang Logo" src="JuaLogo.jpg" style="width:auto;height:auto;">
</p>


<h1 align="center">JuaLang</h1> 

> language features are under examinations

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
continue;
break;
/*stmts*/
} 
```
```c
if (EXPR) {
/*stmts*/
} else if (EXPR) {
/*stmts*/
} else {
/* stmts */
}
```
- Jua has native functions , but Jua native functions because of the special behavior of return token does not return value
- btw Jua functions are inlined and also before it be used , it must be declared
```javascript
/* inlined functions are called macro now with macro keyword for defining them . */
function func_name(args) {
/* stmts */
}
```
- all types of function calls are by reference
```c
func_name(args ...);
```
```c
hi = "string data type"
numeric = 1.2 
```
- Jua native inline functions does not return value so can't use them in expr , so zed here is Jua extension func
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
- as an example you can take a look at main.cpp file which shows how to use the below features
- Jua has enough functionality to be as a basic bridge between your binary program and user
- for creating functions in Jua first you need to include the Juax interpreter which lives in JuaLang header
```c
#include "JuaLang.h"
```
- after this you can now use JuaModule to create Jua functions which are callabe from Jua side , e.g.:
- from v2.0.0 jua extension funcs use JuaStackVal , refer to releases for more details
- from version v5.0.0 , JuaModule is acting like a placeholder of class type and must be inherited for example : 
```cpp
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
```
- and then you use it in your interpreter
```cpp
        JuaInterpter instance;
	zed test2;
	instance.add_extension("print", &zed::jua_extension_func, &test2);

```
- the functions are now callable with that associated names
- also if your functions gonna send an object of the class to the Jua system , its possible by casting them to void* and return with VOID type . this type is only used for these kind of usages
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
- also you can enable func availability checking by :
```c++
	JuaLang cinstance;
	JuaInterpter instance;
	zed test2;
	instance.add_extension("print", &zed::jua_extension_func, &test2);
	cinstance.set_interpter(&instance);
```
- when you use set_interpter function , it produces faster Juax code for available interpreter , btw if you want to produce general Juax code do not use this function

## sys
- for using sys to create new programming language :
### Lexer : DFMatcher.h
- for using lexer you just create TokenDFA and pass them to it , the one which has the highest priority and accepts the highest len will be reported -> for each token it sees you have to recall its functions
- you should take a look at its source code , i tried to explain what each func does in comments

### compiler : DFAction.h
- DFAction is modular dfa like system but it has some key differences which makes working with it really easy
- you should inherit this class and create the main road the program will go like :
```
// example from JuaLang source code
		dfa[START][IDENT] = IDENTER;
		dfa[IDENTER][SEMICOLON] = START;

		this->add_special_dfa(START, dfa);

		DFA while_handler;
		while_handler[WHILE_HANDLER][CLOSE_PARAN] = WHILE_SCOPE;
		this->add_special_dfa(WHILE_HANDLER, while_handler);

		DFA if_handler;
		if_handler[IF_HANDLER][CLOSE_PARAN] = IF_SCOPE;
		this->add_special_dfa(IF_HANDLER, if_handler);

		DFA func_handler;
		this->add_special_dfa(FUNC_HANDLER, func_handler);


		DFA expr;
		expr[EXPR][OPERATOR] = EXPR_OPS;
		expr[EXPR_OPS][IDENT] = EXPR;
		expr[EXPR_OPS][CLOSE_PARAN] = EXPR;
		expr[EXPR_OPS][CONST_DOUBLE] = EXPR;
		expr[EXPR_OPS][CONST_STRING] = EXPR;
		this->add_special_dfa(EXPR_OPS, expr);

		DFA ret;
		this->add_special_dfa(RETURN_HANDLER, ret);
		
		DFA expr_para;
		this->add_special_dfa(EXPR_PARA, expr_para);

		/// adding the main road
		new_dfa(&dfa);
```
- after defining the main part which the program must follow , you will create sub roads and connect them to it via action_function which you must override
- try to create as many as sub road possible , it makes your job so much easier
- you manage how DFAction is going to work with DFActionFlowCode and go_next_index variables which are going to be passed to your overrided action_function and many more ...
- DFAction experienced some changes over the time refer to its source code for more details

## Contributions
- after all hope Jua be useful embedable programming language and feel free to help me extend it more and in a better way
- GoodLuck ! :)

## TODO!
- [x] lexer init
- [x] Jua native macro creation
- [x] Jua native macro calls
- [ ] Jua native function call/creation
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

