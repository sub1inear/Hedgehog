#ifndef HHG_TOKEN_H
#define HHG_TOKEN_H

#include <stdint.h>
#include <limits.h>

#include "str.h"

#define HHG_TOKEN_TYPE_START UCHAR_MAX
#define HHG_PREC_NONE (-1)
enum _hhg_token_type_t {
    HHG_TOKEN_TYPE_NONE = HHG_TOKEN_TYPE_START,
    HHG_TOKEN_TYPE_ID,
    // literals
    HHG_TOKEN_TYPE_INT_LITERAL,
    HHG_TOKEN_TYPE_FLOAT_LITERAL,

    HHG_TOKEN_TYPE_CHAR_LITERAL,
    HHG_TOKEN_TYPE_STRING_LITERAL,

    // keywords
    HHG_TOKEN_TYPE_IF,
    HHG_TOKEN_TYPE_WHILE,
    HHG_TOKEN_TYPE_FOR,

    HHG_TOKEN_TYPE_BREAK,
    HHG_TOKEN_TYPE_CONTINUE,

    HHG_TOKEN_TYPE_AND,
    HHG_TOKEN_TYPE_OR,
    HHG_TOKEN_TYPE_NOT,

    HHG_TOKEN_TYPE_TRUE,
    HHG_TOKEN_TYPE_FALSE,

    HHG_TOKEN_TYPE_IN,
    HHG_TOKEN_TYPE_RANGE,

    HHG_TOKEN_TYPE_ENUM,

    HHG_TOKEN_TYPE_DEF,

    // types
    HHG_TOKEN_TYPE_I8,
    HHG_TOKEN_TYPE_U8,

    HHG_TOKEN_TYPE_I16,
    HHG_TOKEN_TYPE_U16,
    
    HHG_TOKEN_TYPE_I32,
    HHG_TOKEN_TYPE_U32,

    HHG_TOKEN_TYPE_I64,
    HHG_TOKEN_TYPE_U64,

    HHG_TOKEN_TYPE_INT,

    HHG_TOKEN_TYPE_F32,
    HHG_TOKEN_TYPE_F64,

    HHG_TOKEN_TYPE_FLOAT,

    HHG_TOKEN_TYPE_BOOL,

    HHG_TOKEN_TYPE_CHAR,

    HHG_TOKEN_TYPE_ISIZE,
    HHG_TOKEN_TYPE_USIZE,

    HHG_TOKEN_TYPE_TIME_T,

    // composite operators
    HHG_TOKEN_TYPE_LSHIFT,
    HHG_TOKEN_TYPE_RSHIFT,

    HHG_TOKEN_TYPE_EQ,
    HHG_TOKEN_TYPE_NOT_EQ,
    HHG_TOKEN_TYPE_LT_EQ,
    HHG_TOKEN_TYPE_GT_EQ,

    HHG_TOKEN_TYPE_PLUS_EQ,
    HHG_TOKEN_TYPE_SUB_EQ,
    HHG_TOKEN_TYPE_MUL_EQ,
    HHG_TOKEN_TYPE_DIV_EQ,
    HHG_TOKEN_TYPE_MOD_EQ,

    HHG_TOKEN_TYPE_AND_EQ,
    HHG_TOKEN_TYPE_OR_EQ,
    HHG_TOKEN_TYPE_XOR_EQ,

    HHG_TOKEN_TYPE_LSHIFT_EQ,
    HHG_TOKEN_TYPE_RSHIFT_EQ,    

    HHG_TOKEN_TYPE_INC,
    HHG_TOKEN_TYPE_DEC,
};
// guarentee being signed (for comparison with EOF)
typedef int hhg_token_type_t;

#define HHG_TOKEN_TYPE_END (HHG_TOKEN_TYPE_DEC + 1)

void hhg_token_type_print(hhg_token_type_t type);

typedef struct _hhg_token_t {
    hhg_token_type_t type;
    hhg_str_t str;
    int32_t prec;
} hhg_token_t;

void hhg_token_init(hhg_token_t *token);

void hhg_token_reset_aux(hhg_token_t *token);

void hhg_token_print(hhg_token_t *token);

void hhg_token_del(hhg_token_t *token);

#endif
