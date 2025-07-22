cmake_minimum_required(VERSION 3.10)
project(JuaSysTest)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


option(BUILD_SHARED_LIBS "Build libraries as shared objects" ON)


add_library(JuaLang
    SHARED
    JuaCompiler/JuaScope.cpp 
    JuaCompiler/JuaLangUtilFuncs.cpp
    JuaCompiler/JuaLangInitFuncs.cpp
    JuaCompiler/JuaLangActionFuncs.cpp
    JuaCompiler/JuaMacroScope.cpp
    JuaInterpter/JuaInterpter_Types.cpp
    JuaInterpter/JuaInterpter.cpp
)

target_include_directories(JuaLang
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/JuaCompiler
      ${CMAKE_CURRENT_SOURCE_DIR}/JuaInterpter
      ${CMAKE_CURRENT_SOURCE_DIR}/sys
      ${CMAKE_CURRENT_SOURCE_DIR}/std
)


add_executable(JuaTest
    main.cpp
)


target_link_libraries(JuaTest
    PRIVATE
      JuaLang
)
