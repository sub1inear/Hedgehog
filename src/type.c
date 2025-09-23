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

hhg_type_t hhg_token_type_to_type(hhg_token_type_t token_type)
{
    switch (token_type) {
    case HHG_TOKEN_TYPE_I8:
        return (hhg_type_t) { HHG_TYPE_I8, false, false, 0 };
    case HHG_TOKEN_TYPE_U8:
        return (hhg_type_t) { HHG_TYPE_U8, false, false, 0 };
    case HHG_TOKEN_TYPE_I16:
        return (hhg_type_t) { HHG_TYPE_I16, false, false, 0 };
    case HHG_TOKEN_TYPE_U16:
        return (hhg_type_t) { HHG_TYPE_U16, false, false, 0 };
    case HHG_TOKEN_TYPE_I32:
        return (hhg_type_t) { HHG_TYPE_I32, false, false, 0 };
    case HHG_TOKEN_TYPE_U32:
        return (hhg_type_t) { HHG_TYPE_U32, false, false, 0 };
    case HHG_TOKEN_TYPE_I64:
        return (hhg_type_t) { HHG_TYPE_I64, false, false, 0 };
    case HHG_TOKEN_TYPE_U64:
        return (hhg_type_t) { HHG_TYPE_U64, false, false, 0 };
    case HHG_TOKEN_TYPE_INT:
        return (hhg_type_t) { HHG_TYPE_INT, false, false, 0 };
    case HHG_TOKEN_TYPE_F32:
        return (hhg_type_t) { HHG_TYPE_F32, false, false, 0 };
    case HHG_TOKEN_TYPE_F64:
        return (hhg_type_t) { HHG_TYPE_F64, false, false, 0 };
    case HHG_TOKEN_TYPE_FLOAT:
        return (hhg_type_t) { HHG_TYPE_FLOAT, false, false, 0 };
    case HHG_TOKEN_TYPE_BOOL:
        return (hhg_type_t) { HHG_TYPE_BOOL, false, false, 0 };
    case HHG_TOKEN_TYPE_CHAR:
        return (hhg_type_t) { HHG_TYPE_CHAR, false, false, 0 };
    case HHG_TOKEN_TYPE_ISIZE:
        return (hhg_type_t) { HHG_TYPE_ISIZE, false, false, 0 };
    case HHG_TOKEN_TYPE_USIZE:
        return (hhg_type_t) { HHG_TYPE_USIZE, false, false, 0 };
    case HHG_TOKEN_TYPE_TIME_T:
        return (hhg_type_t) { HHG_TYPE_TIME_T, false, false, 0 };
    default:
        return (hhg_type_t) { HHG_TYPE_NONE, false, false, 0 };
    }
}

void hhg_type_print(hhg_type_t type)
{
    if (type.is_const)
        fputs("const ", stdout);
    
    if (type.is_volatile)
        fputs("volatile ", stdout);
    
    for (unsigned int i = 0; i < type.ref_count; i++)
        fputc('&', stdout);

    fputs(base_type_to_str[type.type], stdout);
}
