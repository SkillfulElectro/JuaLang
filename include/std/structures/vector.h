#ifndef JUA_STD_VECTOR
#define JUA_STD_VECTOR

#include <vector>
#include <string>
#include <cstdlib>
#include "JuaLang.h"

class JuaStdVector : public JuaVoidType {
    std::vector<JuaOprand> vec;

    void get_oprand(JuaStackVal& stack_val , JuaOprand*& main_place , JuaOprand& tmp);
public:
    JuaStdVector(std::vector<JuaStackVal>& params);


    JuaOprand pop_back(std::vector<JuaStackVal>& params);


    JuaOprand push_back(std::vector<JuaStackVal>& params , size_t start_index = 1) ;

    JuaOprand get(std::vector<JuaStackVal>& params) ;

    JuaOprand set(std::vector<JuaStackVal>& params);

    JuaOprand resize(std::vector<JuaStackVal>& params);

    JuaOprand insert(std::vector<JuaStackVal>& params);

    JuaOprand erase(std::vector<JuaStackVal>& params);

    JuaOprand run_func_by_symbol(const std::string& name, std::vector<JuaStackVal>& params) override;
};


JuaOprand jua_create_vector(std::vector<JuaStackVal>& params) ;

#endif // JUA_STD_VECTOR
