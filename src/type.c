#include <stdbool.h>
#include <stdio.h>

#include <stb_ds.h>

#include "type.h"
#include "token.h"
#include "mem.h"
#include "sym.h"
#include "utils.h"

static const char *const base_type_to_str[] = {
    [HHG_TYPE_NONE] = "none",

    [HHG_TYPE_I8] = "i8",
    [HHG_TYPE_U8] = "u8",

    [HHG_TYPE_I16] = "i16",
    [HHG_TYPE_U16] = "u16",

    [HHG_TYPE_I32] = "i32",
    [HHG_TYPE_U32] = "u32",

    [HHG_TYPE_I64] = "i64",
    [HHG_TYPE_U64] = "u64",

    [HHG_TYPE_INT] = "int",

    [HHG_TYPE_F32] = "f32",
    [HHG_TYPE_F64] = "f64",

    [HHG_TYPE_FLOAT] = "float",

    [HHG_TYPE_BOOL] = "bool",
    [HHG_TYPE_CHAR] = "char",

    [HHG_TYPE_ISIZE] = "isize",
    [HHG_TYPE_USIZE] = "usize",

    [HHG_TYPE_REF] = "ref",
    [HHG_TYPE_ARR] = "arr",

    [HHG_TYPE_FUNC] = "func",
    [HHG_TYPE_CLASS] = "class",
    [HHG_TYPE_ENUM] = "enum",

    [HHG_TYPE_ID] = "id",
};

void hhg_type_init(hhg_type_t *type, hhg_base_type_t base)
{
    *type = (hhg_type_t) {
        .type = base,
        .is_const = false,
        .is_volatile = false,
    };
}

hhg_type_t *hhg_type_new(hhg_base_type_t base, hhg_arena_t *arena)
{
    hhg_type_t *type = hhg_arena_malloc(arena, sizeof(hhg_type_t));
    hhg_type_init(type, base);
    return type;
}

hhg_base_type_t hhg_token_type_to_base_type(hhg_token_type_t token_type)
{
    switch (token_type) {
    case HHG_TOKEN_I8:
        return HHG_TYPE_I8;
    case HHG_TOKEN_U8:
        return HHG_TYPE_U8;
    case HHG_TOKEN_I16:
        return HHG_TYPE_I16;
    case HHG_TOKEN_U16:
        return HHG_TYPE_U16;
    case HHG_TOKEN_I32:
        return HHG_TYPE_I32;
    case HHG_TOKEN_U32:
        return HHG_TYPE_U32;
    case HHG_TOKEN_I64:
        return HHG_TYPE_I64;
    case HHG_TOKEN_U64:
        return HHG_TYPE_U64;
    case HHG_TOKEN_INT:
        return HHG_TYPE_INT;
    case HHG_TOKEN_F32:
        return HHG_TYPE_F32;
    case HHG_TOKEN_F64:
        return HHG_TYPE_F64;
    case HHG_TOKEN_FLOAT:
        return HHG_TYPE_FLOAT;
    case HHG_TOKEN_BOOL:
        return HHG_TYPE_BOOL;
    case HHG_TOKEN_CHAR:
        return HHG_TYPE_CHAR;
    case HHG_TOKEN_ISIZE:
        return HHG_TYPE_ISIZE;
    case HHG_TOKEN_USIZE:
        return HHG_TYPE_USIZE;
    default:
        return HHG_TYPE_NONE;
    }
}

bool hhg_base_type_is_arith(hhg_token_type_t token_type)
{
    switch (token_type) {
    case HHG_TYPE_I8:
    case HHG_TYPE_U8:
    case HHG_TYPE_I16:
    case HHG_TYPE_U16:
    case HHG_TYPE_I32:
    case HHG_TYPE_U32:
    case HHG_TYPE_I64:
    case HHG_TYPE_U64:
    case HHG_TYPE_INT:
    case HHG_TYPE_F32:
    case HHG_TYPE_F64:
    case HHG_TYPE_FLOAT:
        return true;
    default:
        return false;
    }
}

bool hhg_type_eq(hhg_type_t *l, hhg_type_t *r)
{
    // all types are cached so pointer equality is sufficient
    if (l == r)
        return true;

    // otherwise, compare allowing qualifiers to differ

    // eliminate base case
    if (l->type != r->type)
        return false;

    // recursively compare type info for complex types
    switch (l->type) {
    case HHG_TYPE_REF:
        return hhg_type_eq(l->info.ref.base, r->info.ref.base);
    case HHG_TYPE_ARR:
        return l->info.arr.size == r->info.arr.size &&
            hhg_type_eq(l->info.arr.elem, r->info.arr.elem);
    case HHG_TYPE_FUNC:
        return l->info.func.sym == r->info.func.sym;
    case HHG_TYPE_CLASS:
        return l->info.class.sym == r->info.class.sym;
    default:
        return false;
    }
}

void hhg_type_print(hhg_type_t *type)
{
    hhg_type_print_core(
        type,
        hhg_vfprintf_out_char,
        hhg_vfprintf_out_str,
        stdout
    );
}

void hhg_type_print_core(
    hhg_type_t *type,
    void (*out_char)(void *arg, char c),
    void (*out_str)(void *arg, const char *str),
    void *arg
)
{
    if (type == NULL) {
        out_str(arg, "none");
        return;
    }

    if (type->is_const)
        out_str(arg, "const");

    if (type->is_volatile)
        out_str(arg, "volatile");

    out_str(arg, base_type_to_str[type->type]);

    switch (type->type) {
    case HHG_TYPE_REF:
        out_char(arg, ' ');
        hhg_type_print_core(type->info.ref.base, out_char, out_str, arg);
        break;
    case HHG_TYPE_ARR:
        hhg_printf_core(
            " [%zd] of ",
            out_str,
            out_char,
            arg,
            type->info.arr.size
        );
        hhg_type_print_core(type->info.arr.elem, out_char, out_str, arg);
        break;
    case HHG_TYPE_FUNC:
        // methods have no symbol
        if (type->info.func.sym != NULL) {
            out_char(arg, ' ');
            out_str(arg, type->info.func.sym->key);
        }
        break;
    case HHG_TYPE_CLASS:
        out_char(arg, ' ');
        out_str(arg, type->info.class.sym->key);
        break;
    case HHG_TYPE_ID:
        out_char(arg, ' ');
        out_str(arg, type->info.id);
        break;
    default:
        break;
    }
}

void hhg_type_del(hhg_type_t *type)
{
    switch (type->type) {
    case HHG_TYPE_FUNC: {
        size_t len = arrlenu(type->info.func.params);
        for (size_t i = 0; i < len; i++)
            hhg_type_del(type->info.func.params[i]);
        arrfree(type->info.func.params);
        break;
    }
    case HHG_TYPE_CLASS: {
        size_t len = shlenu(type->info.class.fields);
        for (size_t i = 0; i < len; i++)
            hhg_type_del(type->info.class.fields[i].value);
        shfree(type->info.class.fields);
        break;
    }
    }
}

