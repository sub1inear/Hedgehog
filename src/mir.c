#include <stdint.h>
#include <stdio.h>

#include <stb_ds.h>

#include "mir.h"
#include "mem.h"
#include "msg.h"
#include "sym.h"
#include "utils.h"

#define hhg_mir_gen_new_instr_value(gen, instr_type) \
    hhg_arena_malloc(                                \
        gen->arena,                                  \
        sizeof(instr_type)                           \
    )

static const char *mir_instr_op_to_str[] = {
    [HHG_MIR_ADD]          = "add",
    [HHG_MIR_SUB]          = "sub",
    [HHG_MIR_MUL]          = "mul",
    [HHG_MIR_DIV]          = "div",
    [HHG_MIR_MOD]          = "mod",
    [HHG_MIR_SHL]          = "shl",
    [HHG_MIR_LSHR]         = "lshr",
    [HHG_MIR_ASHR]         = "ashr",
    [HHG_MIR_AND]          = "and",
    [HHG_MIR_OR]           = "or",
    [HHG_MIR_XOR]          = "xor",
    [HHG_MIR_NOT]          = "not",
    [HHG_MIR_NEG]          = "neg",
    [HHG_MIR_CMP]          = "cmp",
    [HHG_MIR_JUMP]         = "jump",
    [HHG_MIR_BR]           = "br",
    [HHG_MIR_CALL]         = "call",
    [HHG_MIR_RET]          = "ret",
    [HHG_MIR_LOAD]         = "load",
    [HHG_MIR_STORE]        = "store",
    [HHG_MIR_ALLOCA]       = "alloca",
    [HHG_MIR_MALLOC]       = "malloc",
    [HHG_MIR_REALLOC]      = "realloc",
    [HHG_MIR_FREE]         = "free",
    [HHG_MIR_ARC]          = "arc",
    [HHG_MIR_MOVE]         = "move",
    [HHG_MIR_COPY]         = "copy",
    [HHG_MIR_BORROW]       = "borrow",
    [HHG_MIR_REBORROW]     = "reborrow",
    [HHG_MIR_RELEASE]      = "release",
    [HHG_MIR_CAST]         = "cast",
    [HHG_MIR_STRUCT_INIT]  = "struct_init",
    [HHG_MIR_STRUCT_LOAD]  = "struct_load",
    [HHG_MIR_STRUCT_STORE] = "struct_store",
    [HHG_MIR_ARR_INIT]     = "arr_init",
    [HHG_MIR_ARR_LOAD]     = "arr_load",
    [HHG_MIR_ARR_STORE]    = "arr_store",
};

static hhg_mir_reg_t hhg_mir_gen_run_core(
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
static hhg_type_t *hhg_mir_gen_get_tmp(hhg_mir_gen_t *gen, hhg_mir_reg_t tmp);

static void hhg_mir_gen_push_ctx(hhg_mir_gen_t *gen, hhg_mir_ctx_t ctx);
static void hhg_mir_gen_pop_ctx(hhg_mir_gen_t *gen);
static hhg_mir_ctx_t *hhg_mir_gen_get_cur_ctx(hhg_mir_gen_t *gen);

static void hhg_mir_gen_run_children(
    hhg_mir_gen_t *gen,
    hhg_node_t **children
);
static hhg_mir_reg_t hhg_mir_gen_run_block(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_id(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_if(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_while(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_var_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_obj_init(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_func_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_class_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_func_call(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_return(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_arr_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_expr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_inc_dec(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_bool_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_string_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_int_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_reg_t hhg_mir_gen_run_float_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);

static void hhg_mir_gen_func_print(hhg_mir_func_t *func);

static void hhg_mir_opnd_print(hhg_mir_opnd_t *opnd);
static void hhg_mir_instr_print(hhg_mir_instr_t *instr);

static void hhg_mir_func_free(hhg_mir_func_t *func);

static void hhg_mir_instr_free(hhg_mir_instr_t *instr);

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
    assert(prog->type == HHG_NODE_BLOCK);
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
        (hhg_mir_ctx_t) {
            .in_global_scope = true,
            .func = &arrlast(gen->funcs),
        }
    );

    hhg_mir_gen_run_block(gen, prog);
    
    hhg_mir_gen_pop_ctx(gen);
}

void hhg_mir_gen_print(hhg_mir_gen_t *gen)
{
    size_t size = arrlenu(gen->funcs);
    for (size_t i = 0; i < size; i++)
        hhg_mir_gen_func_print(&gen->funcs[i]);
}

void hhg_mir_gen_del(hhg_mir_gen_t *gen)
{
    size_t size = arrlenu(gen->funcs);
    for (size_t i = 0; i < size; i++)
        hhg_mir_func_free(&gen->funcs[i]);

    arrfree(gen->funcs);
    arrfree(gen->ctx);
    arrfree(gen->global_arr);
    arrfree(gen->tmp_arr);
}

static hhg_mir_reg_t hhg_mir_gen_run_core(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    switch (node->type) {
    case HHG_NODE_BLOCK: {
        hhg_mir_ctx_t new_ctx = *hhg_mir_gen_get_cur_ctx(gen);
        new_ctx.in_global_scope = false;
        hhg_mir_gen_push_ctx(gen, new_ctx);

        hhg_mir_reg_t reg = hhg_mir_gen_run_block(gen, node);
        
        hhg_mir_gen_pop_ctx(gen);
        
        return reg;
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
    case HHG_NODE_ARR_LITERAL:
        return hhg_mir_gen_run_arr_literal(gen, node);
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
        return hhg_mir_gen_run_expr(gen, node);
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC:
        return hhg_mir_gen_run_inc_dec(gen, node);
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        return hhg_mir_gen_run_bool_literal(gen, node);
    case HHG_TOKEN_STRING_LITERAL:
        return hhg_mir_gen_run_string_literal(gen, node);
    case HHG_TOKEN_INT_LITERAL:
        return hhg_mir_gen_run_int_literal(gen, node);
    case HHG_TOKEN_FLOAT_LITERAL:
        return hhg_mir_gen_run_float_literal(gen, node);
    case '.':
    case HHG_NODE_PARAM:
        return -1;
    default:
        hhg_fatal_error(
            "unhandled node type `%n` in hhg_mir_gen_run_core",
            node->type
        );
        return -1;
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
    hhg_mir_ctx_t *ctx = hhg_mir_gen_get_cur_ctx(gen);
    // local should not already exist
    assert(hmgetp_null(ctx->local_tab, sym) == NULL);
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

    hhg_mir_load_t *load_value = hhg_mir_gen_new_instr_value(gen, hhg_mir_load_t);
    load_value->dest = tmp_reg;
    load_value->src = (hhg_mir_opnd_t){
        .type = HHG_MIR_OPND_SYM,
        .value.sym = sym,
    };
    hhg_mir_gen_add_instr(gen, node, HHG_MIR_LOAD, load_value);

    return tmp_reg;
}

static hhg_mir_reg_t hhg_mir_gen_get_local(hhg_mir_gen_t *gen, hhg_sym_t *sym)
{
    hhg_mir_ctx_t *ctx = &arrlast(gen->ctx);
    hhg_mir_local_t *entry = hmgetp_null(ctx->local_tab, sym);
    return entry == NULL ? -1 : entry->value;
}

static hhg_type_t *hhg_mir_gen_get_tmp(hhg_mir_gen_t *gen, hhg_mir_reg_t tmp)
{
    assert(tmp > 0);
    return gen->tmp_arr[tmp];
}

static void hhg_mir_gen_push_ctx(hhg_mir_gen_t *gen, hhg_mir_ctx_t ctx)
{
    arrput(gen->ctx, ctx);
}

static void hhg_mir_gen_pop_ctx(hhg_mir_gen_t *gen)
{
    (void)arrpop(gen->ctx);
}

static hhg_mir_ctx_t *hhg_mir_gen_get_cur_ctx(hhg_mir_gen_t *gen)
{
    return &arrlast(gen->ctx);
}

static void hhg_mir_gen_run_children(
    hhg_mir_gen_t *gen,
    hhg_node_t **children
)
{
    size_t size = arrlenu(children);
    for (size_t i = 0; i < size; i++)
        hhg_mir_gen_run_core(gen, children[i]);
}

static hhg_mir_reg_t hhg_mir_gen_run_block(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    hhg_mir_gen_run_children(gen, node->value.block.body);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_id(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    if (hhg_mir_gen_get_cur_ctx(gen)->in_global_scope)
        return hhg_mir_gen_get_global(gen, node->value.id.sym, node);
    hhg_mir_reg_t reg = hhg_mir_gen_get_local(gen, node->value.id.sym);
    assert(reg != -1);
    return reg;
}

static hhg_mir_reg_t hhg_mir_gen_run_if(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_while(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_var_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    hhg_mir_reg_t expr_reg = hhg_mir_gen_run_core(gen, node->value.var_decl.expr);

    hhg_mir_ctx_t *ctx = hhg_mir_gen_get_cur_ctx(gen);

    // declare variable if first
    if (node->value.var_decl.first) {
        if (ctx->in_global_scope)
            hhg_mir_gen_new_global(gen, node->value.var_decl.id.sym);
        else
            hhg_mir_gen_new_local(gen, node->value.var_decl.id.sym);
    }

    if (ctx->in_global_scope) {
        hhg_mir_store_t *store_value =
            hhg_mir_gen_new_instr_value(gen, hhg_mir_store_t);
        store_value->dest = node->value.var_decl.id.sym;
        store_value->src = (hhg_mir_opnd_t) {
            .type = HHG_MIR_OPND_REG,
            .value.reg = expr_reg,
        };
        hhg_mir_gen_add_instr(gen, node, HHG_MIR_STORE, store_value);
    } else {
        hhg_mir_reg_t local_reg =
            hhg_mir_gen_get_local(gen, node->value.var_decl.id.sym);
        hhg_mir_move_t *move_value =
            hhg_mir_gen_new_instr_value(gen, hhg_mir_move_t);
        move_value->dest = local_reg;
        move_value->src = expr_reg;
        hhg_mir_gen_add_instr(gen, node, HHG_MIR_MOVE, move_value);
    }

    return expr_reg;
}

static hhg_mir_reg_t hhg_mir_gen_run_obj_init(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_func_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_class_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_func_call(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_return(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_arr_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_expr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_inc_dec(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_bool_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_string_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_int_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static hhg_mir_reg_t hhg_mir_gen_run_float_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return -1;
}

static void hhg_mir_gen_func_print(hhg_mir_func_t *func)
{
    printf("def %s\n", func->sym->key);
    
    size_t size = arrlenu(func->instrs);

    for (size_t i = 0; i < size; i++) {
        fputs("  ", stdout);
        hhg_mir_instr_print(&func->instrs[i]);
    }
}

static void hhg_mir_opnd_print(hhg_mir_opnd_t *opnd)
{
    switch (opnd->type) {
    case HHG_MIR_OPND_REG:
        printf("%%%" HHG_PRIreg, opnd->value.reg);
        break;
    case HHG_MIR_OPND_CNST:
        switch (opnd->value.cnst.type) {
        case HHG_MIR_CNST_SI:
            printf("%" PRId64, opnd->value.cnst.value.si);
            break;
        case HHG_MIR_CNST_UI:
            printf("%" PRIu64, opnd->value.cnst.value.ui);
            break;
        case HHG_MIR_CNST_F32:
            printf("%f", opnd->value.cnst.value.f);
            break;
        case HHG_MIR_CNST_F64:
            printf("%f", opnd->value.cnst.value.d);
            break;
        case HHG_MIR_CNST_BOOL:
            printf("%s", opnd->value.cnst.value.b ? "true" : "false");
            break;
        case HHG_MIR_CNST_CHAR:
            printf("'%c'", opnd->value.cnst.value.c);
            break;
        case HHG_MIR_CNST_STR:
            printf("\"%s\"", opnd->value.cnst.value.str);
            break;
        }
        break;
    case HHG_MIR_OPND_SYM:
        printf("$%s", opnd->value.sym->key);
        break;
    default:
        break;
    }
}

static void hhg_mir_instr_print(hhg_mir_instr_t *instr)
{
    switch (instr->op) {
    case HHG_MIR_ADD:
    case HHG_MIR_SUB:
    case HHG_MIR_MUL:
    case HHG_MIR_DIV:
    case HHG_MIR_MOD:
    case HHG_MIR_SHL:
    case HHG_MIR_LSHR:
    case HHG_MIR_ASHR:
    case HHG_MIR_AND:
    case HHG_MIR_OR:
    case HHG_MIR_XOR:
    case HHG_MIR_NOT:
    case HHG_MIR_NEG: {
        hhg_mir_expr_t *expr = &instr->value->expr;
        printf("%%%" HHG_PRIreg " = ", expr->dest);
        break;
    }
    case HHG_MIR_LOAD: {
        hhg_mir_load_t *load = &instr->value->load;
        printf("%%%" HHG_PRIreg " = ", load->dest);
        break;
    }
    case HHG_MIR_STORE: {
        hhg_mir_store_t *store = &instr->value->store;
        printf("$%s = ", store->dest->key);
        break;
    }
    default:
        break;
    }

    printf("%s ", mir_instr_op_to_str[instr->op]);
    
    switch (instr->op) {
    case HHG_MIR_ADD:
    case HHG_MIR_SUB:
    case HHG_MIR_MUL:
    case HHG_MIR_DIV:
    case HHG_MIR_MOD:
    case HHG_MIR_SHL:
    case HHG_MIR_LSHR:
    case HHG_MIR_ASHR:
    case HHG_MIR_AND:
    case HHG_MIR_OR:
    case HHG_MIR_XOR:
    case HHG_MIR_NOT:
    case HHG_MIR_NEG: {
        hhg_mir_expr_t *expr = &instr->value->expr;
        hhg_mir_opnd_print(&expr->left);
        fputs(", ", stdout);
        hhg_mir_opnd_print(&expr->right);
        break;
    }
    case HHG_MIR_LOAD: {
        hhg_mir_load_t *load = &instr->value->load;
        hhg_mir_opnd_print(&load->src);
        break;
    }
    case HHG_MIR_STORE: {
        hhg_mir_store_t *store = &instr->value->store;
        hhg_mir_opnd_print(&store->src);
        break;
    }
    default:
        break;
    }
    putchar('\n');
}

static void hhg_mir_func_free(hhg_mir_func_t *func)
{
    size_t size = arrlenu(func->instrs);
    for (size_t i = 0; i < size; i++)
        hhg_mir_instr_free(&func->instrs[i]);
}

static void hhg_mir_instr_free(hhg_mir_instr_t *instr)
{
    switch (instr->op) {
    case HHG_MIR_CALL:
        arrfree(instr->value->call.args);
        break;
    case HHG_MIR_STRUCT_INIT:
        arrfree(instr->value->struct_init.fields);
        break;
    case HHG_MIR_ARR_INIT:
        arrfree(instr->value->arr_init.elems);
        break;
    }
}