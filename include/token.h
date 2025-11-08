#ifndef HHG_TOKEN_H
#define HHG_TOKEN_H

#include <stdint.h>
#include <limits.h>

#include "str.h"

#define HHG_TOKEN_START UCHAR_MAX
enum hhg_token_type {
    HHG_TOKEN_NONE = HHG_TOKEN_START,
    HHG_TOKEN_ID,
    // literals
    HHG_TOKEN_INT_LITERAL,
    HHG_TOKEN_FLOAT_LITERAL,

    HHG_TOKEN_CHAR_LITERAL,
    HHG_TOKEN_STRING_LITERAL,

    // keywords
    HHG_TOKEN_IF,
    HHG_TOKEN_WHILE,
    HHG_TOKEN_FOR,

    HHG_TOKEN_BREAK,
    HHG_TOKEN_CONTINUE,

    HHG_TOKEN_AND,
    HHG_TOKEN_OR,
    HHG_TOKEN_NOT,

    HHG_TOKEN_TRUE,
    HHG_TOKEN_FALSE,

    HHG_TOKEN_IN,
    HHG_TOKEN_RANGE,

    HHG_TOKEN_ENUM,

    HHG_TOKEN_DEF,
    HHG_TOKEN_RETURN,

    HHG_TOKEN_CLASS,

    // types
    HHG_TOKEN_I8,
    HHG_TOKEN_U8,

    HHG_TOKEN_I16,
    HHG_TOKEN_U16,
    
    HHG_TOKEN_I32,
    HHG_TOKEN_U32,

    HHG_TOKEN_I64,
    HHG_TOKEN_U64,

    HHG_TOKEN_INT,

    HHG_TOKEN_F32,
    HHG_TOKEN_F64,

    HHG_TOKEN_FLOAT,

    HHG_TOKEN_BOOL,

    HHG_TOKEN_CHAR,

    HHG_TOKEN_ISIZE,
    HHG_TOKEN_USIZE,

    HHG_TOKEN_TIME_T,

    // type modifiers
    HHG_TOKEN_CONST,
    HHG_TOKEN_VOLATILE,

    // composite operators
    HHG_TOKEN_LSHIFT,
    HHG_TOKEN_RSHIFT,

    HHG_TOKEN_EQ,
    HHG_TOKEN_NOT_EQ,
    HHG_TOKEN_LT_EQ,
    HHG_TOKEN_GT_EQ,

    HHG_TOKEN_PLUS_EQ,
    HHG_TOKEN_SUB_EQ,
    HHG_TOKEN_MUL_EQ,
    HHG_TOKEN_DIV_EQ,
    HHG_TOKEN_MOD_EQ,

    HHG_TOKEN_AND_EQ,
    HHG_TOKEN_OR_EQ,
    HHG_TOKEN_XOR_EQ,

    HHG_TOKEN_LSHIFT_EQ,
    HHG_TOKEN_RSHIFT_EQ,    

    HHG_TOKEN_INC,
    HHG_TOKEN_DEC,
};
// guarantee being signed (for comparison with EOF)
typedef int hhg_token_type_t;

#define HHG_TOKEN_END (HHG_TOKEN_DEC + 1)

#define HHG_PREC_NONE (-1)

const char *hhg_token_type_to_str(hhg_token_type_t type);

void hhg_token_type_print(hhg_token_type_t type);

typedef struct hhg_token {
    hhg_token_type_t type;
    hhg_str_t str;
    int32_t prec;
} hhg_token_t;

void hhg_token_init(hhg_token_t *token);

void hhg_token_reset_aux(hhg_token_t *token);

bool hhg_token_is_type(hhg_token_t *token);

void hhg_token_print(hhg_token_t *token);

void hhg_token_del(hhg_token_t *token);

#endif
