#if 0
#include <inttypes.h>
#include <stdio.h>

#include <stb_ds.h>

#include "mir.h"
#include "msg.h"
#include "utils.h"

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


void hhg_mir_opnd_print(hhg_mir_opnd_t *opnd)
{
    hhg_mir_opnd_fprint(opnd, stdout);
}

void hhg_mir_opnd_fprint(hhg_mir_opnd_t *opnd, FILE *stream)
{
    switch (opnd->type) {
    case HHG_MIR_OPND_NONE:
        fputs("none", stream);
        break;
    case HHG_MIR_OPND_REG:
        fprintf(stream, "%%%" HHG_PRIreg, opnd->value.reg);
        break;
    case HHG_MIR_OPND_CNST:
        switch (opnd->value.cnst.type) {
        case HHG_MIR_CNST_SI:
            fprintf(stream, "%" PRIi64, opnd->value.cnst.value.si);
            break;
        case HHG_MIR_CNST_UI:
            fprintf(stream, "%" PRIu64, opnd->value.cnst.value.ui);
            break;
        case HHG_MIR_CNST_F32:
            fprintf(stream, "%f", opnd->value.cnst.value.f);
            break;
        case HHG_MIR_CNST_F64:
            fprintf(stream, "%f", opnd->value.cnst.value.d);
            break;
        case HHG_MIR_CNST_BOOL:
            fputs(opnd->value.cnst.value.b ? "true" : "false", stream);
            break;
        case HHG_MIR_CNST_CHAR:
            fprintf(stream, "'%c'", opnd->value.cnst.value.c);
            break;
        case HHG_MIR_CNST_STR:
            fprintf(stream, "\"%s\"", opnd->value.cnst.value.str);
            break;
        }
        break;
    default:
        hhg_compiler_error("invalid operand type: %i", opnd->type);
        break;
    }
}


void hhg_mir_instr_print(hhg_mir_instr_t *instr)
{
    hhg_mir_instr_fprint(instr, stdout);
}

void hhg_mir_instr_fprint(hhg_mir_instr_t *instr, FILE *stream)
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
        fprintf(stream, "%%%" HHG_PRIreg " = ", expr->dst);
        break;
    }
    case HHG_MIR_LOAD: {
        hhg_mir_load_t *load = &instr->value->load;
        fprintf(stream, "%%%" HHG_PRIreg " = ", load->dst);
        break;
    }
    case HHG_MIR_STORE: {
        hhg_mir_store_t *store = &instr->value->store;
        fprintf(stream, "$%s = ", store->dst->key);
        break;
    }
    default:
        break;
    }

    fprintf(stream, "%s ", mir_instr_op_to_str[instr->op]);
    
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
        hhg_mir_opnd_fprint(&expr->left, stream);
        fputs(", ", stream);
        hhg_mir_opnd_fprint(&expr->right, stream);
        break;
    }
    case HHG_MIR_LOAD: {
        hhg_mir_load_t *load = &instr->value->load;
        fprintf(stream, "$%s", load->src->key);
        break;
    }
    case HHG_MIR_STORE: {
        hhg_mir_store_t *store = &instr->value->store;
        hhg_mir_opnd_fprint(&store->src, stream);
        break;
    }
    default:
        break;
    }
    fputc('\n', stream);
}

void hhg_mir_func_print(hhg_mir_func_t *func)
{
    hhg_mir_func_fprint(func, stdout);
}

void hhg_mir_func_fprint(hhg_mir_func_t *func, FILE *stream)
{
    fprintf(stream, "fn %s\n", func->sym->key);
    
    size_t len = arrlenu(func->instrs);
    for (size_t i = 0; i < len; i++) {
        fputs("  ", stream);
        hhg_mir_instr_fprint(&func->instrs[i], stream);
    }
}



void hhg_mir_instr_free(hhg_mir_instr_t *instr)
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

void hhg_mir_func_free(hhg_mir_func_t *func)
{
    size_t len = arrlenu(func->instrs);
    for (size_t i = 0; i < len; i++)
        hhg_mir_instr_free(&func->instrs[i]);
}
#endif