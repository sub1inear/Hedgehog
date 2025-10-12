#include <stdbool.h>
#include <stdio.h>

#include "token.h"

const char *const token_type_to_str[] = {
    // ASCII
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "newline", "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", " ",  "!", "\"", "#", "$", "%", "&", "'", "(",       ")", "*", "+", ",",
    "-", ".", "/",  "0", "1",  "2", "3", "4", "5", "6", "7",       "8", "9", ":", ";",
    "<", "=", ">",  "?", "@",  "A", "B", "C", "D", "E", "F",       "G", "H", "I", "J",
    "K", "L", "M",  "N", "O",  "P", "Q", "R", "S", "T", "U",       "V", "W", "X", "Y",
    "Z", "[", "\\", "]", "^",  "_", "`", "a", "b", "c", "d",       "e", "f", "g", "h",
    "i", "j", "k",  "l", "m",  "n", "o", "p", "q", "r", "s",       "t", "u", "v", "w",
    "x", "y", "z",  "{", "|",  "}", "~", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",
    "?", "?", "?",  "?", "?",  "?", "?", "?", "?", "?", "?",       "?", "?", "?", "?",

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

    // type modifiers
    "const",
    "volatile",

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
    "arg",
};

const char *hhg_token_type_to_str(hhg_token_type_t type)
{
    if (type == EOF) 
        return "EOF";
    else
        return token_type_to_str[type];
}

void hhg_token_type_print(hhg_token_type_t type)
{
    fputs(hhg_token_type_to_str(type), stdout);
}

void hhg_token_init(hhg_token_t *token)
{
    token->type = HHG_TOKEN_NONE;
    hhg_str_init(&token->str);
}

void hhg_token_reset_aux(hhg_token_t *token)
{
    hhg_str_reset(&token->str);
    token->prec = HHG_PREC_NONE;
}


bool hhg_token_is_type(hhg_token_t *token)
{
    switch (token->type) {
    case HHG_TOKEN_I8:
    case HHG_TOKEN_U8:
    case HHG_TOKEN_I16:
    case HHG_TOKEN_U16:
    case HHG_TOKEN_I32:
    case HHG_TOKEN_U32:
    case HHG_TOKEN_I64:
    case HHG_TOKEN_U64:
    case HHG_TOKEN_INT:
    case HHG_TOKEN_F32:
    case HHG_TOKEN_F64:
    case HHG_TOKEN_FLOAT:
    case HHG_TOKEN_BOOL:
    case HHG_TOKEN_CHAR:
    case HHG_TOKEN_ISIZE:
    case HHG_TOKEN_USIZE:
    case HHG_TOKEN_TIME_T:
        return true;
    default:
        return false;
    }
}


void hhg_token_print(hhg_token_t *token)
{
    fputs("{ type = ", stdout);

    hhg_token_type_print(token->type);

    fputs(", value = ", stdout);

    switch (token->type) {
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
    case HHG_TOKEN_CHAR_LITERAL:
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_ID:
        if (token->str.str)
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
