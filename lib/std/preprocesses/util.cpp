#include "JuaLang.h"


namespace fs = std::filesystem;

std::string import(const std::vector<DFActionToken>& tokens) {

    if (tokens.size() < 4
        || ( std::get<std::string>(tokens[0].value) != "import" && std::get<std::string>(tokens[0].value) != "include" )
        || std::get<std::string>(tokens[1].value) != "("
        || std::get<std::string>(tokens.back().value) != ")"
        || tokens[2].type != CONST_STRING)
    {
        return "";
    }


    std::string literal = std::get<std::string>(tokens[2].value);
    if (literal.size() < 2) return "";
    std::string fname = literal.substr(1, literal.size() - 2);


    fs::path p = fs::u8path(fname);


    std::ifstream file(p, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return "";
    }


    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size <= 0) {
        return "";
    }
    std::string contents;
    contents.resize(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);
    file.read(contents.data(), size);


    contents.erase(
      std::remove(contents.begin(), contents.end(), '\r'),
      contents.end()
    );

    return contents;
}
