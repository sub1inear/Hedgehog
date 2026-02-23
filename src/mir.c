#include <stdio.h>

#include <stb_ds.h>

#include "mir.h"
#include "msg.h"

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
    switch (opnd->type) {
    case HHG_MIR_OPND_NONE:
        fputs("none", stdout);
        break;
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
    default:
        hhg_compiler_error("invalid operand type: %i", (int)opnd->type);
        break;
    }
}

void hhg_mir_instr_print(hhg_mir_instr_t *instr)
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
        printf("$%s", load->src->key);
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

void hhg_mir_func_print(hhg_mir_func_t *func)
{
    printf("def %s\n", func->sym->key);
    
    size_t size = arrlenu(func->instrs);

    for (size_t i = 0; i < size; i++) {
        fputs("  ", stdout);
        hhg_mir_instr_print(&func->instrs[i]);
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
    size_t size = arrlenu(func->instrs);
    for (size_t i = 0; i < size; i++)
        hhg_mir_instr_free(&func->instrs[i]);
}
