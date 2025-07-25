#ifndef JUA_STD_STRING
#define JUA_STD_STRING

#include "JuaLang.h"

class JuaStdString : public JuaVoidType {
    void get_oprand(JuaStackVal& stack_val , JuaOprand*& main_place , JuaOprand& tmp) {
        switch (stack_val.type) {
        case REF :
            main_place = stack_val.get_ptr();
            return;
        case VALUE:
            tmp = stack_val.get_obj();
            main_place = &tmp;
            return;
        }
    }
public:
    JuaOprand len(std::vector<JuaStackVal>& params) {
        JuaOprand* str;

        JuaOprand tmp_str;

        if (params.size() < 2) {
            return {DOUBLE , 0.0};
        }

        switch (params[1].type)
        {
        case REF:
            str = params[1].get_ptr();
            break;
        case VALUE:
            tmp_str = params[1].get_obj();
            str = &tmp_str;
            break;
        }

        switch (str->op_type) {
        case STRING:
            return {DOUBLE , (double)str->get_str().length()};
        case DOUBLE: {
            double tmp = str->get_doub();
            return {DOUBLE , (double)std::to_string(tmp).length()};
        }   
        default: 
            return {DOUBLE , 0.0};
        }
    }

    JuaOprand set_to_index(std::vector<JuaStackVal>& params) {
        if (params[1].type != REF || params.size() < 4) {
            return {DOUBLE , 0.0};
        }

        JuaOprand* str = params[1].get_ptr();
        JuaOprand* index;
        JuaOprand* charr;

        JuaOprand tmp_index;
        JuaOprand tmp_charr;

        get_oprand(params[2] , index , tmp_index);
        get_oprand(params[3] , charr , tmp_charr);

        size_t actual_index;

        switch (index->op_type) {
        case DOUBLE:
            actual_index = (size_t)index->get_doub();
            break;

        case STRING:
            actual_index = (size_t)(atof(index->get_str().c_str()));
            break;

        default:
            return {DOUBLE , 0.0};
        }

        char actual_char;

        switch (charr->op_type) {
        case DOUBLE:
            actual_char = std::to_string(charr->get_doub())[0];
            break;

        case STRING:
            if (charr->get_str().length() < 1) {
                return {DOUBLE , 0.0};
            }
            actual_char = charr->get_str()[0];
            break;

        default:
            return {DOUBLE , 0.0};
        }

        switch (str->op_type)
        {
        case STRING:
            if (str->get_str().length() <= actual_index) {
                return {DOUBLE , 0.0};
            }
            
            str->get_str()[actual_index] = actual_char;

            
            return {DOUBLE , 1.0};
        
        case DOUBLE: {
            std::string tmp_str = std::to_string(str->get_doub());
            
            if (tmp_str.length() <= actual_index) {
                return {DOUBLE , 0.0};
            }

            tmp_str[actual_index] = actual_char;

            str->value = atof(tmp_str.c_str());
        }
        default:
            return {DOUBLE , 0.0};
        }
    }

    JuaOprand get_from_index(std::vector<JuaStackVal>& params) {
        // Need at least 3 params: [0]=func, [1]=value, [2]=index
        if (params.size() < 3) {
            return { STRING, std::string("") };
        }

        // Unpack source and index (no REF required)
        JuaOprand* src;
        JuaOprand* index;
        JuaOprand  tmp_src;
        JuaOprand  tmp_index;

        get_oprand(params[1], src,   tmp_src);
        get_oprand(params[2], index, tmp_index);

        // Resolve index into a size_t
        size_t actual_index;
        switch (index->op_type) {
            case DOUBLE:
                actual_index = (size_t)index->get_doub();
                break;
            case STRING:
                actual_index = (size_t)atof(index->get_str().c_str());
                break;
            default:
                // unsupported index type → return empty string
                return { STRING, std::string("") };
        }


        std::string source;
        switch (src->op_type) {
        case STRING: {
            if (src->get_str().length() <= actual_index) {
                return { STRING, std::string("") };
            };

            char c = src->get_str()[actual_index];
            return { STRING, std::string(1, c) };
        }
            break;
        case DOUBLE:
            source = std::to_string(src->get_doub());
            break;
        default:
            return { STRING, std::string("") };
        }


        if (actual_index >= source.size()) {
            return { STRING, std::string("") };
        }


        char c = source[actual_index];
        return { STRING, std::string(1, c) };
    }


    JuaOprand run_func_by_symbol(const std::string& symbol 
        ,std::vector<JuaStackVal>& params) override {
        
        if (symbol == "len") {
            return len(params);
        } else if (symbol == "set_to_index") {
            return set_to_index(params);
        } else if (symbol == "get_from_index") {
            return get_from_index(params);
        } else {
            return {DOUBLE , -1.1};
        }
    }
};

JuaOprand create_string_util(std::vector<JuaStackVal>& oprands) {
    JuaOprand ret{ VOID, new JuaStdString };
    ret.destructor = [](JuaOprand* obj) {
        delete obj->get_void_ptr();
    };
    return ret;
}

#endif