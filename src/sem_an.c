#include <stdbool.h>
#include <string.h>

#include <stb_ds.h>

#include "sem_an.h"
#include "sym.h"
#include "sym_tab.h"
#include "node.h"
#include "msg.h"
#include "type.h"
#include "type_ctx.h"

static void hhg_sem_an_run_children(
    hhg_sem_an_t *sem_an,
    hhg_node_t **children
);
static void hhg_sem_an_run_block(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_id(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_if(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_while(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_var_decl(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_obj_init(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_func_decl(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_class_decl(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_add_class_var_decls(
    hhg_node_t *node,
    hhg_type_t *class_type
);
static void hhg_sem_an_add_class_func_decls(
    hhg_sem_an_t *sem_an,
    hhg_node_t *node,
    hhg_type_t *class_type
);
static void hhg_sem_an_add_class_func_params(
    hhg_sem_an_t *sem_an,
    hhg_node_t *node,
    hhg_type_t *class_type
);
static void hhg_sem_an_run_func_call(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_return(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_arr_literal(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_expr(hhg_sem_an_t *sem_an, hhg_node_t *node);
static void hhg_sem_an_run_inc_dec(hhg_sem_an_t *sem_an, hhg_node_t *node);

void hhg_sem_an_init(
    hhg_sem_an_t *sem_an,
    hhg_sym_tab_t *sym_tab,
    hhg_type_ctx_t *type_ctx,
    hhg_arena_t *arena
)
{
    *sem_an = (hhg_sem_an_t) {
        .sym_tab = sym_tab,
        .type_ctx = type_ctx,
        .arena = arena,
    };
}

void hhg_sem_an_run(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    switch (node->type) {
    case HHG_NODE_BLOCK:
        hhg_sem_an_run_block(sem_an, node);
        break;
    case HHG_TOKEN_ID:
        hhg_sem_an_run_id(sem_an, node);
        break;
    case HHG_TOKEN_IF:
        hhg_sem_an_run_if(sem_an, node);
        break;
    case HHG_TOKEN_WHILE:
        hhg_sem_an_run_while(sem_an, node);
        break;
    case '=':
        hhg_sem_an_run_var_decl(sem_an, node);
        break;
    case HHG_NODE_OBJ_INIT:
        hhg_sem_an_run_obj_init(sem_an, node);
        break;
    case HHG_TOKEN_DEF:
        hhg_sem_an_run_func_decl(sem_an, node);
        break;
    case HHG_TOKEN_CLASS:
        hhg_sem_an_run_class_decl(sem_an, node);
        break;
    case '.':
        // field access / method access handling not implemented yet
        break;
    case HHG_NODE_FUNC_CALL:
        hhg_sem_an_run_func_call(sem_an, node);
        break;
    case HHG_TOKEN_RETURN:
        hhg_sem_an_run_return(sem_an, node);
        break;
    case HHG_NODE_ARR_LITERAL:
        hhg_sem_an_run_arr_literal(sem_an, node);
        break;
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
    case HHG_TOKEN_AND:
    case HHG_TOKEN_OR:
        hhg_sem_an_run_expr(sem_an, node);
        break;
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC:
        hhg_sem_an_run_inc_dec(sem_an, node);
        break;
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
    case HHG_NODE_PARAM:
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
        break;
    default:
        hhg_fatal_error(
            "unhandled node type %s in hhg_sem_an_run",
            hhg_token_type_to_str(node->type)
        );
        break;
    }
}

static void hhg_sem_an_run_children(
    hhg_sem_an_t *sem_an,
    hhg_node_t **children
)
{
    size_t len = arrlenu(children);
    for (size_t i = 0; i < len; i++)
        hhg_sem_an_run(sem_an, children[i]);
}

static void hhg_sem_an_run_block(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sym_tab_enter_scope(sem_an->sym_tab);
    hhg_sem_an_run_children(sem_an, node->value.block.body);
    hhg_sym_tab_exit_scope(sem_an->sym_tab);
}

static void hhg_sem_an_run_id(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sym_t *sym =
        hhg_sym_tab_lookup(sem_an->sym_tab, node->value.id.str);
    if (sym == NULL)
        hhg_fatal_error(
            "undefined variable \"%s\"",
            node->value.id.str
        );
    node->value.id.sym = sym;
}

static void hhg_sem_an_run_if(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run(sem_an, node->value.if_stmt.cond);
    hhg_sem_an_run(sem_an, node->value.if_stmt.if_body);
}

static void hhg_sem_an_run_while(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run(sem_an, node->value.while_stmt.cond);
    hhg_sem_an_run(sem_an, node->value.while_stmt.body);
}

static void hhg_sem_an_run_var_decl(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    // run sem an on expr first to handle x = x where x is not defined yet
    hhg_sem_an_run(sem_an, node->value.var_decl.expr);

    // insert variable into symbol table if not done already
    hhg_sym_t *sym =
        hhg_sym_tab_lookup(sem_an->sym_tab, node->value.var_decl.id.str);
    if (sym == NULL) {
        node->value.var_decl.id.sym = hhg_sym_tab_insert(
            sem_an->sym_tab,
            (hhg_sym_t) {
                .key = node->value.var_decl.id.str,
                .value = {
                    .sym_type = HHG_SYM_VAR,
                    .type = node->value_type,
                },
            }
        );
    }
}

static void hhg_sem_an_run_obj_init(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run_children(sem_an, node->value.obj_init.args);
}

static void hhg_sem_an_run_func_decl(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sym_t *sym =
        hhg_sym_tab_lookup(sem_an->sym_tab, node->value.func_decl.id.str);
    hhg_sym_tab_enter_scope(sem_an->sym_tab);
    if (sym == NULL) {
        hhg_type_t *func_type = hhg_type_new(HHG_TYPE_FUNC, sem_an->arena);
        func_type->info.func = (hhg_type_func_info_t) {
            .ret = node->value_type,
            .params = NULL,
        };

        node->value_type = func_type;
        
        size_t len = arrlenu(node->value.func_decl.params);
        for (size_t i = 0; i < len; i++) {
            hhg_node_t *param = node->value.func_decl.params[i];

            arrput(func_type->info.func.params, param->value_type);

            hhg_sym_t *param_sym = hhg_sym_tab_lookup(
                sem_an->sym_tab,
                param->value.param.id.str
            );

            if (param_sym == NULL) {
                param->value.param.id.sym = hhg_sym_tab_insert(
                    sem_an->sym_tab,
                    (hhg_sym_t) {
                        .key = param->value.param.id.str,
                        .value = {
                            .sym_type = HHG_SYM_PARAM,
                            .type = param->value_type,
                        },
                    }
                );
            } else
                hhg_fatal_error(
                    "redeclaration of parameter \"%s\"",
                    param->value.param.id.str
                );
        }

        hhg_sym_t func_sym = {
            .key = node->value.func_decl.id.str,
            .value = {
                .sym_type = HHG_SYM_FUNC,
                .type = func_type,
            },
        };

        node->value.func_decl.id.sym =
            hhg_sym_tab_insert(sem_an->sym_tab, func_sym);

        func_type->info.func.sym = node->value.func_decl.id.sym;
    } else
        hhg_fatal_error(
            "redeclaration of function \"%s\"",
            node->value.func_decl.id.str
        );

    hhg_sem_an_run(sem_an, node->value.func_decl.body);
    hhg_sym_tab_exit_scope(sem_an->sym_tab);
}

static void hhg_sem_an_run_class_decl(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sym_t *sym =
        hhg_sym_tab_lookup(sem_an->sym_tab, node->value.class_decl.id.str);
    if (sym == NULL) {
        hhg_type_t *class_type = hhg_type_new(HHG_TYPE_CLASS, sem_an->arena);
        class_type->info.class = (hhg_type_class_info_t) {
            .fields = NULL,
        };

        hhg_sym_t class_sym = {
            .key = node->value.class_decl.id.str,
            .value = {
                .sym_type = HHG_SYM_CLASS,
                .type = class_type,
            },
        };


        node->value_type = class_type;
        node->value.class_decl.id.sym =
            hhg_sym_tab_insert(sem_an->sym_tab, class_sym);

        class_type->info.class.sym = node->value.class_decl.id.sym;

        hhg_sym_tab_enter_scope(sem_an->sym_tab);

        hhg_sym_tab_insert(
            sem_an->sym_tab,
            (hhg_sym_t) {
            .key = "self",
                .value = {
                    .sym_type = HHG_SYM_VAR,
                    .type = class_type,
                },
            }
        );

        hhg_sem_an_add_class_var_decls(node, class_type);
        hhg_sem_an_add_class_func_decls(sem_an, node, class_type);

        hhg_sym_tab_exit_scope(sem_an->sym_tab);
    } else
        hhg_fatal_error(
            "redeclaration of class \"%s\"",
            node->value.class_decl.id.str
        );

}

static void hhg_sem_an_add_class_var_decls(
    hhg_node_t *node,
    hhg_type_t *class_type
)
{
    size_t var_decls_len = arrlenu(node->value.class_decl.var_decls);

    for (size_t i = 0; i < var_decls_len; i++) {
        hhg_node_t *var_decl = node->value.class_decl.var_decls[i];

        hhg_type_class_field_t *entry =
            shgetp_null(
                class_type->info.class.fields,
                var_decl->value.var_decl.id.str
            );
        if (entry == NULL)
            shput(
                class_type->info.class.fields,
                var_decl->value.var_decl.id.str,
                var_decl->value_type
            );
        else
            hhg_fatal_error(
                "redeclaration of field \"%s\" in class \"%s\"",
                var_decl->value.var_decl.id.str,
                node->value.class_decl.id.str
            );
    }
}

static void hhg_sem_an_add_class_func_decls(
    hhg_sem_an_t *sem_an,
    hhg_node_t *node,
    hhg_type_t *class_type
)
{
    size_t func_decls_len = arrlenu(node->value.class_decl.func_decls);
    for (size_t i = 0; i < func_decls_len; i++) {
        hhg_node_t *func_decl = node->value.class_decl.func_decls[i];

        hhg_type_t *func_type = hhg_type_new(HHG_TYPE_FUNC, sem_an->arena);
        func_type->info.func = (hhg_type_func_info_t) {
            .ret = func_decl->value_type,
            .params = NULL,
        };

        hhg_sym_tab_enter_scope(sem_an->sym_tab);

        hhg_sem_an_add_class_func_params(sem_an, func_decl, func_type);

        hhg_type_class_field_t *entry =
            shgetp_null(
                class_type->info.class.fields,
                func_decl->value.func_decl.id.str
            );
        if (entry == NULL)
            shput(
                class_type->info.class.fields,
                func_decl->value.func_decl.id.str,
                func_type
            );
        else
            hhg_fatal_error(
                "redeclaration of method \"%s\" in class \"%s\"",
                func_decl->value.func_decl.id.str,
                node->value.class_decl.id.str
            );

        // update func decl node with func type
        func_decl->value_type = func_type;

        hhg_sem_an_run(sem_an, func_decl->value.func_decl.body);

        hhg_sym_tab_exit_scope(sem_an->sym_tab);
    }
}

static void hhg_sem_an_add_class_func_params(
    hhg_sem_an_t *sem_an,
    hhg_node_t *node,
    hhg_type_t *func_type
)
{
    size_t params_len = arrlenu(node->value.func_decl.params);
    for (size_t j = 0; j < params_len; j++) {
        hhg_node_t *param = node->value.func_decl.params[j];

        arrput(func_type->info.func.params, param->value_type);

        hhg_sym_t *param_sym = hhg_sym_tab_lookup(
            sem_an->sym_tab,
            param->value.param.id.str
        );

        if (param_sym == NULL) {
            param->value.param.id.sym = hhg_sym_tab_insert(
                sem_an->sym_tab,
                (hhg_sym_t) {
                    .key = param->value.param.id.str,
                    .value = {
                        .sym_type = HHG_SYM_PARAM,
                        .type = param->value_type,
                    },
                }
            );
        } else
            hhg_fatal_error(
                "redeclaration of parameter \"%s\"",
                param->value.param.id.str
            );
    }
}

static void hhg_sem_an_run_func_call(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run_children(sem_an, node->value.func_call.args);
}

static void hhg_sem_an_run_return(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run(sem_an, node->value.ret.expr);
}

static void hhg_sem_an_run_arr_literal(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run_children(sem_an, node->value.arr_literal.elems);
}

static void hhg_sem_an_run_expr(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run(sem_an, node->value.expr.left);
    hhg_sem_an_run(sem_an, node->value.expr.right);
}

static void hhg_sem_an_run_inc_dec(hhg_sem_an_t *sem_an, hhg_node_t *node)
{
    hhg_sem_an_run(sem_an, node->value.expr.left);
}