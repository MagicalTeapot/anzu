#include "vocabulary.hpp"

#include <string_view>
#include <unordered_set>

namespace anzu {

auto is_keyword(std::string_view token) -> bool
{
    static const std::unordered_set<std::string_view> tokens = {
        tk_break, tk_continue, tk_do, tk_elif, tk_else, tk_end,
        tk_false, tk_for, tk_if, tk_in, tk_null, tk_true, tk_while
    };
    return tokens.contains(token);
}

auto is_sentinel(std::string_view token) -> bool
{
    static const std::unordered_set<std::string_view> tokens = {
        tk_do, tk_elif, tk_else, tk_end
    };
    return tokens.contains(token);
}

auto is_symbol(std::string_view token) -> bool
{
    static const std::unordered_set<std::string_view> tokens = {
        tk_add, tk_and, tk_assign, tk_colon, tk_comma,
        tk_div, tk_eq, tk_ge, tk_gt, tk_lbracket, tk_le,
        tk_lparen, tk_lt, tk_mod, tk_mul, tk_ne, tk_or,
        tk_period, tk_rbracket, tk_rparen, tk_sub, tk_rarrow
    };
    return tokens.contains(token);
}

}