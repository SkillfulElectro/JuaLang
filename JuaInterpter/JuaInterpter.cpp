#include "JuaInterpter.h"

JuaInterpter::JuaInterpter(const std::string& juax_code) {
    TokenDFA ident_func;
    ident_func.start_state = "0";
    ident_func.token_identifier = FUNC_IDENT;
    ident_func.ignore = false;
    ident_func.add_ASCII_range("0", 'a', 'z', "1");
    ident_func.add_ASCII_range("0", 'A', 'Z', "1");
    ident_func.dfa["0"]['_'] = "1";
    ident_func.add_ASCII_range("1", 'a', 'z', "1");
    ident_func.add_ASCII_range("1", 'A', 'Z', "1");
    ident_func.add_ASCII_range("1", '0', '9', "1");
    ident_func.dfa["1"]['_'] = "1";
    ident_func.add_final_state("1");
    lexer.insert_token(ident_func);

    lexer.create_word_token("push", PUSH, false);
    lexer.create_word_token("pop", POP, false);
    lexer.create_word_token("call", CALL, false);
    lexer.create_word_token("=", ASSIGN, false);
    lexer.create_word_token("+", PLUS, false);
    lexer.create_word_token("-", NEGATIVE, false);
    lexer.create_word_token("/", DIVIDE, false);
    lexer.create_word_token("*", MULTI, false);

    TokenDFA addr;
    addr.start_state = "0";
    addr.token_identifier = ADDR;
    addr.ignore = false;
    addr.add_ASCII_range("0", '0', '9', "1");
    addr.add_ASCII_range("1", '0', '9', "1");;
    addr.dfa["1"]['.'] = "2";
    addr.add_ASCII_range("2", '0', '9', "3");
    addr.add_ASCII_range("3", '0', '9', "3");
    addr.add_final_state("1");
    addr.add_final_state("3");
    lexer.insert_token(addr);

    TokenDFA num;
    num.start_state = "num";
    num.token_identifier = DOUBLE;
    num.ignore = false;
    num.dfa["num"]['#'] = "0";
    num.add_ASCII_range("0", '0', '9', "1");
    num.add_ASCII_range("1", '0', '9', "1");;
    num.dfa["1"]['.'] = "2";
    num.add_ASCII_range("2", '0', '9', "3");
    num.add_ASCII_range("3", '0', '9', "3");
    num.add_final_state("1");
    num.add_final_state("3");
    lexer.insert_token(num);

    TokenDFA str;
    str.start_state = "0";
    str.ignore = false;
    str.token_identifier = STRING;
    str.dfa["0"]['\"'] = "1";
    str.add_ASCII_range("1", char(0), char(127), "1");
    str.dfa["1"]['\"'] = "2";
    str.dfa["1"]['\\'] = "3";
    str.add_ASCII_range("3", char(0), char(127), "1");
    str.add_final_state("2");
    lexer.insert_token(str);

    lexer.create_word_token(" ", IGNORES, true);
    lexer.create_word_token("\n", NEXT_INST, false);
    lexer.create_word_token(";", EMPTY, false);
    lexer.create_word_token("jmp", JUMP, false);
    lexer.create_word_token("jmpf", JUMPF, false);
    lexer.create_word_token("jmpt", JUMPT, false);
    lexer.create_word_token("ret", RETURN, false);


    if (juax_code != "") {
        insert_bytecode(juax_code);
    }
}

JuaOprand JuaInterpter::convert_DFMatcherRes(const DFMatcherRes& res) {
    JuaOprand oprand;

    switch (res.token_identifier)
    {
    case ADDR:
        oprand.op_type = DFActionType(res.token_identifier);
        oprand.value = std::stoul(std::string(res.value));

        break;
    case DOUBLE:
        oprand.op_type = DFActionType(res.token_identifier);
        oprand.value = atof(std::string(res.value).substr(1).c_str());
        break;
    case STRING:
        oprand.op_type = DFActionType(res.token_identifier);
        oprand.value = std::string(res.value).substr(1, res.value.length() - 2);
        break;
    case EMPTY:
    case FUNC_IDENT:
        oprand.op_type = DFActionType(res.token_identifier);
        oprand.value = std::string(res.value);
        break;
    }

    return oprand;
}

void JuaInterpter::insert_bytecode(const std::string& juax_code) {
    instructions.clear();

    size_t index = 0;
    DFMatcherRes res;

    JuaInstruction instruction;
    size_t inst_val = 0;



    do {
        res = lexer.get_token(juax_code, index);



        switch (inst_val)
        {
        case 0:
            instruction.job = DFActionType(res.token_identifier);
            break;
        case 1:
            instruction.oprand1 = convert_DFMatcherRes(res);
            break;
        case 2:
            instruction.oprand2 = convert_DFMatcherRes(res);
            break;
        case 3:
            instruction.result = convert_DFMatcherRes(res);
            break;
        }

        if (res.token_identifier != NEXT_INST) {
            ++inst_val;
        }
        else {
            instructions.push_back(instruction);
            inst_val = 0;
        }

    } while (res.status != END_OF_FILE);
}

void JuaInterpter::add_extension(const std::string& name, JuaFunc func) {

    ext_table[name] = extensions.size();

    extensions.push_back(func);
}

std::vector<JuaOprand> JuaInterpter::run_instructions() {
    std::vector<JuaOprand> rets;

    for (size_t inst_index{ 0 }; inst_index < instructions.size(); ++inst_index) {
        auto& instruction = instructions[inst_index];

        switch (instruction.job)
        {
        case PUSH:
            switch (instruction.oprand1.op_type)
            {
            case ADDR:
                stack.push_back({ REF , &v_mem[instruction.oprand1.get_sizet()] });

                break;
            case DOUBLE: {
                stack.push_back({ VALUE , JuaOprand{ instruction.oprand1.op_type , instruction.oprand1.get_doub() } });

                break;
            }
            case STRING:
                stack.push_back({ VALUE , JuaOprand{ instruction.oprand1.op_type , instruction.oprand1.get_str() } });

                break;
            }
            break;

        case POP: {
            // pop ; ; addr
            auto back = stack.back();
            stack.pop_back();

            switch (back.type)
            {
            case VALUE:
                v_mem[instruction.result.get_sizet()] = back.get_obj();
                break;
            case REF:
                v_mem[instruction.result.get_sizet()] = *back.get_ptr();
                break;
            }

            break;
        }
        case CALL: {
            std::vector<JuaStackVal> input(stack.end() - instruction.oprand2.get_sizet(), stack.end());
            stack.erase(stack.end() - instruction.oprand2.get_sizet(), stack.end());

            

            switch (instruction.oprand1.op_type)
            {
            case FUNC_IDENT: {

                if (input.size() >= 1) {
                    switch (input[0].type)
                    {
                    case REF:{
                        JuaOprand* ptr = input[0].get_ptr();
                        
                        if (ptr->op_type == VOID) {
                            JuaVoidType* func = ptr->get_void_ptr();
                            v_mem[instruction.result.get_sizet()] = func->run_func_by_symbol(
                                instruction.oprand1.get_str() , input);
                        } else {
                            auto& func = extensions[ext_table[instruction.oprand1.get_str()]];
                            v_mem[instruction.result.get_sizet()] = func(input);
                        }
                    }
                    break;
                    
                    case VALUE: {
                        JuaOprand val = input[0].get_obj();
                        
                        if (val.op_type == VOID) {
                            JuaVoidType* func = val.get_void_ptr();
                            v_mem[instruction.result.get_sizet()] = func->run_func_by_symbol(
                                instruction.oprand1.get_str() , input);
                        } else {
                            auto& func = extensions[ext_table[instruction.oprand1.get_str()]];
                            v_mem[instruction.result.get_sizet()] = func(input);
                        }
                    }
                    break;
                    }

                } else {
                    auto& func = extensions[ext_table[instruction.oprand1.get_str()]];
                    v_mem[instruction.result.get_sizet()] = func(input);
                }



                break;
            }
            case ADDR: {
                auto& func = extensions[instruction.oprand1.get_sizet()];
                v_mem[instruction.result.get_sizet()] = func(input);
               

                break;
            }
            }

            break;
        }
        case RETURN:
            // ret n ; ;
            for (size_t i{ 0 }; i < instruction.oprand1.get_sizet(); ++i) {
                auto back = stack.back();
                stack.pop_back();

                switch (back.type)
                {
                case VALUE:
                    rets.push_back(back.get_obj());
                    break;
                case REF:
                    rets.push_back(*back.get_ptr());
                    break;
                }
            }

            break;
        case JUMP:
            inst_index = instruction.oprand1.get_sizet();
            --inst_index;
            break;
        case JUMPF:
            switch (instruction.oprand1.op_type)
            {
            case ADDR:
                // jmpf addr i ;

                switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                {
                case DOUBLE:
                    if (v_mem[instruction.oprand1.get_sizet()].get_doub() <= 0) {
                        inst_index = instruction.oprand2.get_sizet();
                        --inst_index;

                        break;
                    }

                    break;
                case STRING:
                    if (v_mem[instruction.oprand1.get_sizet()].get_str().length() <= 0) {
                        inst_index = instruction.oprand2.get_sizet();
                        --inst_index;

                        break;
                    }
                    break;
                }

                break;
            case DOUBLE: {
                if (instruction.oprand1.get_doub() <= 0) {
                    inst_index = instruction.oprand2.get_sizet();
                    --inst_index;

                    break;
                }

                break;
            }
            case STRING:
                if (instruction.oprand1.get_str().length() <= 0) {
                    inst_index = instruction.oprand2.get_sizet();
                    --inst_index;

                    break;
                }

                break;
            }


            break;
        case JUMPT:
            switch (instruction.oprand1.op_type)
            {
            case ADDR:
                // jmpf addr i ;

                switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                {
                case DOUBLE:
                    if (v_mem[instruction.oprand1.get_sizet()].get_doub() > 0) {
                        inst_index = instruction.oprand2.get_sizet();
                        --inst_index;

                        break;
                    }

                    break;
                case STRING:
                    if (v_mem[instruction.oprand1.get_sizet()].get_str().length() > 0) {
                        inst_index = instruction.oprand2.get_sizet();
                        --inst_index;

                        break;
                    }
                    break;
                }

                break;
            case DOUBLE: {
                if (instruction.oprand1.get_doub() > 0) {
                    inst_index = instruction.oprand2.get_sizet();
                    --inst_index;

                    break;
                }

                break;
            }
            case STRING:
                if (instruction.oprand1.get_str().length() > 0) {
                    inst_index = instruction.oprand2.get_sizet();
                    --inst_index;

                    break;
                }

                break;
            }


            break;
        case ASSIGN: {
            JuaOprand res;
            switch (instruction.oprand1.op_type)
            {
            case STRING:
            case DOUBLE:
                res.op_type = instruction.oprand1.op_type;
                res.value = instruction.oprand1.value;
                v_mem[instruction.result.get_sizet()] = std::move(res);
                break;
            case ADDR: {
                auto& tmp = v_mem[instruction.oprand1.get_sizet()];
                v_mem[instruction.result.get_sizet()] = (tmp.op_type == VOID) ? std::move(tmp) : tmp;
                break;
            }
            default:
                break;
            }

            break;
        }
        case PLUS: {


            switch (instruction.oprand1.op_type)
            {
            case ADDR: {

                switch (instruction.oprand2.op_type)
                {
                case ADDR: {

                    switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                        switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                        {
                        case DOUBLE:
                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() +
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() +
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].op_type = STRING;
                        switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                        {
                        case DOUBLE:
                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_str() +
                                std::to_string(v_mem[instruction.oprand2.get_sizet()].get_doub());
                            break;
                        case STRING:
                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_str() +
                                v_mem[instruction.oprand2.get_sizet()].get_str();
                            break;
                        }
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() +
                            instruction.oprand2.get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].op_type = STRING;
                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_str() +
                            std::to_string(instruction.oprand2.get_doub());
                        break;
                    }
                    break;
                }
                case STRING: {
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() +
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].op_type = STRING;
                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_str() +
                            instruction.oprand2.get_str();
                        break;
                    }
                    break;
                }
                }
                break;
            }
            case DOUBLE: {

                switch (instruction.oprand2.op_type)
                {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                    {
                    case DOUBLE:
                        v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() +
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:
                        v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() +
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() +
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() +
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            case STRING: {

                switch (instruction.oprand2.op_type)
                {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                    {
                    case DOUBLE:
                        v_mem[instruction.result.get_sizet()].op_type = STRING;
                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_str() +
                            std::to_string(v_mem[instruction.oprand2.get_sizet()].get_doub());
                        break;
                    case STRING:
                        v_mem[instruction.result.get_sizet()].op_type = STRING;
                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_str() +
                            v_mem[instruction.oprand2.get_sizet()].get_str();
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].op_type = STRING;
                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_str() +
                        std::to_string(instruction.oprand2.get_doub());
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].op_type = STRING;
                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_str() +
                        instruction.oprand2.get_str();
                    break;
                }
                }
                break;
            }
            }

            break;
        }

        case NEGATIVE: {

            switch (instruction.oprand1.op_type)
            {
            case ADDR: {

                switch (instruction.oprand2.op_type)
                {
                case ADDR: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                        {
                        case DOUBLE:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() -
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() -
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    case STRING:

                        switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                        {
                        case DOUBLE:

                            v_mem[instruction.result.get_sizet()].value =
                                atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) -
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:
                            v_mem[instruction.result.get_sizet()].value =
                                atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) -
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() -
                            instruction.oprand2.get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) -
                            instruction.oprand2.get_doub();
                        break;
                    }
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() -
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) -
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    }
                    break;
                }
                }
                break;
            }
            case DOUBLE: {

                v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                switch (instruction.oprand2.op_type)
                {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() -
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() -
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() -
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() -
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            case STRING: {

                v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                switch (instruction.oprand2.op_type)
                {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type)
                    {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(instruction.oprand1.get_str().c_str()) -
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(instruction.oprand1.get_str().c_str()) -
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].value =
                        atof(instruction.oprand1.get_str().c_str()) -
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].value =
                        atof(instruction.oprand1.get_str().c_str()) -
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            }
            break;
        }


        case MULTI: {
            switch (instruction.oprand1.op_type) {
            case ADDR: {

                switch (instruction.oprand2.op_type) {
                case ADDR: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type) {
                    case DOUBLE: {

                        switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                        case DOUBLE:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() *
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() *
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    }
                    case STRING: {

                        switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                        case DOUBLE:

                            v_mem[instruction.result.get_sizet()].value =
                                atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) *
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:

                            v_mem[instruction.result.get_sizet()].value =
                                atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) *
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    }
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() *
                            instruction.oprand2.get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) *
                            instruction.oprand2.get_doub();
                        break;
                    }
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() *
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) *
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    }
                    break;
                }
                }
                break;
            }
            case DOUBLE: {

                v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                switch (instruction.oprand2.op_type) {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() *
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() *
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() *
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() *
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            case STRING: {

                v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                switch (instruction.oprand2.op_type) {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(instruction.oprand1.get_str().c_str()) *
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(instruction.oprand1.get_str().c_str()) *
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].value =
                        atof(instruction.oprand1.get_str().c_str()) *
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].value =
                        atof(instruction.oprand1.get_str().c_str()) *
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            }
            break;
        }

        case DIVIDE: {
            switch (instruction.oprand1.op_type) {
            case ADDR: {

                switch (instruction.oprand2.op_type) {
                case ADDR: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type) {
                    case DOUBLE: {

                        switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                        case DOUBLE:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() /
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:

                            v_mem[instruction.result.get_sizet()].value =
                                v_mem[instruction.oprand1.get_sizet()].get_doub() /
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    }
                    case STRING: {

                        switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                        case DOUBLE:

                            v_mem[instruction.result.get_sizet()].value =
                                atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) /
                                v_mem[instruction.oprand2.get_sizet()].get_doub();
                            break;
                        case STRING:

                            v_mem[instruction.result.get_sizet()].value =
                                atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) /
                                atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                            break;
                        }
                        break;
                    }
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() /
                            instruction.oprand2.get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) /
                            instruction.oprand2.get_doub();
                        break;
                    }
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                    switch (v_mem[instruction.oprand1.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            v_mem[instruction.oprand1.get_sizet()].get_doub() /
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(v_mem[instruction.oprand1.get_sizet()].get_str().c_str()) /
                            atof(instruction.oprand2.get_str().c_str());
                        break;
                    }
                    break;
                }
                }
                break;
            }
            case DOUBLE: {

                v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                switch (instruction.oprand2.op_type) {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() /
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            instruction.oprand1.get_doub() /
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() /
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].value =
                        instruction.oprand1.get_doub() /
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            case STRING: {

                v_mem[instruction.result.get_sizet()].op_type = DOUBLE;
                switch (instruction.oprand2.op_type) {
                case ADDR: {

                    switch (v_mem[instruction.oprand2.get_sizet()].op_type) {
                    case DOUBLE:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(instruction.oprand1.get_str().c_str()) /
                            v_mem[instruction.oprand2.get_sizet()].get_doub();
                        break;
                    case STRING:

                        v_mem[instruction.result.get_sizet()].value =
                            atof(instruction.oprand1.get_str().c_str()) /
                            atof(v_mem[instruction.oprand2.get_sizet()].get_str().c_str());
                        break;
                    }
                    break;
                }
                case DOUBLE: {

                    v_mem[instruction.result.get_sizet()].value =
                        atof(instruction.oprand1.get_str().c_str()) /
                        instruction.oprand2.get_doub();
                    break;
                }
                case STRING: {

                    v_mem[instruction.result.get_sizet()].value =
                        atof(instruction.oprand1.get_str().c_str()) /
                        atof(instruction.oprand2.get_str().c_str());
                    break;
                }
                }
                break;
            }
            }
            break;
        }
        default:
            break;
        }
    }

    return rets;
}
