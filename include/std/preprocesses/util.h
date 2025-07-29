#ifndef JUA_STD_PREPROCESS
#define JUA_STD_PREPROCESS

#include "JuaLang.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

std::string import(const std::vector<DFActionToken>& tokens) ;

#endif // JUA_STD_PREPROCESS
