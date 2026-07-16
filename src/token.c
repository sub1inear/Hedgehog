#include <stdio.h>

#include "token.h"
#include "file_range.h"
#include "str.h"
#include "utils.h"

const char *const token_type_to_str[] = {
    // special tokens
    [HHG_TOKEN_NONE] = "none",
    [HHG_TOKEN_ID] = "id",
    [HHG_TOKEN_EOF] = "EOF",
    [HHG_TOKEN_NEWLINE] = "newline",

    // arithmetic operators
    [HHG_TOKEN_PLUS] = "+",
    [HHG_TOKEN_MINUS] = "-",
    [HHG_TOKEN_STAR] = "*",
    [HHG_TOKEN_SLASH] = "/",
    [HHG_TOKEN_PERCENT] = "%",
    
    // bitwise operators
    [HHG_TOKEN_AMPERSAND] = "&",
    [HHG_TOKEN_PIPE] = "|",
    [HHG_TOKEN_CARET] = "^",
    [HHG_TOKEN_TILDE] = "~",
    [HHG_TOKEN_LSHIFT] = "<<",
    [HHG_TOKEN_RSHIFT] = ">>",

    // assignment operators
    [HHG_TOKEN_EQ] = "=",
    [HHG_TOKEN_PLUS_EQ] = "+=",
    [HHG_TOKEN_MINUS_EQ] = "-=",
    [HHG_TOKEN_STAR_EQ] = "*=",
    [HHG_TOKEN_SLASH_EQ] = "/=",
    [HHG_TOKEN_PERCENT_EQ] = "%=",
    [HHG_TOKEN_AMPERSAND_EQ] = "&=",
    [HHG_TOKEN_PIPE_EQ] = "|=",
    [HHG_TOKEN_CARET_EQ] = "^=",
    [HHG_TOKEN_LSHIFT_EQ] = "<<=",
    [HHG_TOKEN_RSHIFT_EQ] = ">>=",
    
    // comparison operators
    [HHG_TOKEN_EQ_EQ] = "==",
    [HHG_TOKEN_NOT_EQ] = "!=",
    [HHG_TOKEN_LT] = "<",
    [HHG_TOKEN_LT_EQ] = "<=",
    [HHG_TOKEN_GT] = ">",
    [HHG_TOKEN_GT_EQ] = ">=",

    // punctuation
    [HHG_TOKEN_ARROW] = "->",
    [HHG_TOKEN_FAT_ARROW] = "=>",

    [HHG_TOKEN_DOT] = ".",
    [HHG_TOKEN_DOT_DOT] = "..",
    [HHG_TOKEN_DOT_DOT_EQ] = "..=",

    [HHG_TOKEN_QUESTION] = "?",
    [HHG_TOKEN_BANG] = "!",
    [HHG_TOKEN_COLON] = ":",
    [HHG_TOKEN_AT] = "@",

    [HHG_TOKEN_LPAREN] = "(",
    [HHG_TOKEN_RPAREN] = ")",
    
    [HHG_TOKEN_LBRACE] = "{",
    [HHG_TOKEN_RBRACE] = "}",

    [HHG_TOKEN_LBRACKET] = "[",
    [HHG_TOKEN_RBRACKET] = "]",

    [HHG_TOKEN_COMMA] = ",",

    // literals
    [HHG_TOKEN_INT_LIT] = "int lit",
    [HHG_TOKEN_FLOAT_LIT] = "float lit",
    [HHG_TOKEN_CHAR_LIT] = "char lit",
    [HHG_TOKEN_STR_LIT] = "str lit",

    // keywords
    [HHG_TOKEN_LET] = "let",
    [HHG_TOKEN_MUT] = "mut",
    [HHG_TOKEN_CONST] = "const",

    [HHG_TOKEN_FN] = "fn",
    [HHG_TOKEN_CLASS] = "class",
    [HHG_TOKEN_ENUM] = "enum",
    [HHG_TOKEN_INTERFACE] = "interface",
    [HHG_TOKEN_TYPE] = "type",
    [HHG_TOKEN_IMPORT] = "import",
    [HHG_TOKEN_FROM] = "from",
    [HHG_TOKEN_AS] = "as",

    [HHG_TOKEN_IF] = "if",
    [HHG_TOKEN_ELSE] = "else",
    [HHG_TOKEN_WHILE] = "while",
    [HHG_TOKEN_FOR] = "for",
    [HHG_TOKEN_IN] = "in",
    [HHG_TOKEN_MATCH] = "match",
    [HHG_TOKEN_BREAK] = "break",
    [HHG_TOKEN_CONTINUE] = "continue",
    [HHG_TOKEN_RETURN] = "return",

    [HHG_TOKEN_AND] = "and",
    [HHG_TOKEN_OR] = "or",
    [HHG_TOKEN_NOT] = "not",

    [HHG_TOKEN_TRUE] = "true",
    [HHG_TOKEN_FALSE] = "false",
    [HHG_TOKEN_NULL] = "null",
    [HHG_TOKEN_SELF] = "self",

    [HHG_TOKEN_STATIC] = "static",
    [HHG_TOKEN_UNSAFE] = "unsafe",
    [HHG_TOKEN_PUB] = "pub",

    // types
    [HHG_TOKEN_I8] = "i8",
    [HHG_TOKEN_U8] = "u8",

    [HHG_TOKEN_I16] = "i16",
    [HHG_TOKEN_U16] = "u16",

    [HHG_TOKEN_I32] = "i32",
    [HHG_TOKEN_U32] = "u32",

    [HHG_TOKEN_I64] = "i64",
    [HHG_TOKEN_U64] = "u64",

    [HHG_TOKEN_F32] = "f32",
    [HHG_TOKEN_F64] = "f64",

    [HHG_TOKEN_BOOL] = "bool",

    [HHG_TOKEN_CHAR] = "char",

    [HHG_TOKEN_ISIZE] = "isize",
    [HHG_TOKEN_USIZE] = "usize",
};

void hhg_token_init(hhg_token_t *token)
{
    token->type = HHG_TOKEN_NONE;
    token->prec = HHG_PREC_NONE;
    hhg_str_init(&token->str);
    hhg_file_range_init(&token->range);
}

void hhg_token_type_print(hhg_token_type_t type)
{
    hhg_token_type_fprint(type, stdout);
}

void hhg_token_type_fprint(hhg_token_type_t type, FILE *stream)
{
    fputs(hhg_token_type_to_str(type), stream);
}

const char *hhg_token_type_to_str(hhg_token_type_t type)
{
    hhg_assert(type >= 0 && type < HHG_ARR_LEN(token_type_to_str));
    return token_type_to_str[type];
}

void hhg_token_reset_aux(hhg_token_t *token)
{
    hhg_str_reset(&token->str);
    token->prec = HHG_PREC_NONE;
}

void hhg_token_print(hhg_token_t *token)
{
    hhg_token_fprint(token, stdout);
}

void hhg_token_fprint(hhg_token_t *token, FILE *stream)
{
    fprintf(
        stream,
        "{ .type = `%s`, .value = ",
        hhg_token_type_to_str(token->type)
    );
    switch (token->type) {
    case HHG_TOKEN_INT_LIT:
    case HHG_TOKEN_FLOAT_LIT:
    case HHG_TOKEN_ID:
        fprintf(stream, "`%s`", token->str.str);
        break;
    case HHG_TOKEN_CHAR_LIT:
        fprintf(stream, "'%s'", token->str.str);
        break;
    case HHG_TOKEN_STR_LIT:
        fprintf(stream, "\"%s\"", token->str.str);
        break;
    default:
        fputs("null", stream);
        break;
    }
    fputs(", .range = ", stream);
    hhg_file_range_fprint(&token->range, stream);
    fputs(" }", stream);
}


void hhg_token_del(hhg_token_t *token)
{
    hhg_str_del(&token->str);
}
