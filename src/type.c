#include <stdbool.h>
#include <stdio.h>

#include <stb_ds.h>

#include "type.h"
#include "token.h"
#include "node.h"
#include "mem.h"
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

    [HHG_TYPE_F32] = "f32",
    [HHG_TYPE_F64] = "f64",

    [HHG_TYPE_BOOL] = "bool",
    [HHG_TYPE_CHAR] = "char",

    [HHG_TYPE_ISIZE] = "isize",
    [HHG_TYPE_USIZE] = "usize",

    [HHG_TYPE_VOID] = "void",

    [HHG_TYPE_REF] = "ref",
    [HHG_TYPE_ARR] = "arr",

    [HHG_TYPE_FN] = "fn",
};

static const hhg_base_type_t token_type_to_base_type[] = {
    // special tokens
    [HHG_TOKEN_NONE] = HHG_TYPE_NONE,
    [HHG_TOKEN_ID] = HHG_TYPE_NONE,
    [HHG_TOKEN_EOF] = HHG_TYPE_NONE,
    [HHG_TOKEN_NEWLINE] = HHG_TYPE_NONE,

    // arithmetic operators
    [HHG_TOKEN_PLUS] = HHG_TYPE_NONE,
    [HHG_TOKEN_MINUS] = HHG_TYPE_NONE,
    [HHG_TOKEN_STAR] = HHG_TYPE_NONE,
    [HHG_TOKEN_SLASH] = HHG_TYPE_NONE,
    [HHG_TOKEN_PERCENT] = HHG_TYPE_NONE,
    
    // bitwise operators
    [HHG_TOKEN_AMPERSAND] = HHG_TYPE_NONE,
    [HHG_TOKEN_PIPE] = HHG_TYPE_NONE,
    [HHG_TOKEN_CARET] = HHG_TYPE_NONE,
    [HHG_TOKEN_TILDE] = HHG_TYPE_NONE,
    [HHG_TOKEN_LSHIFT] = HHG_TYPE_NONE,
    [HHG_TOKEN_RSHIFT] = HHG_TYPE_NONE,

    // assignment operators
    [HHG_TOKEN_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_PLUS_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_MINUS_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_STAR_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_SLASH_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_PERCENT_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_AMPERSAND_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_PIPE_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_CARET_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_LSHIFT_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_RSHIFT_EQ] = HHG_TYPE_NONE,
    
    // comparison operators
    [HHG_TOKEN_EQ_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_NOT_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_LT] = HHG_TYPE_NONE,
    [HHG_TOKEN_LT_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_GT] = HHG_TYPE_NONE,
    [HHG_TOKEN_GT_EQ] = HHG_TYPE_NONE,

    // punctuation
    [HHG_TOKEN_ARROW] = HHG_TYPE_NONE,
    [HHG_TOKEN_FAT_ARROW] = HHG_TYPE_NONE,
    [HHG_TOKEN_DOT] = HHG_TYPE_NONE,
    [HHG_TOKEN_DOT_DOT] = HHG_TYPE_NONE,
    [HHG_TOKEN_DOT_DOT_EQ] = HHG_TYPE_NONE,
    [HHG_TOKEN_QUESTION] = HHG_TYPE_NONE,
    [HHG_TOKEN_BANG] = HHG_TYPE_NONE,
    [HHG_TOKEN_COLON] = HHG_TYPE_NONE,
    [HHG_TOKEN_AT] = HHG_TYPE_NONE,

    [HHG_TOKEN_LPAREN] = HHG_TYPE_NONE,
    [HHG_TOKEN_RPAREN] = HHG_TYPE_NONE,
    [HHG_TOKEN_LBRACE] = HHG_TYPE_NONE,
    [HHG_TOKEN_RBRACE] = HHG_TYPE_NONE,

    [HHG_TOKEN_LBRACKET] = HHG_TYPE_NONE,
    [HHG_TOKEN_RBRACKET] = HHG_TYPE_NONE,
    [HHG_TOKEN_COMMA] = HHG_TYPE_NONE,

    // literals
    [HHG_TOKEN_INT_LIT] = HHG_TYPE_NONE,
    [HHG_TOKEN_FLOAT_LIT] = HHG_TYPE_NONE,
    [HHG_TOKEN_CHAR_LIT] = HHG_TYPE_NONE,
    [HHG_TOKEN_STR_LIT] = HHG_TYPE_NONE,

    // keywords
    [HHG_TOKEN_LET] = HHG_TYPE_NONE,
    [HHG_TOKEN_MUT] = HHG_TYPE_NONE,
    [HHG_TOKEN_CONST] = HHG_TYPE_NONE,

    [HHG_TOKEN_FN] = HHG_TYPE_NONE,
    [HHG_TOKEN_CLASS] = HHG_TYPE_NONE,
    [HHG_TOKEN_ENUM] = HHG_TYPE_NONE,
    [HHG_TOKEN_INTERFACE] = HHG_TYPE_NONE,
    [HHG_TOKEN_TYPE] = HHG_TYPE_NONE,
    [HHG_TOKEN_IMPORT] = HHG_TYPE_NONE,
    [HHG_TOKEN_FROM] = HHG_TYPE_NONE,
    [HHG_TOKEN_AS] = HHG_TYPE_NONE,

    [HHG_TOKEN_IF] = HHG_TYPE_NONE,
    [HHG_TOKEN_ELSE] = HHG_TYPE_NONE,
    [HHG_TOKEN_WHILE] = HHG_TYPE_NONE,
    [HHG_TOKEN_FOR] = HHG_TYPE_NONE,
    [HHG_TOKEN_IN] = HHG_TYPE_NONE,
    [HHG_TOKEN_MATCH] = HHG_TYPE_NONE,
    [HHG_TOKEN_BREAK] = HHG_TYPE_NONE,
    [HHG_TOKEN_CONTINUE] = HHG_TYPE_NONE,
    [HHG_TOKEN_RETURN] = HHG_TYPE_NONE,

    [HHG_TOKEN_AND] = HHG_TYPE_NONE,
    [HHG_TOKEN_OR] = HHG_TYPE_NONE,
    [HHG_TOKEN_NOT] = HHG_TYPE_NONE,

    [HHG_TOKEN_TRUE] = HHG_TYPE_NONE,
    [HHG_TOKEN_FALSE] = HHG_TYPE_NONE,
    [HHG_TOKEN_NULL] = HHG_TYPE_NONE,
    [HHG_TOKEN_SELF] = HHG_TYPE_NONE,

    [HHG_TOKEN_STATIC] = HHG_TYPE_NONE,
    [HHG_TOKEN_UNSAFE] = HHG_TYPE_NONE,
    [HHG_TOKEN_PUB] = HHG_TYPE_NONE,

    // types
    [HHG_TOKEN_I8] = HHG_TYPE_I8,
    [HHG_TOKEN_U8] = HHG_TYPE_U8,

    [HHG_TOKEN_I16] = HHG_TYPE_I16,
    [HHG_TOKEN_U16] = HHG_TYPE_U16,

    [HHG_TOKEN_I32] = HHG_TYPE_I32,
    [HHG_TOKEN_U32] = HHG_TYPE_U32,

    [HHG_TOKEN_I64] = HHG_TYPE_I64,
    [HHG_TOKEN_U64] = HHG_TYPE_U64,

    [HHG_TOKEN_F32] = HHG_TYPE_F32,
    [HHG_TOKEN_F64] = HHG_TYPE_F64,

    [HHG_TOKEN_BOOL] = HHG_TYPE_BOOL,
    [HHG_TOKEN_CHAR] = HHG_TYPE_CHAR,

    [HHG_TOKEN_ISIZE] = HHG_TYPE_ISIZE,
    [HHG_TOKEN_USIZE] = HHG_TYPE_USIZE,

    [HHG_TOKEN_VOID] = HHG_TYPE_VOID,
};

static const bool base_type_is_arith[] = {
    [HHG_TYPE_NONE] = false,
    [HHG_TYPE_I8] = true,
    [HHG_TYPE_U8] = true,
    [HHG_TYPE_I16] = true,
    [HHG_TYPE_U16] = true,
    [HHG_TYPE_I32] = true,
    [HHG_TYPE_U32] = true,
    [HHG_TYPE_I64] = true,
    [HHG_TYPE_U64] = true,
    [HHG_TYPE_F32] = true,
    [HHG_TYPE_F64] = true,
    [HHG_TYPE_BOOL] = false,
    [HHG_TYPE_CHAR] = false,
    [HHG_TYPE_ISIZE] = true,
    [HHG_TYPE_USIZE] = true,
    [HHG_TYPE_VOID] = false,
    [HHG_TYPE_REF] = false,
    [HHG_TYPE_ARR] = false,
    [HHG_TYPE_FN] = false,
};

void hhg_base_type_print(hhg_base_type_t base)
{
    hhg_base_type_fprint(base, stdout);
}

void hhg_base_type_fprint(hhg_base_type_t base, FILE *stream)
{
    fputs(hhg_base_type_to_str(base), stream);
}

const char *hhg_base_type_to_str(hhg_base_type_t base)
{
    return base_type_to_str[base];
}

hhg_base_type_t hhg_token_type_to_base_type(hhg_token_type_t token_type)
{
    return token_type_to_base_type[token_type];
}

bool hhg_base_type_is_arith(hhg_token_type_t token_type)
{
    return base_type_is_arith[token_type];
}

void hhg_type_init(hhg_type_t *type, hhg_base_type_t base)
{
    *type = (hhg_type_t) {
        .type = base,
    };
}

hhg_type_t *hhg_type_new(hhg_base_type_t base, hhg_arena_t *arena)
{
    hhg_type_t *type = hhg_arena_malloc(arena, sizeof(hhg_type_t));
    hhg_type_init(type, base);
    return type;
}

bool hhg_type_eq(hhg_type_t *l, hhg_type_t *r)
{
    return l == r;
}

void hhg_type_print(hhg_type_t *type)
{
    hhg_type_fprint(type, stdout);
}

void hhg_type_fprint(hhg_type_t *type, FILE *stream)
{
    hhg_base_type_fprint(type->type, stream);

    switch (type->type) {
    case HHG_TYPE_REF:
        fputc(' ', stream);
        hhg_type_fprint(type->value.ref.base, stream);
        break;
    case HHG_TYPE_ARR:
        assert(type->value.arr.size->type == HHG_NODE_INT_LIT);
        fprintf(
            stream,
            " [%s] of ",
            type->value.arr.size->value.int_lit.str
        );
        hhg_type_fprint(type->value.arr.elem, stream);
        break;
    case HHG_TYPE_FN:
        fputs(" (", stream);
        size_t len = arrlenu(type->value.fn.params);
        for (size_t i = 0; i < len; i++) {
            hhg_type_fprint(type->value.fn.params[i], stream);
            if (i > 0) fputs(", ", stream);
        }
        fputs(") -> ", stream);
        hhg_type_fprint(type->value.fn.ret, stream);
        break;
    default:
        break;
    }
}


void hhg_type_del(hhg_type_t *type)
{
    if (type->type == HHG_TYPE_FN) {
        size_t len = arrlenu(type->value.fn.params);
        for (size_t i = 0; i < len; i++)
            hhg_type_del(type->value.fn.params[i]);
        arrfree(type->value.fn.params);
    }
}
