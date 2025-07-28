#ifndef JUA_STD_HASHMAP
#define JUA_STD_HASHMAP

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdlib>
#include "JuaLang.h"

namespace std
{
    template <>
    struct hash<JuaOprand>
    {
        size_t operator()(const JuaOprand &s) const noexcept
        {
            switch (s.op_type)
            {
            case STRING:
            {

                const std::string &tmp =
                    const_cast<JuaOprand &>(s).get_str();
                return std::hash<std::string>{}(tmp);
            }

            case DOUBLE:
            {
                double tmp =
                    const_cast<JuaOprand &>(s).get_doub();
                return std::hash<double>{}(tmp);
            }

            case VOID:
            {
                JuaVoidType *ptr =
                    const_cast<JuaOprand &>(s).get_void_ptr();
                if (ptr->is_hashable())
                {
                    return ptr->hash();
                }
                break;
            }

            default:
                break;
            }

            return 0;
        }
    };
}

namespace std
{
    template <>
    struct equal_to<JuaOprand>
    {
        bool operator()(const JuaOprand &a, const JuaOprand &b) const noexcept
        {
            if (a.op_type != b.op_type)
                return false;
            switch (a.op_type)
            {
            case STRING:

                return const_cast<JuaOprand &>(a).get_str() ==
                       const_cast<JuaOprand &>(b).get_str();
            case DOUBLE:
                return const_cast<JuaOprand &>(a).get_doub() ==
                       const_cast<JuaOprand &>(b).get_doub();
            case VOID:
            {
                auto pa = const_cast<JuaOprand &>(a).get_void_ptr();
                auto pb = const_cast<JuaOprand &>(b).get_void_ptr();

                if (pa == pb)
                    return true;
                if (pa->is_equatable())
                    return pa->equals(pb);
                return false;
            }
            default:
                return false;
            }
        }
    };
}

class JuaStdHashMap : public JuaVoidType
{
    std::unordered_map<JuaOprand, JuaOprand> map;

    void get_oprand(JuaStackVal &stack_val, JuaOprand *&out_ptr, JuaOprand &tmp)
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

public:
    JuaStdHashMap(std::vector<JuaStackVal> &params)
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

    JuaOprand get(std::vector<JuaStackVal> &params)
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

    JuaOprand set(std::vector<JuaStackVal>& params) {
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

    JuaOprand contains(std::vector<JuaStackVal>& params) {
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

    JuaOprand erase(std::vector<JuaStackVal>& params) {
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

    JuaOprand run_func_by_symbol(const std::string &name, std::vector<JuaStackVal> &params) override
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
};

inline JuaOprand jua_create_hash_map(std::vector<JuaStackVal> &params)
{
    JuaOprand ret{VOID, new JuaStdHashMap(params)};
    ret.destructor = [](JuaOprand *obj)
    { delete obj->get_void_ptr(); };
    return ret;
}

#endif // JUA_STD_HASHMAP
