#include <stdio.h>

#include <stb_ds.h>

#include "mir.h"
#include "mem.h"
#include "msg.h"
#include "utils.h"

static hhg_mir_instr_t *hhg_mir_gen_run_block(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_id(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_if(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_while(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_var_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_obj_init(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_func_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_class_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_func_call(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_return(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_arr_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_expr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_inc_dec(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_bool_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_string_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_int_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_float_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);
static hhg_mir_instr_t *hhg_mir_gen_run_core(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
);

void hhg_mir_gen_init(hhg_mir_gen_t *gen, hhg_arena_t *arena)
{
    *gen = (hhg_mir_gen_t) {
        .ctx = NULL,
        .arena = arena,
    };
}

hhg_mir_instr_t *hhg_mir_instr_new(hhg_mir_gen_t *gen, hhg_mir_op_t op)
{
    hhg_mir_instr_t *instr = hhg_arena_malloc(
        gen->arena,
        sizeof(hhg_mir_instr_t)
    );
    *instr = (hhg_mir_instr_t) {
        .op = op,
        .opnds = NULL,
        .src = NULL,
        .range = NULL,
    };
    return instr;
}
hhg_mir_instr_t *hhg_mir_instr_new_opnd(
    hhg_mir_gen_t *gen,
    hhg_mir_op_t op,
    hhg_mir_opnd_t opnd
)
{
    hhg_mir_instr_t *instr = hhg_mir_instr_new(gen, op);
    arrpush(instr->opnds, opnd);
    return instr;
}


hhg_mir_instr_t *hhg_mir_gen_run(hhg_mir_gen_t *gen, hhg_node_t *prog)
{
    assert(prog->type == HHG_NODE_BLOCK);
    return hhg_mir_gen_run_block(gen, prog);
}

void hhg_mir_gen_print(hhg_mir_gen_t *gen)
{
    HHG_UNUSED(gen);
}

void hhg_mir_gen_del(hhg_mir_gen_t *gen)
{
    arrfree(gen->ctx);
}

void hhg_mir_instr_free(hhg_mir_instr_t *instr)
{
    arrfree(instr->opnds);
}

static hhg_mir_instr_t *hhg_mir_gen_run_core(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    switch (node->type) {
    case HHG_NODE_BLOCK:
        return hhg_mir_gen_run_block(gen, node);
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
        return NULL;
    default:
        hhg_fatal_error(
            "unhandled node type `%n` in hhg_mir_gen_run_core",
            node->type
        );
        return NULL;
    }
}


static hhg_mir_instr_t *hhg_mir_gen_run_block(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_id(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_if(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_while(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_var_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_obj_init(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_func_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_class_decl(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_func_call(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_return(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_arr_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_expr(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_inc_dec(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_bool_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_string_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_int_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}

static hhg_mir_instr_t *hhg_mir_gen_run_float_literal(
    hhg_mir_gen_t *gen,
    hhg_node_t *node
)
{
    HHG_UNUSED(gen, node);
    return NULL;
}
