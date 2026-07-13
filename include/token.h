#ifndef HHG_TOKEN_H
#define HHG_TOKEN_H

#include <stdint.h>
#include <limits.h>

#include "file_range.h"
#include "str.h"

#define HHG_TOKEN_START UCHAR_MAX
#define HHG_TOKEN_END (HHG_TOKEN_RSHIFT_EQ + 1)

#define HHG_PREC_NONE (-1)

typedef struct hhg_str hhg_str_t;

enum hhg_token_type {
    HHG_TOKEN_NONE = HHG_TOKEN_START,
    HHG_TOKEN_ID,
    // literals
    HHG_TOKEN_INT_LIT,
    HHG_TOKEN_FLOAT_LIT,

    HHG_TOKEN_CHAR_LIT,
    HHG_TOKEN_STR_LIT,

    // keywords
    HHG_TOKEN_LET,
    HHG_TOKEN_MUT,
    HHG_TOKEN_CONST,

    HHG_TOKEN_FN,
    HHG_TOKEN_CLASS,
    HHG_TOKEN_ENUM,
    HHG_TOKEN_INTERFACE,
    HHG_TOKEN_TYPE,
    HHG_TOKEN_IMPORT,
    HHG_TOKEN_FROM,
    HHG_TOKEN_AS,
    
    HHG_TOKEN_IF,
    HHG_TOKEN_ELSE,
    HHG_TOKEN_WHILE,
    HHG_TOKEN_FOR,
    HHG_TOKEN_IN,
    HHG_TOKEN_MATCH,
    HHG_TOKEN_BREAK,
    HHG_TOKEN_CONTINUE,
    HHG_TOKEN_RETURN,
    
    HHG_TOKEN_AND,
    HHG_TOKEN_OR,
    HHG_TOKEN_NOT,
    
    HHG_TOKEN_TRUE,
    HHG_TOKEN_FALSE,
    HHG_TOKEN_NULL,
    HHG_TOKEN_SELF,
    
    HHG_TOKEN_STATIC,
    HHG_TOKEN_UNSAFE,
    HHG_TOKEN_PUB,

    // types
    HHG_TOKEN_I8,
    HHG_TOKEN_U8,

    HHG_TOKEN_I16,
    HHG_TOKEN_U16,
    
    HHG_TOKEN_I32,
    HHG_TOKEN_U32,

    HHG_TOKEN_I64,
    HHG_TOKEN_U64,

    HHG_TOKEN_F32,
    HHG_TOKEN_F64,

    HHG_TOKEN_BOOL,
    HHG_TOKEN_CHAR,

    HHG_TOKEN_ISIZE,
    HHG_TOKEN_USIZE,

    HHG_TOKEN_THIN_ARROW,
    HHG_TOKEN_FAT_ARROW,

    HHG_TOKEN_LSHIFT,
    HHG_TOKEN_RSHIFT,

    HHG_TOKEN_EQ,
    HHG_TOKEN_NOT_EQ,
    HHG_TOKEN_LT_EQ,
    HHG_TOKEN_GT_EQ,

    HHG_TOKEN_ADD_EQ,
    HHG_TOKEN_SUB_EQ,
    HHG_TOKEN_MUL_EQ,
    HHG_TOKEN_DIV_EQ,
    HHG_TOKEN_MOD_EQ,

    HHG_TOKEN_AND_EQ,
    HHG_TOKEN_OR_EQ,
    HHG_TOKEN_XOR_EQ,

    HHG_TOKEN_LSHIFT_EQ,
    HHG_TOKEN_RSHIFT_EQ,
};
// guarantee being signed (for comparison with EOF)
typedef int hhg_token_type_t;

void hhg_token_type_print(hhg_token_type_t type);
void hhg_token_type_fprint(hhg_token_type_t type, FILE *stream);
const char *hhg_token_type_to_str(hhg_token_type_t type);

typedef struct hhg_token {
    hhg_token_type_t type;
    hhg_str_t str;
    int32_t prec;
    hhg_file_range_t range;
} hhg_token_t;

void hhg_token_init(hhg_token_t *token);

void hhg_token_reset_aux(hhg_token_t *token);

void hhg_token_print(hhg_token_t *token);
void hhg_token_fprint(hhg_token_t *token, FILE *stream);

void hhg_token_del(hhg_token_t *token);

#endif
