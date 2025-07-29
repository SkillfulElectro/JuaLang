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

    void get_oprand(JuaStackVal &stack_val, JuaOprand *&out_ptr, JuaOprand &tmp);

public:
    JuaStdHashMap(std::vector<JuaStackVal> &params);

    JuaOprand get(std::vector<JuaStackVal> &params);

    JuaOprand set(std::vector<JuaStackVal>& params);

    JuaOprand contains(std::vector<JuaStackVal>& params) ;

    JuaOprand erase(std::vector<JuaStackVal>& params);

    JuaOprand run_func_by_symbol(const std::string &name, std::vector<JuaStackVal> &params) override ;
};

JuaOprand jua_create_hash_map(std::vector<JuaStackVal> &params);

#endif // JUA_STD_HASHMAP
