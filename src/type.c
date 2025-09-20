#include <stdbool.h>
#include <stdio.h>

#include "type.h"
#include "token.h"

static hhg_token_type_t hhg_type_to_token_type(hhg_type_t type);

hhg_type_t hhg_token_type_to_type(hhg_token_type_t token_type)
{
    switch (token_type) {
    case I8:
        return (hhg_type_t) { TYPE_I8, false, false, 0 };
    case U8:
        return (hhg_type_t) { TYPE_U8, false, false, 0 };
    case I16:
        return (hhg_type_t) { TYPE_I16, false, false, 0 };
    case U16:
        return (hhg_type_t) { TYPE_U16, false, false, 0 };
    case I32:
        return (hhg_type_t) { TYPE_I32, false, false, 0 };
    case U32:
        return (hhg_type_t) { TYPE_U32, false, false, 0 };
    case I64:
        return (hhg_type_t) { TYPE_I64, false, false, 0 };
    case U64:
        return (hhg_type_t) { TYPE_U64, false, false, 0 };
    case INT:
        return (hhg_type_t) { TYPE_INT, false, false, 0 };
    case F32:
        return (hhg_type_t) { TYPE_F32, false, false, 0 };
    case F64:
        return (hhg_type_t) { TYPE_F64, false, false, 0 };
    case FLOAT:
        return (hhg_type_t) { TYPE_FLOAT, false, false, 0 };
    case BOOL:
        return (hhg_type_t) { TYPE_BOOL, false, false, 0 };
    case CHAR:
        return (hhg_type_t) { TYPE_CHAR, false, false, 0 };
    case ISIZE:
        return (hhg_type_t) { TYPE_ISIZE, false, false, 0 };
    case USIZE:
        return (hhg_type_t) { TYPE_USIZE, false, false, 0 };
    case TIME_T:
        return (hhg_type_t) { TYPE_TIME_T, false, false, 0 };
    default:
        return (hhg_type_t) { TYPE_NONE, false, false, 0 };
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

    hhg_token_type_print(hhg_type_to_token_type(type));

}

static hhg_token_type_t hhg_type_to_token_type(hhg_type_t type)
{
    switch (type.type) {
    case TYPE_I8:
        return I8;
    case TYPE_U8:
        return U8;
    case TYPE_I16:
        return I16;
    case TYPE_U16:
        return U16;
    case TYPE_I32:
        return I32;
    case TYPE_U32:
        return U32;
    case TYPE_I64:
        return I64;
    case TYPE_U64:
        return U64;
    case TYPE_INT:
        return INT;
    case TYPE_F32:
        return F32;
    case TYPE_F64:
        return F64;
    case TYPE_FLOAT:
        return FLOAT;
    case TYPE_BOOL:
        return BOOL;
    case TYPE_CHAR:
        return CHAR;
    case TYPE_ISIZE:
        return ISIZE;
    case TYPE_USIZE:
        return USIZE;
    case TYPE_TIME_T:
        return TIME_T;
    default:
        return NONE;
    }
}