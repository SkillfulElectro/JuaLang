cmake_minimum_required(VERSION 3.10)  
project(JuaSysTest)             


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


add_executable(JuaTest
    main.cpp                      
    JuaCompiler/JuaScope.cpp 
    JuaCompiler/JuaLangUtilFuncs.cpp
    JuaCompiler/JuaLangInitFuncs.cpp
    JuaCompiler/JuaLangActionFuncs.cpp
    JuaCompiler/JuaMacroScope.cpp
    JuaInterpter/JuaInterpter_Types.cpp
)


target_include_directories(JuaTest PRIVATE JuaCompiler JuaInterpter sys)
