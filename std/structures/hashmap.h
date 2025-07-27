#ifndef JUA_STD_HASHMAP
#define JUA_STD_HASHMAP

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdlib>
#include "JuaLang.h"


namespace std {
    template<>
    struct hash<JuaOprand> {
        size_t operator()(const JuaOprand& s) const noexcept {
            switch (s.op_type) {
            case STRING: {

                const std::string& tmp =
                    const_cast<JuaOprand&>(s).get_str();
                return std::hash<std::string>{}(tmp);
            }

            case DOUBLE: {
                double tmp =
                    const_cast<JuaOprand&>(s).get_doub();
                return std::hash<double>{}(tmp);
            }

            case VOID: {
                JuaVoidType* ptr =
                    const_cast<JuaOprand&>(s).get_void_ptr();
                if (ptr->is_hashable()) {
                    return ptr->hash_val();
                }
                break;
            }

            default:
                break;
            }

            return 0;
        }
    };
}

namespace std {
    template<>
    struct equal_to<JuaOprand> {
        bool operator()(const JuaOprand& a, const JuaOprand& b) const noexcept {
            if (a.op_type != b.op_type) return false;
            switch (a.op_type) {
            case STRING:

                return const_cast<JuaOprand&>(a).get_str() ==
                       const_cast<JuaOprand&>(b).get_str();
            case DOUBLE:
                return const_cast<JuaOprand&>(a).get_doub() ==
                       const_cast<JuaOprand&>(b).get_doub();
            case VOID: {
                auto pa = const_cast<JuaOprand&>(a).get_void_ptr();
                auto pb = const_cast<JuaOprand&>(b).get_void_ptr();

                if (pa == pb) return true;
                if (pa->is_equatable()) return pa->equals(pb);
                return false;
            }
            default:
                return false;
            }
        }
    };
}



class JuaStdHashMap : public JuaVoidType {
    std::unordered_map<JuaOprand, JuaOprand> map;


    void get_oprand(JuaStackVal& stack_val, JuaOprand*& out_ptr, JuaOprand& tmp) {
        if (stack_val.type == REF) {
            out_ptr = stack_val.get_ptr();
        } else {
            tmp = stack_val.get_obj();
            out_ptr = &tmp;
        }
    }

public:
    JuaStdHashMap(std::vector<JuaStackVal>& params) {
        for (size_t i = 0; i + 1 < params.size(); i += 2) {
            JuaOprand *key_ptr, *val_ptr;
            JuaOprand tmp_key, tmp_val;
            get_oprand(params[i], key_ptr, tmp_key);
            get_oprand(params[i+1], val_ptr, tmp_val);
            map[*key_ptr] = *val_ptr;
        }
    }

    JuaOprand run_func_by_symbol(const std::string& name, std::vector<JuaStackVal>& params) override {
        if (name == "size") {
            return JuaOprand{DOUBLE, static_cast<double>(map.size())};
        }
        if (name == "empty") {
            return JuaOprand{DOUBLE, map.empty() ? 1.0 : 0.0};
        }
        if (name == "clear") {
            map.clear();
            return JuaOprand{DOUBLE, 0.0};
        }
        if (name == "get") {
            // todo
        }
        if (name == "set") {
            //todo
        }
        if (name == "erase") {
            // todo
        }
        if (name == "contains") {
            //todo
        }
        return JuaOprand{DOUBLE, 0.0};
    }
};


inline JuaOprand jua_create_hash_map(std::vector<JuaStackVal>& params) {
    JuaOprand ret{VOID, new JuaStdHashMap(params)};
    ret.destructor = [](JuaOprand* obj) { delete obj->get_void_ptr(); };
    return ret;
}

#endif // JUA_STD_HASHMAP
