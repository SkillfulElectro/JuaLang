#ifndef JUA_STD_VECTOR
#define JUA_STD_VECTOR

#include <vector>
#include <string>
#include <cstdlib>
#include "JuaLang.h"

class JuaStdVector : public JuaVoidType {
    std::vector<JuaOprand> vec;

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
    JuaStdVector(std::vector<JuaStackVal>& params) {
        push_back(params);
    }


    JuaOprand pop_back(std::vector<JuaStackVal>& params) {
        size_t num_to_pop = 1;

        if (params.size() > 1) {
            JuaOprand* ptr;
            JuaOprand tmp;
            switch (params[1].type) {
            case REF:
                {
                    ptr = params[1].get_ptr();
                }
            break;
            case VALUE:
                {
                    tmp = params[1].get_obj();
                    ptr = &tmp;
                }
            break;
            }

            switch (ptr->op_type)
            {
            case STRING:
                num_to_pop = (size_t)atof(ptr->get_str().c_str());
                break;
            
            case DOUBLE:
                num_to_pop = size_t(ptr->get_doub());
                break;
            }
        }


        for (size_t i{0} ; i < num_to_pop ; ++i) {
            vec.pop_back();
        }

        return JuaOprand{DOUBLE , 1.0};
    }


    JuaOprand push_back(std::vector<JuaStackVal>& params) {
        double copied_count = 0.0;
        for (size_t i{1}; i < params.size() ; ++i ) {
            switch (params[i].type)
            {
            case REF: {
                JuaOprand* ptr = params[i].get_ptr();
                
                switch (ptr->op_type)
                {
                case DOUBLE:
                case STRING:
                    vec.push_back(*ptr);
                    break;
                
                case VOID:
                {
                    JuaVoidType* void_ptr = ptr->get_void_ptr();

                    if (void_ptr->is_copyable()) {
                        vec.push_back(std::move(void_ptr->copy()));
                    }
                }                    
                }
            }
                
                break;
            
            case VALUE:{
                JuaOprand ptr = params[i].get_obj();
                
                switch (ptr.op_type)
                {
                case DOUBLE:
                case STRING:
                    vec.push_back(ptr);
                    break;
                
                case VOID:
                {
                    JuaVoidType* void_ptr = ptr.get_void_ptr();

                    if (void_ptr->is_copyable()) {
                        vec.push_back(std::move(void_ptr->copy()));
                    }
                }                    
                }
            }

                break;
            }
        
            copied_count += 1.0;
        }

        return {DOUBLE , copied_count};
    }

    JuaOprand get(std::vector<JuaStackVal>& params) {
        if (params.size() < 2) {
            return JuaOprand{DOUBLE , 0.0};
        }

        JuaOprand* ptr;
        JuaOprand tmp;

        switch (params[1].type) {
        case REF:
            ptr = params[1].get_ptr();
            break;
        case VALUE:
            tmp = params[1].get_obj();
            ptr = &tmp;
            break;
        }

        size_t index;
        switch (ptr->op_type)
        {
        case DOUBLE:
            {
                index = (size_t)ptr->get_doub();
            }
            break;
        case STRING: 
            {
                index = (size_t)atof(ptr->get_str().c_str());
            }
            break;
        default:
            return {DOUBLE , 0.0};
        }

        return vec[index];
    }

    JuaOprand set(std::vector<JuaStackVal>& params) {
        if (params.size() < 3) {
            return JuaOprand{DOUBLE , 0.0};
        }

        JuaOprand* index;
        JuaOprand tmp_index;

        JuaOprand* value;
        JuaOprand tmp_value;

        get_oprand(params[1] , index , tmp_index);
        get_oprand(params[2] , value , tmp_value);

        size_t index_value;
        switch (index->op_type)
        {
        case DOUBLE:
            index_value = (size_t)index->get_doub();
            break;
        case STRING:
            index_value = (size_t)atof(index->get_str().c_str());
            break;
        default:
            return {DOUBLE , 0.0};
        }

        vec[index_value] = *value;

        return {DOUBLE , 1.0};
    }

    JuaOprand resize(std::vector<JuaStackVal>& params) {
        if (params.size() < 2) {
            return {DOUBLE , 0.0};
        }

        JuaOprand* len;
        JuaOprand tmp;

        get_oprand(params[1] , len , tmp);

        size_t new_len;
        switch (len->op_type)
        {
        case DOUBLE:
            new_len = (size_t)len->get_doub();
            break;
        case STRING:
            new_len = (size_t)atof(len->get_str().c_str());
            break;
        default:
            return {DOUBLE , 0.0};
        }

        vec.resize(new_len);

        return {DOUBLE , 1.0};
    }

    JuaOprand insert(std::vector<JuaStackVal>& params) {
        if (params.size() < 3) {
            return {DOUBLE , 0.0};
        }

        JuaOprand* index;
        JuaOprand tmp_index;

        get_oprand(params[1] , index , tmp_index);

        size_t index_value;
        switch (index->op_type)
        {
        case DOUBLE:
            index_value = (size_t)index->get_doub();
            break;
        case STRING:
            index_value = (size_t)atof(index->get_str().c_str());
            break;
        default:
            return {DOUBLE , 0.0};
        }

        JuaOprand* value;
        JuaOprand tmp_value;

        get_oprand(params[2] , value , tmp_value);

        vec.insert(vec.begin() + index_value , *value);

        return {DOUBLE , 1.0};
    }

    JuaOprand erase(std::vector<JuaStackVal>& params) {
        if (params.size() < 2) {
            return {DOUBLE , 0.0};
        }

        JuaOprand* index;
        JuaOprand tmp_index;

        get_oprand(params[1] , index , tmp_index);

        size_t index_value;
        switch (index->op_type)
        {
        case DOUBLE:
            index_value = (size_t)index->get_doub();
            break;
        case STRING:
            index_value = (size_t)atof(index->get_str().c_str());
            break;
        default:
            return {DOUBLE , 0.0};
        }

        vec.erase(vec.begin() + index_value);

        return {DOUBLE , 1.0};
    }    

    JuaOprand run_func_by_symbol(const std::string& name, std::vector<JuaStackVal>& params) override {
        if (name == "size") {
            return JuaOprand{DOUBLE, static_cast<double>(vec.size())};
        }
        if (name == "empty") {
            return JuaOprand{DOUBLE, vec.empty() ? 1.0 : 0.0};
        }
        if (name == "clear") {
            vec.clear();
            return JuaOprand{DOUBLE, 0.0};
        }
        if (name == "front") {
            if (!vec.empty()) return vec.front();
            return JuaOprand{DOUBLE, 0.0};
        }
        if (name == "back") {
            if (!vec.empty()) return vec.back();
            return JuaOprand{DOUBLE, 0.0};
        }
        if (name == "get") {
            return get(params);
        }
        if (name == "set") {
            return set(params);
        }
        if (name == "capacity") {
            return JuaOprand{DOUBLE, static_cast<double>(vec.capacity())};
        }
        if (name == "resize") {
            return resize(params);
        }
        if (name == "insert") {
            return insert(params);
        }
        if (name == "erase") {
            return erase(params);
        }

        if (name == "push_back") return push_back(params);
        if (name == "pop_back") return pop_back(params);


        return JuaOprand{DOUBLE, 0.0};
    }
};


inline JuaOprand jua_create_vector(std::vector<JuaStackVal>& params) {
    JuaOprand ret{VOID, new JuaStdVector(params)};
    ret.destructor = [](JuaOprand* obj) { delete obj->get_void_ptr(); };
    return ret;
}

#endif // JUA_STD_VECTOR
