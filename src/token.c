#include <stdio.h>

#include "token.h"

const char *const token_type_to_str[] = {
    "none",
    "id",

    // literals
    "int literal",
    "float literal",

    "char literal",
    "string literal",

    // keywords
    "if",
    "while",
    "for",

    "break",
    "continue",

    "and",
    "or",
    "not",

    "true",
    "false",

    "in",
    "range",

    "enum",

    "def",
    
    // types
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

    // composite operators
    "<<",
    ">>",

    "==",
    "!=",
    "<=",
    ">=",

    "+=",
    "-=",
    "*=",
    "/=",
    "%=",

    "&=",
    "|=",
    "^=",

    "<<=",
    ">>=",

    "++",
    "--",
    
    // node types
    "block",
};

void hhg_token_type_print(hhg_token_type_t type)
{
    if (type == EOF) 
        fputs("EOF", stdout);
    else if (type == '\n')
        fputs("\\n", stdout);
    else if (type <= HHG_TOKEN_TYPE_START)
        fputc(type, stdout);
    else
        fputs(token_type_to_str[type - HHG_TOKEN_TYPE_START], stdout);
}

void hhg_token_init(hhg_token_t *token)
{
    token->type = HHG_TOKEN_TYPE_NONE;
    hhg_str_init(&token->str);
}

void hhg_token_reset_aux(hhg_token_t *token)
{
    hhg_str_reset(&token->str);
    token->prec = HHG_PREC_NONE;
}

void hhg_token_print(hhg_token_t *token)
{
    fputs("{ type = ", stdout);

    hhg_token_type_print(token->type);

    fputs(", value = ", stdout);

    switch (token->type) {
    case HHG_TOKEN_TYPE_INT_LITERAL:
    case HHG_TOKEN_TYPE_FLOAT_LITERAL:
    case HHG_TOKEN_TYPE_CHAR_LITERAL:
    case HHG_TOKEN_TYPE_STRING_LITERAL:
    case HHG_TOKEN_TYPE_ID:
        fputs(token->str.str, stdout); 
        break;
    default:
        fputs("null", stdout);
        break;
    }

    fputs(" }", stdout);
}

void hhg_token_del(hhg_token_t *token)
{
    hhg_str_del(&token->str);
}
