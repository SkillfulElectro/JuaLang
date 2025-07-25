#ifndef JUA_STD_VECTOR
#define JUA_STD_VECTOR

#include <vector>
#include <string>
#include <cstdlib>
#include "JuaLang.h"

class JuaStdVector : public JuaVoidType {
    std::vector<JuaOprand> vec;

public:
    // constructor: populate initial elements
    JuaStdVector(std::vector<JuaStackVal>& params) {
        push_back(params);
    }

    // pop_back: pop N elements (default 1)
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

    // push_back: append all params, return count
    JuaOprand push_back(std::vector<JuaStackVal>& params) {
        double copied_count = 0.0;
        for (auto& sv : params) {
            if (sv.type == REF) {
                JuaOprand* ptr = sv.get_ptr();
                if (ptr->op_type == DOUBLE || ptr->op_type == STRING) {
                    vec.push_back(*ptr);
                } else if (ptr->op_type == VOID) {
                    JuaVoidType* v = ptr->get_void_ptr();
                    if (v->is_copyable())
                        vec.push_back(std::move(v->copy()));
                }
            } else {
                JuaOprand copy = sv.get_obj();
                if (copy.op_type == DOUBLE || copy.op_type == STRING) {
                    vec.push_back(copy);
                } else if (copy.op_type == VOID) {
                    JuaVoidType* v = copy.get_void_ptr();
                    if (v->is_copyable())
                        vec.push_back(std::move(v->copy()));
                }
            }
            copied_count += 1.0;
        }
        return JuaOprand{DOUBLE, copied_count};
    }

    // dispatch methods by name
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
            // todo
        }
        if (name == "set") {
            // todo
        }
        if (name == "capacity") {
            return JuaOprand{DOUBLE, static_cast<double>(vec.capacity())};
        }
        if (name == "resize") {
            // todo !
        }
        if (name == "insert") {
            // todo
        }
        if (name == "erase") {
            // todo
        }

        if (name == "push_back") return push_back(params);
        if (name == "pop_back") return pop_back(params);


        return JuaOprand{DOUBLE, 0.0};
    }
};

// factory function
inline JuaOprand jua_create_vector(std::vector<JuaStackVal>& params) {
    JuaOprand ret{VOID, new JuaStdVector(params)};
    ret.destructor = [](JuaOprand* obj) { delete obj->get_void_ptr(); };
    return ret;
}

#endif // JUA_STD_VECTOR
