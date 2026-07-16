#if 0 // lexer-only: disabled while token set is in flux
#include <stb_ds.h>

#include "mir_gen.h"
#include "mir.h"
#include "mem.h"
#include "msg.h"
#include "sym.h"
#include "type.h"
#include "utils.h"

#define hhg_mir_gen_new_instr(gen, instr_type) \
    hhg_arena_malloc(                          \
        gen->arena,                            \
        sizeof(instr_type)                     \
    )

static hhg_mir_opnd_t hhg_mir_gen_run_core(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static void hhg_mir_gen_add_instr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node,
    hhg_mir_op_t op,
    void *value
);
static void hhg_mir_gen_new_global(hhg_mir_gen_t *gen, hhg_sym_t *sym);
static void hhg_mir_gen_new_local(hhg_mir_gen_t *gen, hhg_sym_t *sym);
static hhg_mir_reg_t hhg_mir_gen_new_tmp(hhg_mir_gen_t *gen, hhg_type_t *type);
static hhg_mir_lbl_t hhg_mir_gen_new_lbl(hhg_mir_gen_t *gen);

static hhg_mir_reg_t hhg_mir_gen_get_global(hhg_mir_gen_t *gen, hhg_sym_t *sym, hhg_node_t *node);
static hhg_mir_lbl_t hhg_mir_gen_get_local(hhg_mir_gen_t *gen, hhg_sym_t *sym);
static hhg_type_t *hhg_mir_gen_get_tmp_type(
    hhg_mir_gen_t *gen,
    hhg_mir_reg_t tmp
);

static void hhg_mir_gen_push_ctx(hhg_mir_gen_t *gen, hhg_mir_gen_ctx_t ctx);
static void hhg_mir_gen_pop_ctx(hhg_mir_gen_t *gen);
static hhg_mir_gen_ctx_t *hhg_mir_gen_get_cur_ctx(hhg_mir_gen_t *gen);

static void hhg_mir_gen_run_children(
    hhg_mir_gen_t *gen,
    hhg_node_t **children
);
static hhg_mir_opnd_t hhg_mir_gen_run_block(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_id(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_if(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_while(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_var_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_obj_init(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_func_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_class_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_func_call(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_return(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_arr_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_expr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_inc_dec(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_bool_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_str_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_int_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_opnd_t hhg_mir_gen_run_float_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);

void hhg_mir_gen_init(hhg_mir_gen_t *gen, hhg_arena_t *arena)
{
    *gen = (hhg_mir_gen_t) {
        .funcs = NULL,
        .ctx = NULL,
        .global_arr = NULL,
        .tmp_arr = NULL,
        .reg_count = 0,
        .lbl_count = 0,
        .arena = arena,
    };
}

void hhg_mir_gen_run(hhg_mir_gen_t *gen, hhg_node_t *prog)
{
    hhg_assert(prog->type == HHG_NODE_BLOCK);
    hhg_mir_func_t main_func = (hhg_mir_func_t) {
        .sym = hhg_sym_new(
            gen->arena,
            "main",
            (hhg_sym_value_t) {
                .sym_type = HHG_SYM_FUNC,
                .type = NULL,
            }
        ),
        .instrs = NULL,
    };
    
    arrput(gen->funcs, main_func);
    
    hhg_mir_gen_push_ctx(
        gen,
        (hhg_mir_gen_ctx_t) {
            .in_global_scope = true,
            .func = &arrlast(gen->funcs),
        }
    );

    hhg_mir_gen_run_block(gen, prog);
    
    hhg_mir_gen_pop_ctx(gen);
}

void hhg_mir_gen_print(hhg_mir_gen_t *gen)
{
    size_t len = arrlenu(gen->funcs);
    for (size_t i = 0; i < len; i++)
        hhg_mir_func_print(&gen->funcs[i]);
}

void hhg_mir_gen_del(hhg_mir_gen_t *gen)
{
    size_t len = arrlenu(gen->funcs);
    for (size_t i = 0; i < len; i++)
        hhg_mir_func_free(&gen->funcs[i]);

    arrfree(gen->funcs);
    arrfree(gen->ctx);
    arrfree(gen->global_arr);
    arrfree(gen->tmp_arr);
}

static hhg_mir_opnd_t hhg_mir_gen_run_core(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    switch (node->type) {
    case HHG_NODE_BLOCK: {
        hhg_mir_gen_ctx_t new_ctx = *hhg_mir_gen_get_cur_ctx(gen);
        new_ctx.in_global_scope = false;
        hhg_mir_gen_push_ctx(gen, new_ctx);

        hhg_mir_opnd_t opnd = hhg_mir_gen_run_block(gen, node);
        
        hhg_mir_gen_pop_ctx(gen);
        
        return opnd;
    }
    case HHG_TOKEN_ID:
        return hhg_mir_gen_run_id(gen, node);
    case HHG_TOKEN_IF:
        return hhg_mir_gen_run_if(gen, node);
    case HHG_TOKEN_WHILE:
        return hhg_mir_gen_run_while(gen, node);
    case '=':
        return hhg_mir_gen_run_var_decl(gen, node);
    case HHG_NODE_OBJ_INIT:
        return hhg_mir_gen_run_obj_init(gen, node);
    case HHG_TOKEN_DEF:
        return hhg_mir_gen_run_func_decl(gen, node);
    case HHG_TOKEN_CLASS:
        return hhg_mir_gen_run_class_decl(gen, node);
    case HHG_NODE_FUNC_CALL:
        return hhg_mir_gen_run_func_call(gen, node);
    case HHG_TOKEN_RETURN:
        return hhg_mir_gen_run_return(gen, node);
    case HHG_NODE_ARR_LIT:
        return hhg_mir_gen_run_arr_lit(gen, node);
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
    case HHG_TOKEN_AND:
    case HHG_TOKEN_OR:
        return hhg_mir_gen_run_expr(gen, node);
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC:
        return hhg_mir_gen_run_inc_dec(gen, node);
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        return hhg_mir_gen_run_bool_lit(gen, node);
    case HHG_TOKEN_STR_LIT:
        return hhg_mir_gen_run_str_lit(gen, node);
    case HHG_TOKEN_INT_LIT:
        return hhg_mir_gen_run_int_lit(gen, node);
    case HHG_TOKEN_FLOAT_LIT:
        return hhg_mir_gen_run_float_lit(gen, node);
    case '.':
    case HHG_NODE_PARAM:
        return (hhg_mir_opnd_t){ .type = HHG_MIR_OPND_NONE };
    default:
        hhg_compiler_error(
            "unhandled node type `%n` in hhg_mir_gen_run_core",
            node->type
        );
        return (hhg_mir_opnd_t){ .type = HHG_MIR_OPND_NONE };
    }
}

static void hhg_mir_gen_add_instr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node,
    hhg_mir_op_t op,
    void *value
)
{
    hhg_mir_instr_t instr = {
        .op = op,
        .value = value,
        .node = node,
    };
    
    arrput(hhg_mir_gen_get_cur_ctx(gen)->func->instrs, instr);
}

static void hhg_mir_gen_new_global(hhg_mir_gen_t *gen, hhg_sym_t *sym)
{
    arrput(gen->global_arr, sym);
}

static void hhg_mir_gen_new_local(hhg_mir_gen_t *gen, hhg_sym_t *sym)
{
    hhg_mir_gen_ctx_t *ctx = hhg_mir_gen_get_cur_ctx(gen);
    // local should not already exist
    hhg_assert(hmgetp_null(ctx->local_tab, sym) == NULL);
    hmput(ctx->local_tab, sym, gen->reg_count);
    gen->reg_count++;
}

static hhg_mir_reg_t hhg_mir_gen_new_tmp(hhg_mir_gen_t *gen, hhg_type_t *type)
{
    arrput(gen->tmp_arr, type);
    return gen->reg_count++;
}

static hhg_mir_lbl_t hhg_mir_gen_new_lbl(hhg_mir_gen_t *gen)
{
    return gen->lbl_count++;
}

static hhg_mir_reg_t hhg_mir_gen_get_global(hhg_mir_gen_t *gen, hhg_sym_t *sym, hhg_node_t *node)
{
    hhg_mir_reg_t tmp_reg = hhg_mir_gen_new_tmp(gen, sym->value.type);

    hhg_mir_load_t *load_value = hhg_mir_gen_new_instr(gen, hhg_mir_load_t);
    load_value->dst = tmp_reg;
    load_value->src = sym;
    hhg_mir_gen_add_instr(gen, node, HHG_MIR_LOAD, load_value);

    return tmp_reg;
}

static hhg_mir_reg_t hhg_mir_gen_get_local(hhg_mir_gen_t *gen, hhg_sym_t *sym)
{
    hhg_mir_gen_ctx_t *ctx = &arrlast(gen->ctx);
    hhg_mir_local_t *entry = hmgetp_null(ctx->local_tab, sym);
    return entry == NULL ? -1 : entry->value;
}

static hhg_type_t *hhg_mir_gen_get_tmp_type(
    hhg_mir_gen_t *gen,
    hhg_mir_reg_t tmp
)
{
    return gen->tmp_arr[tmp];
}

static void hhg_mir_gen_push_ctx(hhg_mir_gen_t *gen, hhg_mir_gen_ctx_t ctx)
{
    arrput(gen->ctx, ctx);
}

static void hhg_mir_gen_pop_ctx(hhg_mir_gen_t *gen)
{
    HHG_UNUSED(arrpop(gen->ctx));
}

static hhg_mir_gen_ctx_t *hhg_mir_gen_get_cur_ctx(hhg_mir_gen_t *gen)
{
    return &arrlast(gen->ctx);
}

static void hhg_mir_gen_run_children(
    hhg_mir_gen_t *gen,
    hhg_node_t **children
)
{
    size_t len = arrlenu(children);
    for (size_t i = 0; i < len; i++)
        hhg_mir_gen_run_core(gen, children[i]);
}

static hhg_mir_opnd_t hhg_mir_gen_run_block(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    hhg_mir_gen_run_children(gen, node->value.block.body);
    return (hhg_mir_opnd_t){ .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_id(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    if (hhg_mir_gen_get_cur_ctx(gen)->in_global_scope)
        return (hhg_mir_opnd_t) {
            .type = HHG_MIR_OPND_REG,
            .value.reg = hhg_mir_gen_get_global(gen, node->value.id.sym, node)
        };
    hhg_mir_reg_t reg = hhg_mir_gen_get_local(gen, node->value.id.sym);
    hhg_assert(reg != -1);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_REG, .value.reg = reg, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_if(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_while(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_var_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    hhg_mir_gen_ctx_t *ctx = hhg_mir_gen_get_cur_ctx(gen);

    // declare variable if first
    if (node->value.var_decl.first) {
        if (ctx->in_global_scope)
            hhg_mir_gen_new_global(gen, node->value.var_decl.id.sym);
        else
            hhg_mir_gen_new_local(gen, node->value.var_decl.id.sym);
    }

    hhg_mir_opnd_t expr_opnd = hhg_mir_gen_run_core(gen, node->value.var_decl.expr);

    if (ctx->in_global_scope) {
        hhg_mir_store_t *store_value =
            hhg_mir_gen_new_instr(gen, hhg_mir_store_t);
        store_value->dst = node->value.var_decl.id.sym;
        store_value->src = expr_opnd;
        hhg_mir_gen_add_instr(gen, node, HHG_MIR_STORE, store_value);
    } else {
        hhg_mir_reg_t local_reg =
            hhg_mir_gen_get_local(gen, node->value.var_decl.id.sym);
        hhg_mir_copy_t *copy_value =
            hhg_mir_gen_new_instr(gen, hhg_mir_copy_t);
        copy_value->dst = local_reg;
        copy_value->src = expr_opnd;
        hhg_mir_gen_add_instr(gen, node, HHG_MIR_COPY, copy_value);
    }

    return expr_opnd;
}

static hhg_mir_opnd_t hhg_mir_gen_run_obj_init(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_func_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_class_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_func_call(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_return(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_arr_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_expr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_inc_dec(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_bool_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_str_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}

static hhg_mir_opnd_t hhg_mir_gen_run_int_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
   
    return (hhg_mir_opnd_t) {
        .type = HHG_MIR_OPND_CNST,
        .value.cnst = (hhg_mir_cnst_t) {
            .type = HHG_MIR_CNST_SI,
            .value.si = 
            hhg_str_to_int64(node->value.lit.str),
        },
    };
}

static hhg_mir_opnd_t hhg_mir_gen_run_float_lit(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return (hhg_mir_opnd_t) { .type = HHG_MIR_OPND_NONE, };
}
#endif // lexer-only
