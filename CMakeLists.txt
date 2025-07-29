cmake_minimum_required(VERSION 3.10)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
project(JuaSysTest)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)



option(BUILD_SHARED_LIBS "Build libraries as shared objects" ON)


add_library(JuaLang
    SHARED
    lib/JuaCompiler/JuaScope.cpp 
    lib/JuaCompiler/JuaLangUtilFuncs.cpp
    lib/JuaCompiler/JuaLangInitFuncs.cpp
    lib/JuaCompiler/JuaLangActionFuncs.cpp
    lib/JuaCompiler/JuaMacroScope.cpp
    lib/JuaCompiler/JuaLangNativeFunc.cpp
    lib/JuaInterpter/JuaInterpter_Types.cpp
    lib/JuaInterpter/JuaInterpter.cpp
    lib/std/preprocesses/util.cpp
    lib/std/console/io.cpp
    lib/std/type/jua_str.cpp
    lib/std/math/JuaStdMath.cpp
    lib/std/structures/vector.cpp
    lib/std/structures/hashmap.cpp
)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")






add_executable(JuaTest
    main.cpp
)


target_link_libraries(JuaTest
    PRIVATE
      JuaLang
)
