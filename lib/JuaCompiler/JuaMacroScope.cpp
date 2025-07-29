#include "JuaLang.h"

void JuaMacroScopeManager::create_scope() {
    scopes.push_back(JuaMacroScope{});
}

void JuaMacroScopeManager::destroy_scope() {
    scopes.pop_back();
}

bool JuaMacroScopeManager::key_exists(const std::string& key) {
    for (auto& scope {--scopes.end()} ; ; --scope) {
        if (scope->find(key) != scope->end()) {
            return true;
        }

        if (scope == scopes.begin()) {
            break;
        }
    }

    return false;
}

JuaMacroDef& JuaMacroScopeManager::get_by_key(const std::string& key) {
    for (auto& scope {--scopes.end()} ; ; --scope) {
        if (scope->find(key) != scope->end()) {
            return scope->at(key);
        }

        if (scope == scopes.begin()) {
            break;
        }
    }

    scopes.back()[key] = JuaMacroDef{};

    return scopes.back()[key];
}