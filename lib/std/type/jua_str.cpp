#include "JuaLang.h"


JuaOprand jua_create_string_util(std::vector<JuaStackVal>& oprands) {
    JuaOprand ret{ VOID, new JuaStdString };
    ret.destructor = [](JuaOprand* obj) {
        delete obj->get_void_ptr();
    };
    return ret;
}