#include "JuaLang.h"

JuaOprand jua_std_math_importer(std::vector<JuaStackVal>& oprands) {
    JuaOprand ret{ VOID, new JuaStdMath };
    ret.destructor = [](JuaOprand* obj) {
        delete obj->get_void_ptr();
    };
    return ret;
}

JuaOprand JuaStdMath::run_func_by_symbol(const std::string& name,
                                std::vector<JuaStackVal>& params)
{
    auto get_double = [&](int idx)->double {
        if (idx < 0 || idx >= int(params.size()))
            throw std::invalid_argument("Wrong number of arguments");

        JuaOprand* ptr;
        JuaOprand  tmp;
        if (params[idx].type == REF) {
            ptr = params[idx].get_ptr();
        } else {
            tmp = params[idx].get_obj();
            ptr = &tmp;
        }
        switch (ptr->op_type)
        {
        case DOUBLE:
            return ptr->get_doub();
        }   

        return atof(ptr->get_str().c_str());       
    };

    double result = 0.0;

    // unary functions & operators
    auto it1 = oneArgMap.find(name);
    if (it1 != oneArgMap.end()) {
        if (params.size() != 2)
            throw std::invalid_argument(name + " expects 1 argument");
        result = it1->second(get_double(1));
    }
    else {
        // binary functions & operators
        auto it2 = twoArgMap.find(name);
        if (it2 != twoArgMap.end()) {
            if (params.size() != 3)
                throw std::invalid_argument(name + " expects 2 arguments");
            result = it2->second(get_double(1), get_double(2));

        }
        else {
            throw std::invalid_argument("Unknown math function/operator: " + name);
        }
    }

    return JuaOprand{ DOUBLE, result };
}
