cmake_minimum_required(VERSION 3.10)  
project(JuaSysTest)             


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


add_executable(JuaTest
    main.cpp                      
    JuaCompiler/JuaScope.cpp 
    JuaCompiler/JuaLangUtilFuncs.cpp
    JuaCompiler/JuaLangInitFuncs.cpp
    JuaCompiler/JuaLangActionFuncs.cpp
    JuaInterpter/JuaInterpter_Types.cpp
)


target_include_directories(JuaTest PRIVATE JuaCompiler JuaInterpter sys)
