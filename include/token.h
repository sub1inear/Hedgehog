#ifndef HHG_TOKEN_H
#define HHG_TOKEN_H

#include <stdint.h>
#include <limits.h>

#include "str.h"

#define HHG_TOKEN_TYPE_START UCHAR_MAX
#define HHG_PREC_NONE (-1)

typedef enum _hhg_token_type_t {
    NONE = HHG_TOKEN_TYPE_START,
    ID,
    // literals
    INT_LITERAL,
    FLOAT_LITERAL,

    CHAR_LITERAL,
    STRING_LITERAL,

    // keywords
    IF,
    WHILE,
    FOR,

    BREAK,
    CONTINUE,

    AND,
    OR,
    NOT,

    TRUE,
    FALSE,

    IN,
    RANGE,

    ENUM,

    DEF,

    // types
    I8,
    U8,

    I16,
    U16,
    
    I32,
    U32,

    I64,
    U64,

    INT,

    F32,
    F64,

    FLOAT,

    BOOL,

    CHAR,

    ISIZE,
    USIZE,

    TIME_T,

    // composite operators
    LSHIFT,
    RSHIFT,

    EQ,
    NOT_EQ,
    LT_EQ,
    GT_EQ,

    PLUS_EQ,
    SUB_EQ,
    MUL_EQ,
    DIV_EQ,
    MOD_EQ,
    
    AND_EQ,
    OR_EQ,
    XOR_EQ,

    LSHIFT_EQ,
    RSHIFT_EQ,    

    INC,
    DEC,
} hhg_token_type_t;

#define HHG_TOKEN_TYPE_END (DEC + 1)

const char *hhg_token_type_to_str(hhg_token_type_t type);

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