#ifndef JUA_STD_PREPROCESS
#define JUA_STD_PREPROCESS

#include "JuaLang.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

std::string import(const std::vector<DFActionToken>& tokens) {
    // 1) Simple syntax check
    if (tokens.size() < 4
        || std::get<std::string>(tokens[0].value) != "import"
        || std::get<std::string>(tokens[1].value) != "("
        || std::get<std::string>(tokens.back().value) != ")"
        || tokens[2].type != CONST_STRING)
    {
        return "";
    }

    // 2) Extract the literal string, strip the quotes 
    std::string literal = std::get<std::string>(tokens[2].value);
    if (literal.size() < 2) return "";
    std::string fname = literal.substr(1, literal.size() - 2);

    // 3) Build a portable path
    fs::path p = fs::u8path(fname);

    // 4) Open in binary mode to avoid any CR/LF or locale translations
    std::ifstream file(p, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    // 5) Read size & contents
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size <= 0) {
        return "";
    }
    std::string contents;
    contents.resize(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);
    file.read(contents.data(), size);

    // 6) Normalize Windows CRLF ("\r\n" → "\n") on all platforms
    contents.erase(
      std::remove(contents.begin(), contents.end(), '\r'),
      contents.end()
    );

    return contents;
}

#endif // JUA_STD_PREPROCESS
