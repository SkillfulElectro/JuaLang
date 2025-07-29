#include "JuaLang.h"

void JuaStdHashMap::get_oprand(JuaStackVal &stack_val, JuaOprand *&out_ptr, JuaOprand &tmp)
{
    if (stack_val.type == REF)
    {
        out_ptr = stack_val.get_ptr();
    }
    else
    {
        tmp = stack_val.get_obj();
        out_ptr = &tmp;
    }
}

JuaStdHashMap::JuaStdHashMap(std::vector<JuaStackVal> &params)
{
    for (size_t i = 0; i + 1 < params.size(); i += 2)
    {
        JuaOprand *key_ptr, *val_ptr;
        JuaOprand tmp_key, tmp_val;
        get_oprand(params[i], key_ptr, tmp_key);
        get_oprand(params[i + 1], val_ptr, tmp_val);

        switch (key_ptr->op_type)
        {
        case DOUBLE:
        case STRING:
        {
            switch (val_ptr->op_type)
            {
            case STRING:
            case DOUBLE:
            {
                map[*key_ptr] = *val_ptr;
            }
            break;

            case VOID:
            {
                JuaVoidType *void_ptr = val_ptr->get_void_ptr();

                if (void_ptr->is_copyable())
                {
                    map[*key_ptr] = void_ptr->copy();
                }
            }
            break;
            }
        }
        break;
        }
    }
}


JuaOprand JuaStdHashMap::get(std::vector<JuaStackVal> &params)
{
    if (params.size() < 2)
    {
        return {DOUBLE, 0.0};
    }

    JuaOprand *key;
    JuaOprand tmp_key;

    get_oprand(params[1], key, tmp_key);

    switch (key->op_type)
    {
    case STRING:
    case DOUBLE:
        return map[*key];
        break;

    default:
        return {DOUBLE , 0.0};
    }
}

JuaOprand JuaStdHashMap::set(std::vector<JuaStackVal>& params) {
    if (params.size() < 3) {
        return {DOUBLE , 0.0};
    }

    JuaOprand* key;
    JuaOprand tmp_key;

    JuaOprand* value;
    JuaOprand tmp_value;

    get_oprand(params[1] , key , tmp_key);
    get_oprand(params[2] , value , tmp_value);

    switch (key->op_type)
    {
    case DOUBLE:
    case STRING: {
        switch (value->op_type)
        {
        case DOUBLE:
        case STRING:
            map[*key] = *value;
            return {DOUBLE , 1.0};
        break;
        
        case VOID:
            {
                JuaVoidType* ptr = value->get_void_ptr();

                if (ptr->is_copyable()) {
                    map[*key] = std::move(ptr->copy());
                } else {
                    return {DOUBLE , 0.0};
                }

                return {DOUBLE , 1.0};
            }
        break;
        }
    }
    break;
    
    }
    return {DOUBLE , 0.0};
}

JuaOprand JuaStdHashMap::contains(std::vector<JuaStackVal>& params) {
    if (params.size() < 2) {
        return {DOUBLE , 0.0};
    }

    JuaOprand* key;
    JuaOprand tmp_key;

    get_oprand(params[1] , key , tmp_key);

    switch (key->op_type)
    {
    case DOUBLE:
    case STRING:
        if (map.find(*key) != map.end()) {
            return {DOUBLE , 1.0};
        }
        break;
    }

    return {DOUBLE , 0.0};
}


JuaOprand JuaStdHashMap::erase(std::vector<JuaStackVal>& params) {
    if (params.size() < 2) {
        return {DOUBLE , 1.0};
    }

    JuaOprand* key;
    JuaOprand tmp_key;

    get_oprand(params[1] , key , tmp_key);

    switch (key->op_type)
    {
    case DOUBLE:
    case STRING:
        if (map.find(*key) != map.end()) {
            map.erase(*key);

            return {DOUBLE , 1.0};
        }
        break;
    }

    return {DOUBLE , 1.0}; 
}

JuaOprand JuaStdHashMap::run_func_by_symbol(const std::string &name, std::vector<JuaStackVal> &params)
{
    if (name == "size")
    {
        return JuaOprand{DOUBLE, static_cast<double>(map.size())};
    }
    if (name == "empty")
    {
        return JuaOprand{DOUBLE, map.empty() ? 1.0 : 0.0};
    }
    if (name == "clear")
    {
        map.clear();
        return JuaOprand{DOUBLE, 1.0};
    }
    if (name == "get")
    {
        return get(params);
    }
    if (name == "set")
    {
        return set(params);
    }
    if (name == "erase")
    {
        return erase(params);
    }
    if (name == "contains")
    {
        return contains(params);
    }
    return JuaOprand{DOUBLE, 0.0};
}

JuaOprand jua_create_hash_map(std::vector<JuaStackVal> &params)
{
    JuaOprand ret{VOID, new JuaStdHashMap(params)};
    ret.destructor = [](JuaOprand *obj)
    { delete obj->get_void_ptr(); };
    return ret;
}