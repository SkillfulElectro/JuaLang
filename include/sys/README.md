# sys : [DOC NOT UPDATED]
- for using sys to create new programming language :
### Lexer : DFMatcher.h
- for using lexer you just create TokenDFA and pass them to it , the one which has the highest priority and accepts the highest len will be reported -> for each token it sees you have to recall its functions
- you should take a look at its source code , i tried to explain what each func does in comments

### compiler : DFAction.h
- DFAction is modular dfa like system but it has some key differences which makes working with it really easy
- you should inherit this class and create the main road the program will go like :
```cpp
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