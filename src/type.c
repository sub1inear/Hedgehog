#include <stdbool.h>
#include <stdio.h>

#include "type.h"
#include "token.h"

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

    "func",
    "class",
    "enum",
};

void hhg_type_init(hhg_type_t *type)
{
    *type = (hhg_type_t) {
        .type = HHG_TYPE_NONE,
        .is_const = false,
        .is_volatile = false,
    };
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

void hhg_type_print(hhg_type_t type)
{
    if (type.is_const)
        fputs("const ", stdout);
    
    if (type.is_volatile)
        fputs("volatile ", stdout);

    fputs(base_type_to_str[type.type], stdout);
}
