#include "token.h"

#include <stdio.h>

#include "file_range.h"
#include "node.h"

const char *const token_type_to_str[] = {
    // ASCII
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "newline",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    " ",
    "!",
    "\"",
    "#",
    "$",
    "%",
    "&",
    "'",
    "(",
    ")",
    "*",
    "+",
    ",",
    "-",
    ".",
    "/",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    ":",
    ";",
    "<",
    "=",
    ">",
    "?",
    "@",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "[",
    "\\",
    "]",
    "^",
    "_",
    "`",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",
    "{",
    "|",
    "}",
    "~",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",

    [HHG_TOKEN_NONE] = "none",
    [HHG_TOKEN_ID] = "id",

    // literals
    [HHG_TOKEN_INT_LITERAL] = "int literal",
    [HHG_TOKEN_FLOAT_LITERAL] = "float literal",
    [HHG_TOKEN_CHAR_LITERAL] = "char literal",
    [HHG_TOKEN_STRING_LITERAL] = "string literal",

    // keywords
    [HHG_TOKEN_IF] = "if",
    [HHG_TOKEN_WHILE] = "while",
    [HHG_TOKEN_FOR] = "for",

    [HHG_TOKEN_BREAK] = "break",
    [HHG_TOKEN_CONTINUE] = "continue",

    [HHG_TOKEN_AND] = "and",
    [HHG_TOKEN_OR] = "or",
    [HHG_TOKEN_NOT] = "not",

    [HHG_TOKEN_TRUE] = "true",
    [HHG_TOKEN_FALSE] = "false",

    [HHG_TOKEN_IN] = "in",
    [HHG_TOKEN_RANGE] = "range",

    [HHG_TOKEN_ENUM] = "enum",

    [HHG_TOKEN_DEF] = "def",
    [HHG_TOKEN_RETURN] = "return",

    [HHG_TOKEN_CLASS] = "class",

    // types
    [HHG_TOKEN_I8] = "i8",
    [HHG_TOKEN_U8] = "u8",

    [HHG_TOKEN_I16] = "i16",
    [HHG_TOKEN_U16] = "u16",

    [HHG_TOKEN_I32] = "i32",
    [HHG_TOKEN_U32] = "u32",

    [HHG_TOKEN_I64] = "i64",
    [HHG_TOKEN_U64] = "u64",

    [HHG_TOKEN_INT] = "int",

    [HHG_TOKEN_F32] = "f32",
    [HHG_TOKEN_F64] = "f64",

    [HHG_TOKEN_FLOAT] = "float",

    [HHG_TOKEN_BOOL] = "bool",

    [HHG_TOKEN_CHAR] = "char",

    [HHG_TOKEN_ISIZE] = "isize",
    [HHG_TOKEN_USIZE] = "usize",

    // type modifiers
    [HHG_TOKEN_CONST] = "const",
    [HHG_TOKEN_VOLATILE] = "volatile",

    // composite operators
    [HHG_TOKEN_LSHIFT] = "<<",
    [HHG_TOKEN_RSHIFT] = ">>",

    [HHG_TOKEN_EQ] = "==",
    [HHG_TOKEN_NOT_EQ] = "!=",
    [HHG_TOKEN_LT_EQ] = "<=",
    [HHG_TOKEN_GT_EQ] = ">=",

    [HHG_TOKEN_ADD_EQ] = "+=",
    [HHG_TOKEN_SUB_EQ] = "-=",
    [HHG_TOKEN_MUL_EQ] = "*=",
    [HHG_TOKEN_DIV_EQ] = "/=",
    [HHG_TOKEN_MOD_EQ] = "%=",

    [HHG_TOKEN_AND_EQ] = "&=",
    [HHG_TOKEN_OR_EQ] = "|=",
    [HHG_TOKEN_XOR_EQ] = "^=",

    [HHG_TOKEN_LSHIFT_EQ] = "<<=",
    [HHG_TOKEN_RSHIFT_EQ] = ">>=",

    [HHG_TOKEN_INC] = "++",
    [HHG_TOKEN_DEC] = "--",

    // node types
    [HHG_NODE_BLOCK] = "block",
    [HHG_NODE_PARAM] = "param",
    [HHG_NODE_FUNC_CALL] = "func call",
    [HHG_NODE_ARR_LITERAL] = "arr literal",
    [HHG_NODE_OBJ_INIT] = "obj init",
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
    hhg_file_range_init(&token->range);
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
    fputs(", range = ", stdout);

    hhg_file_range_print(&token->range);

    fputs(" }", stdout);
}

void hhg_token_del(hhg_token_t *token)
{
    hhg_str_del(&token->str);
}
