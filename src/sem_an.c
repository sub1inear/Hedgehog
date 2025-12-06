#include <stdbool.h>
#include <string.h>

#include <stb_ds.h>

#include "sem_an.h"
#include "sym.h"
#include "sym_tab.h"
#include "node.h"
#include "error.h"
#include "type.h"
#include "type_ctx.h"

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
    case HHG_NODE_BLOCK: {
        hhg_sym_tab_enter_scope(sem_an->sym_tab);
        // recursively run sem an on block body
        size_t len = arrlenu(node->value.block.body);
        for (size_t i = 0; i < len; i++)
            hhg_sem_an_run(sem_an, node->value.block.body[i]);
        hhg_sym_tab_exit_scope(sem_an->sym_tab);
        break;
    }
    case HHG_TOKEN_ID: {
        // lookup symbol in symbol table
        hhg_sym_t *sym =
            hhg_sym_tab_lookup(sem_an->sym_tab, node->value.id.str);
        if (sym == NULL)
            hhg_fatal_error(
                "undefined variable \"%s\"",
                node->value.id.str
            );
        break;
    }
    case HHG_TOKEN_IF:
        hhg_sem_an_run(sem_an, node->value.if_stmt.cond);
        hhg_sem_an_run(sem_an, node->value.if_stmt.if_body);
        break;
    case HHG_TOKEN_WHILE:
        hhg_sem_an_run(sem_an, node->value.while_stmt.cond);
        hhg_sem_an_run(sem_an, node->value.while_stmt.body);
        break;
    case '=': {
        // run sem an on expr first to handle x = x where x is not defined yet
        hhg_sem_an_run(sem_an, node->value.var_decl.expr);

        // insert variable into symbol table if not done already
        // in Hedgehog variable declaration and definition are the same
        hhg_sym_t *sym =
            hhg_sym_tab_lookup(sem_an->sym_tab, node->value.var_decl.id.str);
        if (sym == NULL) {
            hhg_sym_t *new_sym = hhg_sym_tab_insert(sem_an->sym_tab, (hhg_sym_t) {
                .key = node->value.var_decl.id.str,
                .value = {
                    .sym_type = HHG_SYM_VAR,
                    .type = node->value_type,
                },
            });
            node->value.var_decl.id.sym = new_sym;
        }
        break;
    }
    case HHG_NODE_OBJ_INIT: {
        // run sem an on args
        size_t len = arrlenu(node->value.obj_init.args);
        for (size_t i = 0; i < len; i++)
            hhg_sem_an_run(sem_an, node->value.obj_init.args[i]);
        break;
    }
    case HHG_TOKEN_DEF: {
        // check for redeclaration
        hhg_sym_t *sym =
            hhg_sym_tab_lookup(sem_an->sym_tab, node->value.func_decl.id.str);
        if (sym == NULL) {
            hhg_type_t *func_type = hhg_type_new(HHG_TYPE_FUNC, sem_an->arena);
            func_type->info.func = (hhg_type_func_info_t) {
                .ret = node->value_type,
                .params = NULL,
            };

            node->value_type = func_type;

            // insert params into func type info and symbol table
            size_t len = arrlenu(node->value.func_decl.params);
            for (size_t i = 0; i < len; i++) {
                hhg_node_t *param = node->value.func_decl.params[i];

                arrput(
                    func_type->info.func.params,
                    param->value_type
                );

                hhg_sym_t *param_sym = hhg_sym_tab_lookup(
                    sem_an->sym_tab,
                    param->value.param.id.str
                );

                if (param_sym == NULL) {
                    hhg_sym_t *new_sym = hhg_sym_tab_insert(
                        sem_an->sym_tab,
                        (hhg_sym_t) {
                            .key = param->value.param.id.str,
                            .value = {
                                .sym_type = HHG_SYM_PARAM,
                                .type = param->value_type,
                            },
                        }
                    );
                    param->value.param.id.sym = new_sym;
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
            

            hhg_sym_t *new_sym = hhg_sym_tab_insert(sem_an->sym_tab, func_sym);

            // set back-reference to symbol in node info
            node->value.func_decl.id.sym = new_sym;
        } else
            hhg_fatal_error(
                "redeclaration of function \"%s\"",
                node->value.func_decl.id.str
            );

        hhg_sem_an_run(sem_an, node->value.func_decl.body);
        break;
    }
    case HHG_TOKEN_CLASS: {
        hhg_sym_tab_enter_scope(sem_an->sym_tab);

        hhg_sym_t *sym = hhg_sym_tab_lookup(sem_an->sym_tab, node->value.class_decl.id.str);
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

            hhg_sym_t self_sym = {
                .key = "self",
                .value = {
                    .sym_type = HHG_SYM_VAR,
                    .type = class_type,
                },
            };
            hhg_sym_tab_insert(sem_an->sym_tab, self_sym);

            size_t var_decls_len = arrlenu(node->value.class_decl.var_decls);
            for (size_t i = 0; i < var_decls_len; i++) {
                hhg_node_t *var_decl = node->value.class_decl.var_decls[i];
                // check for redeclaration of field in class
                for (size_t j = 0; j < var_decls_len; j++) {
                    if (i == j)
                        continue;
                    hhg_node_t *other_var_decl = node->value.class_decl.var_decls[j];
                    if (!strcmp(
                        var_decl->value.var_decl.id.str,
                        other_var_decl->value.var_decl.id.str)
                       ) {
                        hhg_fatal_error(
                            "redeclaration of field \"%s\" in class \"%s\"",
                            var_decl->value.var_decl.id.str,
                            node->value.class_decl.id.str
                        );
                    }
                }
                arrput(
                    class_type->info.class.fields,
                    var_decl->value_type
                );
            }

            size_t func_decls_len = arrlenu(node->value.class_decl.func_decls);
            for (size_t i = 0; i < func_decls_len; i++) {
                hhg_node_t *func_decl = node->value.class_decl.func_decls[i];

                hhg_type_t *func_type =
                    hhg_type_new(HHG_TYPE_FUNC, sem_an->arena);
                func_type->info.func = (hhg_type_func_info_t) {
                    .ret = func_decl->value_type,
                    .params = NULL,
                };

                // check for redeclaration of method in class
                for (size_t j = 0; j < func_decls_len; j++) {
                    if (i == j)
                        continue;
                    hhg_node_t *other_func_decl = node->value.class_decl.func_decls[j];
                    if (!strcmp(
                        func_decl->value.func_decl.id.str,
                        other_func_decl->value.func_decl.id.str)
                       ) {
                        hhg_fatal_error(
                            "redeclaration of method \"%s\" in class \"%s\"",
                            func_decl->value.func_decl.id.str,
                            node->value.class_decl.id.str
                        );
                    }
                }

                size_t params_len = arrlenu(func_decl->value.func_decl.params);
                for (size_t j = 0; j < params_len; j++) {
                    hhg_node_t *param = func_decl->value.func_decl.params[j];
                    
                    arrput(
                        func_type->info.func.params,
                        param->value_type
                    );
                    
                    hhg_sym_t *param_sym = hhg_sym_tab_lookup(
                        sem_an->sym_tab,
                        param->value.param.id.str
                    );

                    if (param_sym == NULL) {
                        hhg_sym_t *new_sym = hhg_sym_tab_insert(
                            sem_an->sym_tab,
                            (hhg_sym_t) {
                                .key = param->value.param.id.str,
                                .value = {
                                    .sym_type = HHG_SYM_PARAM,
                                    .type = param->value_type,
                                },
                            }
                        );
                        param->value.param.id.sym = new_sym;
                    } else
                        hhg_fatal_error(
                            "redeclaration of parameter \"%s\"",
                            param->value.param.id.str
                        );
                }
                
                arrput(
                    class_type->info.class.fields,
                    func_type
                );

                // update func decl node with func type
                func_decl->value_type = func_type;

                hhg_sem_an_run(
                    sem_an,
                    func_decl->value.func_decl.body
                );

            }

            hhg_sym_t *new_sym = hhg_sym_tab_insert(sem_an->sym_tab, class_sym);

            // update class node with class info
            node->value_type = class_type;
            node->value.class_decl.id.sym = new_sym;

        } else
            hhg_fatal_error(
                "redeclaration of class \"%s\"",
                node->value.class_decl.id.str
            );

        hhg_sym_tab_exit_scope(sem_an->sym_tab);
        break;
    }
    case '.':
        // ...
        break;
    case HHG_NODE_FUNC_CALL: {
        // ...
        size_t len = arrlenu(node->value.func_call.args);
        for (size_t i = 0; i < len; i++)
            hhg_sem_an_run(sem_an, node->value.func_call.args[i]);
        break;
    }
    case HHG_NODE_ARR_LITERAL: {
        size_t len = arrlenu(node->value.arr_literal.elems);
        for (size_t i = 0; i < len; i++)
            hhg_sem_an_run(sem_an, node->value.arr_literal.elems[i]);
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
    case HHG_TOKEN_AND:
    case HHG_TOKEN_OR:
        hhg_sem_an_run(sem_an, node->value.expr.left);
        hhg_sem_an_run(sem_an, node->value.expr.right);
        break;
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC:
        hhg_sem_an_run(sem_an, node->value.expr.left);
        break;
    case HHG_TOKEN_RETURN:
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