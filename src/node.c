#if 0 // lexer-only: disabled while token set is in flux
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"
#include "msg.h"
#include "utils.h"

#define HHG_NODE_INDENT_INC 4

static void hhg_node_fprint_core(
    hhg_node_t *node,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
);
static void hhg_node_fprint_indent(int32_t indent, FILE *stream);
static void hhg_node_fprint_str(const char *str, int32_t indent, FILE *stream);
static void hhg_node_fprint_id(
    hhg_id_t id,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
);

void hhg_node_type_print(hhg_node_type_t type)
{
    hhg_token_type_print((hhg_token_type_t)type);
}

void hhg_node_type_fprint(hhg_node_type_t type, FILE *stream)
{
    hhg_token_type_fprint((hhg_token_type_t)type, stream);
}

hhg_node_t *hhg_node_new(
    hhg_arena_t *arena,
    hhg_node_type_t type,
    hhg_file_src_t *src
)
{
    hhg_node_t *node = hhg_arena_malloc(arena, sizeof(hhg_node_t));

    // initialize type and src, setting other fields to NULL portably
    *node = (hhg_node_t) { .type = type, .src = src };

    return node;
}

void hhg_node_print(hhg_node_t *node, hhg_node_print_mode_t mode)
{
    hhg_node_fprint(node, mode, stdout);
}

void hhg_node_fprint(
    hhg_node_t *node,
    hhg_node_print_mode_t mode,
    FILE *stream
)
{
    hhg_node_fprint_core(node, 0, mode, stream);
}



void hhg_node_free(hhg_node_t *node)
{
    hhg_type_del(node->value_type);
}


static void hhg_node_fprint_core(
    hhg_node_t *node,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
)
{   
    if (node == NULL)
        return;
    hhg_node_fprint_indent(indent, stream);

    hhg_node_type_fprint(node->type, stream);
    
    fputc(' ', stream);
    hhg_type_fprint(node->value_type, stream);

    int32_t next_indent = indent + HHG_NODE_INDENT_INC;
    int32_t next_next_indent = next_indent + HHG_NODE_INDENT_INC;

    switch (node->type) {
    case HHG_NODE_BLOCK: {
        size_t len = arrlenu(node->value.block.body);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(
                node->value.block.body[i],
                next_indent,
                mode,
                stream
            );
        break;
    }
    case HHG_NODE_PARAM:
        hhg_node_fprint_id(
            node->value.param.id,
            next_indent,
            mode,
            stream
        );
        break;
    case HHG_TOKEN_ID:
        hhg_node_fprint_id(
            node->value.id,
            next_indent,
            mode,
            stream
        );
        break;
    case HHG_TOKEN_STR_LIT:
    case HHG_TOKEN_INT_LIT:
    case HHG_TOKEN_FLOAT_LIT:
        hhg_node_fprint_str(
            node->value.lit.str,
            next_indent,
            stream
        );
        break;
    case HHG_TOKEN_IF:
        hhg_node_fprint_core(
            node->value.if_stmt.cond,
            next_indent,
            mode,
            stream
        );
        hhg_node_fprint_core(
            node->value.if_stmt.if_body,
            next_indent,
            mode,
            stream
        );
        break;
    case HHG_TOKEN_WHILE:
        hhg_node_fprint_core(
            node->value.while_stmt.cond,
            next_indent,
            mode,
            stream
        );
        hhg_node_fprint_core(
            node->value.while_stmt.body,
            next_indent,
            mode,
            stream
        );
        break;
    case '=':
        hhg_node_fprint_id(
            node->value.var_decl.id,
            next_indent,
            mode,
            stream
        );
        hhg_node_fprint_core(
            node->value.var_decl.expr,
            next_indent,
            mode,
            stream
        );
        break;
    case HHG_NODE_OBJ_INIT: {
        size_t len = arrlenu(node->value.obj_init.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(
                node->value.obj_init.args[i],
                next_indent,
                mode,
                stream
            );
        break;
    }
    case HHG_TOKEN_DEF: {
        hhg_node_fprint_str(
            "id",
            next_indent,
            stream
        );
        hhg_node_fprint_id(
            node->value.func_decl.id, 
            next_next_indent,
            mode,
            stream
        );

        hhg_node_fprint_str("params", next_indent, stream);
        size_t len = arrlenu(node->value.func_decl.params);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(
                node->value.func_decl.params[i],
                next_next_indent,
                mode,
                stream
            );

        hhg_node_fprint_core(
            node->value.func_decl.body,
            next_indent,
            mode,
            stream
        );
        break;
    }
    case HHG_TOKEN_CLASS: {
        hhg_node_fprint_str("id", next_indent, stream);
        hhg_node_fprint_id(
            node->value.class_decl.id,
            next_next_indent,
            mode,
            stream
        );
        hhg_node_fprint_str("var decls", next_indent, stream);
        size_t var_decls_len = arrlenu(node->value.class_decl.var_decls);
        for (size_t i = 0; i < var_decls_len; i++)
            hhg_node_fprint_core(
                node->value.class_decl.var_decls[i],
                next_next_indent,
                HHG_NODE_PRINT_MODE_NO_SYM, // no sym for var decls in class
                stream
            );

        hhg_node_fprint_str("func decls", next_indent, stream);
        size_t func_decls_len = arrlenu(node->value.class_decl.func_decls);
        for (size_t i = 0; i < func_decls_len; i++)
            hhg_node_fprint_core(
                node->value.class_decl.func_decls[i],
                next_next_indent,
                HHG_NODE_PRINT_MODE_NO_SYM, // no sym for func decls in class
                stream
            );
        break;
    }
    case '.':
        hhg_node_fprint_str("id", next_indent, stream);
        hhg_node_fprint_str(
            node->value.field_access.str,
            next_next_indent,
            stream
        );
        hhg_node_fprint_core(
            node->value.field_access.next,
            next_indent,
            HHG_NODE_PRINT_MODE_NO_SYM, // no sym for field access
            stream
        ); 
        break;
    case HHG_NODE_FUNC_CALL: {
        hhg_node_fprint_str("id", next_indent, stream);
        hhg_node_fprint_id(
            node->value.func_call.id,
            next_next_indent,
            mode,
            stream
        );

        hhg_node_fprint_str("args", next_indent, stream);
        size_t len = arrlenu(node->value.func_call.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(
                node->value.func_call.args[i],
                next_next_indent,
                mode,
                stream
            );
        break;
    }
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        break;
    case HHG_TOKEN_RETURN:
        hhg_node_fprint_core(
            node->value.ret.expr,
            next_indent,
            mode,
            stream
        );
        break;
    case HHG_NODE_ARR_LIT: {
        size_t len = arrlenu(node->value.arr_lit.elems);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(
                node->value.arr_lit.elems[i],
                next_indent,
                mode,
                stream
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
    case HHG_TOKEN_ADD_EQ:
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
        hhg_node_fprint_core(
            node->value.expr.left,
            next_indent,
            mode,
            stream
        );
        hhg_node_fprint_core(
            node->value.expr.right,
            next_indent,
            mode,
            stream
        );
        break;
    default:
        hhg_fatal_error(
            "unhandled node type %t in hhg_node_fprint_core",
            node->type
        );
        break;
    }
}

static void hhg_node_fprint_indent(int32_t indent, FILE *stream)
{
    for (int32_t i = 0; i < indent; i++)
       fputc(' ', stream);
}

static void hhg_node_fprint_str(const char *str, int32_t indent, FILE *stream)
{
    if (str) {
        hhg_node_fprint_indent(indent, stream);
        fputs(str, stream);
        fputc('\n', stream);
    }
}

static void hhg_node_fprint_id(
    hhg_id_t id,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
)
{
    hhg_node_fprint_indent(indent, stream);
    if (mode == HHG_NODE_PRINT_MODE_SYM) {
        hhg_type_fprint(id.sym->value.type, stream);
        fputc(' ', stream);
        fputs(id.sym->key, stream);
    } else
        fputs(id.str, stream);
}
#endif // lexer-only
