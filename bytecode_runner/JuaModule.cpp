// JuaModule.cpp
#include "JuaModule.h"
#include "JuaInterpter.h" 

JuaModule::JuaModule(const std::string& func_name, JuaInterpter* instance) {
    instance->add_extension(func_name, this);
}
