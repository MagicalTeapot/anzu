#include "object.hpp"
#include "vocabulary.hpp"
#include "utility/print.hpp"
#include "utility/overloaded.hpp"

#include <algorithm>
#include <ranges>
#include <string_view>

namespace anzu {
namespace {

auto format_error(const std::string& str) -> void
{
    anzu::print("format error: could not format special chars in '{}'\n", str);
    std::exit(1);
}

}

auto to_string(const object& object) -> std::string
{
    return std::format("{}({})", object.type, format_comma_separated(object.data));
}

auto to_string(const type_name& type) -> std::string
{
    return std::visit([](const auto& t) { return to_string(t); }, type);
}

auto to_string(const type_simple& type) -> std::string
{
    return type.name;
}

auto to_string(const type_list& type) -> std::string
{
    return std::format("{}[{}]", to_string(*type.inner_type), type.count);
}

auto to_string(const type_ptr& type) -> std::string
{
    return std::format("&{}", to_string(*type.inner_type));
}

auto hash(const type_name& type) -> std::size_t
{
    return std::visit([](const auto& t) { return hash(t); }, type);
}

auto hash(const type_simple& type) -> std::size_t
{
    return std::hash<std::string>{}(type.name);
}

auto hash(const type_list& type) -> std::size_t
{
    return hash(*type.inner_type) ^ std::hash<std::size_t>{}(type.count);
}

auto hash(const type_ptr& type) -> std::size_t
{
    static const auto ptr_offset = std::hash<std::string_view>{}("ptr_offset");
    return hash(*type.inner_type) ^ ptr_offset;
}

auto i32_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_i32} }};
}

auto i64_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_i64} }};
}

auto u64_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_u64} }};
}

auto char_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_char} }};
}

auto f64_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_f64} }};
}

auto bool_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_bool} }};
}

auto null_type() -> type_name
{
    return {type_simple{ .name = std::string{tk_null} }};
}

auto make_type(const std::string& name) -> type_name
{
    return { type_simple{ .name=name } };
}

auto concrete_list_type(const type_name& t, std::size_t size) -> type_name
{
    return {type_list{ .inner_type = { t }, .count = size }};
}

auto is_list_type(const type_name& t) -> bool
{
    return std::holds_alternative<type_list>(t);
}

auto concrete_ptr_type(const type_name& t) -> type_name
{
    return {type_ptr{ .inner_type = { t } }};
}

auto is_ptr_type(const type_name& t) -> bool
{
    return std::holds_alternative<type_ptr>(t);
}

auto inner_type(const type_name& t) -> type_name
{
    if (is_list_type(t)) {
        return *std::get<type_list>(t).inner_type;
    }
    if (is_ptr_type(t)) {
        return *std::get<type_ptr>(t).inner_type;
    }
    print("OH NO MY TYPE\n");
    std::exit(1);
    return {};
}

auto is_type_fundamental(const type_name& type) -> bool
{
    return type == i32_type()
        || type == i64_type()
        || type == u64_type()
        || type == f64_type()
        || type == char_type()
        || type == bool_type()
        || type == null_type();
}

// Loads each key/value pair from src into dst. If the key already exists in dst and has a
// different value, stop and return false.
auto update(
    std::unordered_map<int, type_name>& dst, const std::unordered_map<int, type_name>& src
)
    -> bool
{
    for (const auto& [key, value] : src) {
        if (auto it = dst.find(key); it != dst.end()) {
            if (it->second != value) {
                return false;
            }
        } else {
            dst.emplace(key, value);
        }
    }
    return true;
}

auto to_string(const signature& sig) -> std::string
{
    const auto proj = [](const auto& arg) { return arg.type; };
    return std::format("({}) -> {}", format_comma_separated(sig.params, proj), sig.return_type);
}

auto type_store::add(const type_name& name, const type_fields& fields) -> bool
{
    if (d_classes.contains(name)) {
        return false;
    }
    d_classes.emplace(name, fields);
    return true;
}

auto type_store::contains(const type_name& type) const -> bool
{
    return d_classes.contains(type)
        || is_type_fundamental(type)
        || is_list_type(type)
        || is_ptr_type(type);
}

auto type_store::size_of(const type_name& type) const -> std::size_t
{
    if (!contains(type)) {
        print("unknown type '{}'\n", type);
        std::exit(1);
    }

    if (is_ptr_type(type)) {
        return 8; // Two unsigned ints, ptr and size
    }

    if (type == i32_type()) {
        return 4;
    }

    if (type == i64_type() || type == f64_type() || type == u64_type()) {
        return 8;
    }

    if (is_type_fundamental(type)) {
        return 1;
    }
    
    return std::visit(overloaded{
        [&](const type_simple& t) {
            auto size = std::size_t{0};
            for (const auto& field : fields_of(type)) {
                size += size_of(field.type);
            }
            return size;
        },
        [&](const type_list& t) {
            return size_of(*t.inner_type) * t.count;
        },
        [](const type_ptr&) {
            return std::size_t{1};
        }
    }, type);
}

auto type_store::fields_of(const type_name& t) const -> type_fields
{
    if (auto it = d_classes.find(t); it != d_classes.end()) {
        return it->second;
    }
    return {};
}

}