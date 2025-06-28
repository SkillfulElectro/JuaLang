#ifndef JUA_MACRO_SCOPE
#define JUA_MACRO_SCOPE

#include <string>
#include <vector>
#include <unordered_map>

struct JuaMacroDef {
	std::string macro_code;
	std::vector<std::string> paras_addrs;
};

typedef std::unordered_map<std::string, JuaMacroDef> JuaMacroScope;

class JuaMacroScopeManager {
    std::vector<JuaMacroScope> scopes;
public:
    void create_scope();

    void destroy_scope();

    bool key_exists(const std::string& key);

    JuaMacroDef& get_by_key(const std::string& key);
};

#endif