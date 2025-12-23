#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"
#include "msg.h"
#include "file_range.h"

#define HHG_NODE_INDENT_INC 4

static void hhg_node_print_indent(int32_t indent);
static void hhg_node_print_str(const char *str, int32_t indent);
static void hhg_node_print_id(hhg_id_t id, int32_t indent, bool use_sym);

hhg_node_t *hhg_node_new(hhg_arena_t *arena, hhg_node_type_t type)
{
    hhg_node_t *node = hhg_arena_malloc(arena, sizeof(hhg_node_t));

    // initialize type and set other fields to NULL portably
    *node = (hhg_node_t) { .type = type };

    node->value_type = hhg_type_new(HHG_TYPE_NONE, arena);

    return node;
}


void hhg_node_print(hhg_node_t *node, int32_t indent, bool use_sym)
{   
    if (node == NULL)
        return;
    hhg_node_print_indent(indent);

    hhg_token_type_print((hhg_token_type_t)node->type);
    putchar(' ');
    hhg_file_range_print(&node->range);
    putchar('\n');

    int32_t next_indent = indent + HHG_NODE_INDENT_INC;
    int32_t next_next_indent = next_indent + HHG_NODE_INDENT_INC;

    switch (node->type) {
    case HHG_NODE_BLOCK: {
        size_t len = arrlenu(node->value.block.body);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.block.body[i], next_indent, use_sym);
        break;
    }
    case HHG_NODE_PARAM:
        hhg_node_print_id(node->value.param.id, next_indent, use_sym);
        break;
    case HHG_TOKEN_ID:
        hhg_node_print_id(node->value.id, next_indent, use_sym);
        break;
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
        hhg_node_print_str(node->value.literal.str, next_indent);
        break;
    case HHG_TOKEN_IF:
        hhg_node_print(node->value.if_stmt.cond, next_indent, use_sym);
        hhg_node_print(node->value.if_stmt.if_body, next_indent, use_sym);
        break;
    case HHG_TOKEN_WHILE:
        hhg_node_print(node->value.while_stmt.cond, next_indent, use_sym);
        hhg_node_print(node->value.while_stmt.body, next_indent, use_sym);
        break;
    case '=':
        hhg_node_print_id(node->value.var_decl.id, next_indent, use_sym);
        hhg_node_print(node->value.var_decl.expr, next_indent, use_sym);
        break;
    case HHG_NODE_OBJ_INIT: {
        size_t len = arrlenu(node->value.obj_init.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.obj_init.args[i], next_indent, use_sym);
        break;
    }
    case HHG_TOKEN_DEF: {
        hhg_node_print_str("id", next_indent);
        hhg_node_print_id(node->value.func_decl.id, next_next_indent, use_sym);

        hhg_node_print_str("params", next_indent);
        size_t len = arrlenu(node->value.func_decl.params);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(
                node->value.func_decl.params[i],
                next_next_indent,
                use_sym
            );

        hhg_node_print(node->value.func_decl.body, next_indent, use_sym);
        break;
    }
    case HHG_TOKEN_CLASS: {
        hhg_node_print_str("id", next_indent);
        hhg_node_print_id(
            node->value.class_decl.id,
            next_next_indent,
            use_sym
        );
        hhg_node_print_str("var decls", next_indent);
        size_t var_decls_len = arrlenu(node->value.class_decl.var_decls);
        for (size_t i = 0; i < var_decls_len; i++)
            hhg_node_print(
                node->value.class_decl.var_decls[i],
                next_next_indent,
                false // no sym for var decls in class
            );

        hhg_node_print_str("func decls", next_indent);
        size_t func_decls_len = arrlenu(node->value.class_decl.func_decls);
        for (size_t i = 0; i < func_decls_len; i++)
            hhg_node_print(
                node->value.class_decl.func_decls[i],
                next_next_indent,
                false // no sym for func decls in class
            );
        break;
    }
    case '.':
        hhg_node_print_str("id", next_indent);
        hhg_node_print_str(node->value.field_access.str, next_next_indent);
        hhg_node_print(
            node->value.field_access.next,
            next_indent,
            false
        ); // no sym for field access
        break;
    case HHG_NODE_FUNC_CALL: {
        hhg_node_print_str("id", next_indent);
        hhg_node_print_id(node->value.func_call.id, next_next_indent, use_sym);

        hhg_node_print_str("args", next_indent);
        size_t len = arrlenu(node->value.func_call.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(
                node->value.func_call.args[i],
                next_next_indent,
                use_sym
            );
        break;
    }
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        break;
    case HHG_TOKEN_RETURN:
        hhg_node_print(node->value.ret.expr, next_indent, use_sym);
        break;
    case HHG_NODE_ARR_LITERAL: {
        size_t len = arrlenu(node->value.arr_literal.elems);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(
                node->value.arr_literal.elems[i],
                next_indent,
                use_sym
            );
        break;
    }
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '<':
    case '>':
    case '&':
    case '^':
    case '|':
    case HHG_TOKEN_LSHIFT:
    case HHG_TOKEN_RSHIFT:
    case HHG_TOKEN_EQ:
    case HHG_TOKEN_NOT_EQ:
    case HHG_TOKEN_LT_EQ:
    case HHG_TOKEN_GT_EQ:
    case HHG_TOKEN_PLUS_EQ:
    case HHG_TOKEN_SUB_EQ:
    case HHG_TOKEN_MUL_EQ:
    case HHG_TOKEN_DIV_EQ:
    case HHG_TOKEN_MOD_EQ:
    case HHG_TOKEN_AND_EQ:
    case HHG_TOKEN_OR_EQ:
    case HHG_TOKEN_XOR_EQ:
    case HHG_TOKEN_LSHIFT_EQ:
    case HHG_TOKEN_RSHIFT_EQ:
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC:
    case HHG_TOKEN_AND:
    case HHG_TOKEN_OR:
        hhg_node_print(node->value.expr.left, next_indent, use_sym);
        hhg_node_print(node->value.expr.right, next_indent, use_sym);
        break;
    default:
        hhg_fatal_error(
            "unhandled node type %s in hhg_node_print",
            hhg_token_type_to_str(node->type)
        );
        break;
    }
}

void hhg_node_del(hhg_node_t *node)
{
    hhg_type_del(node->value_type);
}

static void hhg_node_print_indent(int32_t indent)
{
    for (int32_t i = 0; i < indent; i++) {
        putchar(' ');
    }
}

static void hhg_node_print_str(const char *str, int32_t indent)
{
    if (str) {
        hhg_node_print_indent(indent);
        puts(str);
    }
}

static void hhg_node_print_id(hhg_id_t id, int32_t indent, bool use_sym)
{
    hhg_node_print_indent(indent);
    if (use_sym) {
        hhg_type_print(id.sym->value.type);
        printf(" %s\n", id.sym->key);
    } else
        puts(id.str);
}
