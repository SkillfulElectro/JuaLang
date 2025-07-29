#ifndef JUA_STD_MATH
#define JUA_STD_MATH

#include "JuaLang.h"
#include <cmath>
#include <unordered_map>
#include <functional>
#include <stdexcept>

// every math call/operator returns a double
class JuaStdMath : public JuaVoidType {
public:
    JuaOprand run_func_by_symbol(const std::string& name,
                                 std::vector<JuaStackVal>& params) override;

private:
    // one-arg: math functions + unary operators (names in English)
    static inline const std::unordered_map<std::string, std::function<double(double)>> oneArgMap = {
        // math
        {"abs",         [](double x){ return std::abs(x); }},
        {"acos",        [](double x){ return std::acos(x); }},
        {"asin",        [](double x){ return std::asin(x); }},
        {"atan",        [](double x){ return std::atan(x); }},
        {"ceil",        [](double x){ return std::ceil(x); }},
        {"cos",         [](double x){ return std::cos(x); }},
        {"cosh",        [](double x){ return std::cosh(x); }},
        {"exp",         [](double x){ return std::exp(x); }},
        {"fabs",        [](double x){ return std::fabs(x); }},
        {"floor",       [](double x){ return std::floor(x); }},
        {"log",         [](double x){ return std::log(x); }},
        {"log10",       [](double x){ return std::log10(x); }},
        {"round",       [](double x){ return std::round(x); }},
        {"sin",         [](double x){ return std::sin(x); }},
        {"sinh",        [](double x){ return std::sinh(x); }},
        {"sqrt",        [](double x){ return std::sqrt(x); }},
        {"tan",         [](double x){ return std::tan(x); }},
        {"tanh",        [](double x){ return std::tanh(x); }},
        {"trunc",       [](double x){ return std::trunc(x); }},
        {"cbrt",        [](double x){ return std::cbrt(x); }},
        {"exp2",        [](double x){ return std::exp2(x); }},
        {"log2",        [](double x){ return std::log2(x); }},
        // unary ops
        {"plus",        [](double x){ return +x; }},
        {"negate",      [](double x){ return -x; }},
        {"logical_not", [](double x){ return x == 0.0 ? 1.0 : 0.0; }},
        {"bitwise_not", [](double x){ auto v = static_cast<long long>(x); return static_cast<double>(~v); }},
        {"increment",   [](double x){ return x + 1.0; }},
        {"decrement",   [](double x){ return x - 1.0; }},
    };


    const std::unordered_map<std::string, std::function<double(double,double)>> twoArgMap = {
        // math
        {"atan2",       [](double y,double x){ return std::atan2(y,x); }},
        {"fmod",        [](double x,double y){ return std::fmod(x,y); }},
        {"hypot",       [](double x,double y){ return std::hypot(x,y); }},
        {"ldexp",       [](double x,double n){ return std::ldexp(x,int(n)); }},
        {"pow",         [](double x,double y){ return std::pow(x,y); }},
        {"remainder",    [](double x,double y){ return std::remainder(x,y); }},
        // arithmetic
        {"add",         [](double x,double y){ return x + y; }},
        {"subtract",    [](double x,double y){ return x - y; }},
        {"multiply",    [](double x,double y){ return x * y; }},
        {"divide",      [](double x,double y){ return x / y; }},
        {"modulus",     [](double x,double y){ return std::fmod(x,y); }},
        // bitwise
        {"bitwise_and", [](double x,double y){ auto a=static_cast<long long>(x); auto b=static_cast<long long>(y); return static_cast<double>(a & b); }},
        {"bitwise_or",  [](double x,double y){ auto a=static_cast<long long>(x); auto b=static_cast<long long>(y); return static_cast<double>(a | b); }},
        {"bitwise_xor", [](double x,double y){ auto a=static_cast<long long>(x); auto b=static_cast<long long>(y); return static_cast<double>(a ^ b); }},
        {"left_shift",  [](double x,double y){ auto a=static_cast<long long>(x); auto b=static_cast<int>(y); return static_cast<double>(a << b); }},
        {"right_shift", [](double x,double y){ auto a=static_cast<long long>(x); auto b=static_cast<int>(y); return static_cast<double>(a >> b); }},
        // logical
        {"logical_and", [](double x,double y){ return (x != 0.0 && y != 0.0) ? 1.0 : 0.0; }},
        {"logical_or",  [](double x,double y){ return (x != 0.0 || y != 0.0) ? 1.0 : 0.0; }},
        // comparisons
        {"equal",       [](double x,double y){ return x == y ? 1.0 : 0.0; }},
        {"not_equal",   [](double x,double y){ return x != y ? 1.0 : 0.0; }},
        {"less_than",   [](double x,double y){ return x <  y ? 1.0 : 0.0; }},
        {"less_equal",  [](double x,double y){ return x <= y ? 1.0 : 0.0; }},
        {"greater_than",[](double x,double y){ return x >  y ? 1.0 : 0.0; }},
        {"greater_equal",[](double x,double y){ return x >= y ? 1.0 : 0.0; }},
    };
};


JuaOprand jua_std_math_importer(std::vector<JuaStackVal>& oprands);



#endif // JUA_STD_MATH
