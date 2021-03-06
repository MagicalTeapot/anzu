#pragma once
#include "object.hpp"
#include "functions.hpp"
#include "token.hpp"

#include <variant>
#include <vector>
#include <memory>

namespace anzu {

struct node_expr;
using node_expr_ptr = std::unique_ptr<node_expr>;

struct node_literal_expr
{
    anzu::object value;

    anzu::token token;
};

struct node_variable_expr
{
    std::string name;

    anzu::token token;
};

struct node_field_expr
{
    node_expr_ptr expr;
    std::string   field_name;

    anzu::token token;
};

struct node_unary_op_expr
{
    node_expr_ptr expr;

    anzu::token token;
};

struct node_binary_op_expr
{
    node_expr_ptr lhs;
    node_expr_ptr rhs;

    anzu::token token;
};

struct node_function_call_expr
{
    std::string                function_name;
    std::vector<node_expr_ptr> args;

    anzu::token token;
};

struct node_member_function_call_expr
{
    node_expr_ptr              expr;
    std::string                function_name;
    std::vector<node_expr_ptr> args;

    anzu::token token;
};

struct node_list_expr
{
    std::vector<node_expr_ptr> elements;

    anzu::token token;
};

struct node_repeat_list_expr
{
    node_expr_ptr value;
    std::size_t   size;

    anzu::token token;
};

struct node_addrof_expr
{
    node_expr_ptr expr;

    anzu::token token;
};

struct node_deref_expr
{
    node_expr_ptr expr;
    
    anzu::token token;
};

struct node_sizeof_expr
{
    node_expr_ptr expr;

    anzu::token token;
};

struct node_subscript_expr
{
    node_expr_ptr expr;
    node_expr_ptr index;
    
    anzu::token token;
};

struct node_new_expr
{
    type_name     type;
    node_expr_ptr size;
    
    anzu::token token;
};

struct node_expr : std::variant<
    // Rvalue expressions
    node_literal_expr,
    node_unary_op_expr,
    node_binary_op_expr,
    node_function_call_expr,
    node_member_function_call_expr,
    node_list_expr,
    node_repeat_list_expr,
    node_addrof_expr,
    node_sizeof_expr,
    node_new_expr,

    // Lvalue expressions
    node_variable_expr,
    node_field_expr,
    node_deref_expr,
    node_subscript_expr>
{
};

struct node_stmt;
using node_stmt_ptr = std::unique_ptr<node_stmt>;

struct node_sequence_stmt
{
    std::vector<node_stmt_ptr> sequence;

    anzu::token token;
};

struct node_while_stmt
{
    node_expr_ptr condition;
    node_stmt_ptr body;

    anzu::token token;
};

struct node_if_stmt
{
    node_expr_ptr condition;
    node_stmt_ptr body;
    node_stmt_ptr else_body;

    anzu::token token;
};

struct node_struct_stmt
{
    std::string                name;
    type_fields                fields;
    std::vector<node_stmt_ptr> functions;

    anzu::token token;
};

struct node_break_stmt
{
    anzu::token token;
};

struct node_continue_stmt
{
    anzu::token token;
};

struct node_declaration_stmt
{
    std::string   name;
    node_expr_ptr expr;

    anzu::token token;
};

struct node_assignment_stmt
{
    node_expr_ptr position;
    node_expr_ptr expr;

    anzu::token token;
};

struct node_function_def_stmt
{
    std::string   name;
    signature     sig;
    node_stmt_ptr body;

    anzu::token token;
};

struct node_member_function_def_stmt
{
    std::string   struct_name;
    std::string   function_name;
    signature     sig;
    node_stmt_ptr body;

    anzu::token token;
};

struct node_expression_stmt
{
    node_expr_ptr expr;

    anzu::token token;
};

struct node_return_stmt
{
    node_expr_ptr return_value;

    anzu::token token;
};

struct node_delete_stmt
{
    node_expr_ptr expr;

    anzu::token token;
};

struct node_stmt : std::variant<
    node_sequence_stmt,
    node_while_stmt,
    node_if_stmt,
    node_struct_stmt,
    node_break_stmt,
    node_continue_stmt,
    node_declaration_stmt,
    node_assignment_stmt,
    node_member_function_def_stmt,
    node_function_def_stmt,
    node_expression_stmt,
    node_return_stmt,
    node_delete_stmt>
{
};

auto print_node(const anzu::node_expr& node, int indent = 0) -> void;
auto print_node(const anzu::node_stmt& node, int indent = 0) -> void;

}