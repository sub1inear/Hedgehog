#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stb_ds.h>

#include "type.h"
#include "type_ctx.h"
#include "token.h"
#include "mem.h"
#include "sym.h"

static const char *const base_type_to_str[] = {
    "none",

    "i8",
    "u8",

    "i16",
    "u16",

    "i32",
    "u32",

    "i64",
    "u64",

    "int",

    "f32",
    "f64",

    "float",

    "bool",

    "char",

    "isize",
    "usize",

    "time_t",

    "ref",
    "arr",

    "func",
    "class",
    "enum",
};


static bool hhg_type_is_arith(hhg_type_t *type);
static bool hhg_type_dyn_promote(
    hhg_type_promote_t *result,
    hhg_type_t *l,
    hhg_type_t *r,
    hhg_base_type_t expect,
    hhg_type_t *target
);
static bool hhg_type_is_signed(hhg_type_t *type);
static int32_t hhg_type_get_rank(hhg_type_t *type);
static int32_t hhg_type_get_bits(hhg_type_t *type);
static hhg_base_type_t hhg_base_type_to_unsigned(hhg_base_type_t base);

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
    case HHG_TOKEN_TIME_T:
        return HHG_TYPE_TIME_T;
    default:
        return HHG_TYPE_NONE;
    }
}

bool hhg_type_eq(hhg_type_t *l, hhg_type_t *r, bool strict)
{
    // all types are cached so pointer equality is sufficient
    if (l == r)
        return true;

    // if we are being strict, exit
    if (strict)
        return false;

    // otherwise, compare allowing qualifiers to differ

    // eliminate base case
    if (l->type != r->type)
        return false;

    // allow r to be more qualified than l
    if (l->is_const && !r->is_const)
        return false;
    if (l->is_volatile && !r->is_volatile)
        return false;

    // compare type info for complex types
    switch (l->type) {
    case HHG_TYPE_REF:
        return hhg_type_eq(l->info.ref.base, r->info.ref.base, strict);
    case HHG_TYPE_ARR:
        return l->info.arr.size == r->info.arr.size &&
            hhg_type_eq(l->info.arr.elem, r->info.arr.elem, strict);
    case HHG_TYPE_FUNC:
        return l->info.func.sym == r->info.func.sym;
    case HHG_TYPE_CLASS:
        return l->info.class.sym == r->info.class.sym;
    default:
        return false;
    }
}

bool hhg_type_arith_promote(
    hhg_type_promote_t *result,
    hhg_type_t *l,
    hhg_type_t *r,
    hhg_type_ctx_t *type_ctx
)
{
    // only arithmetic types can be promoted, otherwise types must be equal
    if (!hhg_type_is_arith(l) || !hhg_type_is_arith(r)) {
        if (hhg_type_eq(l, r, true)) {
            *result = (hhg_type_promote_t){ .l = l, .r = r };
            return false;
        } else
            return true;
    }

    // if either is a dynamic float, promote other to a standard f64
    if (!hhg_type_dyn_promote(
            result,
            l, r,
            HHG_TYPE_F64,
            hhg_type_ctx_get_builtin(type_ctx, HHG_TYPE_F64)
        ))
        return false;

    // if either is a dynamic int, promote other to a standard i64
    if (!hhg_type_dyn_promote(
            result,
            l, r,
            HHG_TYPE_I64,
            hhg_type_ctx_get_builtin(type_ctx, HHG_TYPE_I64)
        ))
        return false;
    
    // if same signedness, promote to higher rank
    if (hhg_type_is_signed(l) == hhg_type_is_signed(r)) {
        if (hhg_type_get_rank(l) > hhg_type_get_rank(r))
            *result = (hhg_type_promote_t){ .l = l, .r = l };
        else
            *result = (hhg_type_promote_t){ .l = l, .r = l };
        return false;
    }

    // identify signed and unsigned types
    hhg_type_t *s;
    hhg_type_t *u;
    if (hhg_type_is_signed(l)) {
        s = l;
        u = r;
    } else {
        s = r;
        u = l;
    }
    
    // higher ranked unsigned type wins
    if (hhg_type_get_rank(u) >= hhg_type_get_rank(s)) {
        *result = (hhg_type_promote_t){ .l = u, .r = u };
        return false;
    }
    
    // if signed type can represent all values of unsigned type, it wins
    if (hhg_type_get_bits(s) > hhg_type_get_bits(u)) {
        *result = (hhg_type_promote_t){ .l = s, .r = s };
        return false;
    }

    // otherwise, promote both to unsigned type
    hhg_base_type_t u_base = hhg_base_type_to_unsigned(s->type);
    *result = (hhg_type_promote_t){
        .l = hhg_type_ctx_get_builtin(type_ctx, u_base),
        .r = hhg_type_ctx_get_builtin(type_ctx, u_base),
    };
    return false;
}

hhg_type_t *hhg_type_assign_promote(
    hhg_type_t *l,
    hhg_type_t *r
)
{
    if (!hhg_type_is_arith(l) || !hhg_type_is_arith(r)) {
        if (hhg_type_eq(l, r, false))
            return l;
        else
            return NULL; // non-arithmetic types must be equal (checked above)
    }

    return l;
}

void hhg_type_print(hhg_type_t *type)
{
    if (type->is_const)
        fputs("const ", stdout);
    
    if (type->is_volatile)
        fputs("volatile ", stdout);

    fputs(base_type_to_str[type->type], stdout);

    switch (type->type) {
    case HHG_TYPE_REF:
        hhg_type_print(type->info.ref.base);
        break;
    case HHG_TYPE_ARR:
        printf("[%zd] of ", type->info.arr.size);
        hhg_type_print(type->info.arr.elem);
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

bool hhg_type_is_arith(hhg_type_t *type)
{
    switch (type->type) {
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

// helper for dynamic type promotion
static bool hhg_type_dyn_promote(
    hhg_type_promote_t *result,
    hhg_type_t *l,
    hhg_type_t *r,
    hhg_base_type_t expect,
    hhg_type_t *target
)
{
    if (l->type == expect) {
        *result = (hhg_type_promote_t){ .l = l, .r = target };
        return false;
    }
    if (r->type == expect) {
        *result = (hhg_type_promote_t){ .l = target, .r = r };
        return false;
    }
    return true;
}


bool hhg_type_is_signed(hhg_type_t *type)
{
    switch (type->type) {
    case HHG_TYPE_I8:
    case HHG_TYPE_I16:
    case HHG_TYPE_I32:
    case HHG_TYPE_I64:
    case HHG_TYPE_INT:
        return true;
    default:
        return false;
    }
}

int32_t hhg_type_get_rank(hhg_type_t *type)
{
    switch (type->type) {
    case HHG_TYPE_I8:
    case HHG_TYPE_U8:
        return 0;
    case HHG_TYPE_I16:
    case HHG_TYPE_U16:
        return 1;
    case HHG_TYPE_I32:
    case HHG_TYPE_U32:
        return 2;
    case HHG_TYPE_I64:
    case HHG_TYPE_U64:
        return 3;
    case HHG_TYPE_F32:
        return 4;
    case HHG_TYPE_F64:
        return 5;
    default:
        return -1;
    }
}

int32_t hhg_type_get_bits(hhg_type_t *type)
{
    switch (type->type) {
    case HHG_TYPE_I8:
        return 7;
    case HHG_TYPE_U8:
        return 8;
    case HHG_TYPE_I16:
        return 15;
    case HHG_TYPE_U16:
        return 16;
    case HHG_TYPE_I32:
        return 31;
    case HHG_TYPE_U32:
        return 32;
    case HHG_TYPE_F32:
        return 32;
    case HHG_TYPE_I64:
        return 63;
    case HHG_TYPE_U64:
        return 64;
    case HHG_TYPE_F64:
        return 64;
    case HHG_TYPE_INT:
        return INT32_MAX;
    case HHG_TYPE_ISIZE:
        return INT32_MAX;
    case HHG_TYPE_USIZE:
        return INT32_MAX;
    case HHG_TYPE_TIME_T:
        return INT32_MAX;
    case HHG_TYPE_FLOAT:
        return INT32_MAX;
    default:
        return -1;
    }
}

static hhg_base_type_t hhg_base_type_to_unsigned(hhg_base_type_t base)
{
    switch (base) {
    case HHG_TYPE_I8:
    case HHG_TYPE_U8:
        return HHG_TYPE_U8;
    case HHG_TYPE_I16:
    case HHG_TYPE_U16:
        return HHG_TYPE_U16;
    case HHG_TYPE_I32:
    case HHG_TYPE_U32:
        return HHG_TYPE_U32;
    case HHG_TYPE_I64:
    case HHG_TYPE_U64:
        return HHG_TYPE_U64;
    default:
        return base;
    }
}
