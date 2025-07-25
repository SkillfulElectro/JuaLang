#ifndef JUA_STD_HASHMAP
#define JUA_STD_HASHMAP

#include "JuaLang.h"


/////// TODO !

namespace std
{
    struct hash<JuaOprand>
    {
        size_t operator()(JuaOprand& s) const noexcept
        {
            ;
        }
    };
}

class JuaStdHashMap : public JuaVoidType {
public:
    JuaOprand run_func_by_symbol(const std::string& name , std::vector<JuaStackVal>& params) override {

    }
};

JuaOprand jua_create_hash_map(std::vector<JuaStackVal>& params) {
    
}

#endif