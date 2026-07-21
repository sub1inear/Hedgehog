#ifndef HHG_TOKEN_H
#define HHG_TOKEN_H

#include <stdint.h>

#include "file_range.h"
#include "str.h"

#define HHG_PREC_NONE (-1)

#define HHG_TOKEN_TYPE_START HHG_TOKEN_NONE
#define HHG_TOKEN_TYPE_END HHG_TOKEN_VOID + 1
#define HHG_TOKEN_TYPE_COUNT (HHG_TOKEN_TYPE_END - HHG_TOKEN_TYPE_START)

typedef struct hhg_str hhg_str_t;

typedef enum hhg_token_type {
    // special tokens
    HHG_TOKEN_NONE,
    HHG_TOKEN_ID,
    HHG_TOKEN_EOF,
    HHG_TOKEN_NEWLINE,

    // arithmetic operators
    HHG_TOKEN_PLUS,
    HHG_TOKEN_MINUS,
    HHG_TOKEN_STAR,
    HHG_TOKEN_SLASH,
    HHG_TOKEN_PERCENT,

    // bitwise operators
    HHG_TOKEN_AMPERSAND,
    HHG_TOKEN_PIPE,
    HHG_TOKEN_CARET,
    HHG_TOKEN_TILDE,
    HHG_TOKEN_LSHIFT,
    HHG_TOKEN_RSHIFT,

    // assignment operators
    HHG_TOKEN_EQ,

    HHG_TOKEN_PLUS_EQ,
    HHG_TOKEN_MINUS_EQ,
    HHG_TOKEN_STAR_EQ,
    HHG_TOKEN_SLASH_EQ,
    HHG_TOKEN_PERCENT_EQ,
    HHG_TOKEN_AMPERSAND_EQ,
    HHG_TOKEN_PIPE_EQ,
    HHG_TOKEN_CARET_EQ,
    HHG_TOKEN_LSHIFT_EQ,
    HHG_TOKEN_RSHIFT_EQ,

    // comparison operators
    HHG_TOKEN_EQ_EQ,
    HHG_TOKEN_NOT_EQ,
    HHG_TOKEN_LT,
    HHG_TOKEN_LT_EQ,
    HHG_TOKEN_GT,
    HHG_TOKEN_GT_EQ,

    // punctuation
    HHG_TOKEN_ARROW,
    HHG_TOKEN_FAT_ARROW,

    HHG_TOKEN_DOT,
    HHG_TOKEN_DOT_DOT,
    HHG_TOKEN_DOT_DOT_EQ,

    HHG_TOKEN_QUESTION,
    HHG_TOKEN_BANG,
    HHG_TOKEN_COLON,
    HHG_TOKEN_AT,

    HHG_TOKEN_LPAREN,
    HHG_TOKEN_RPAREN,
    
    HHG_TOKEN_LBRACE,
    HHG_TOKEN_RBRACE,

    HHG_TOKEN_LBRACKET,
    HHG_TOKEN_RBRACKET,

    HHG_TOKEN_COMMA,
    
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

    HHG_TOKEN_VOID,
} hhg_token_type_t;

typedef struct hhg_token {
    hhg_token_type_t type;
    hhg_str_t str;
    hhg_file_range_t range;
} hhg_token_t;

void hhg_token_type_print(hhg_token_type_t type);
void hhg_token_type_fprint(hhg_token_type_t type, FILE *stream);
const char *hhg_token_type_to_str(hhg_token_type_t type);

void hhg_token_init(hhg_token_t *token);

void hhg_token_print(hhg_token_t *token);
void hhg_token_fprint(hhg_token_t *token, FILE *stream);

void hhg_token_del(hhg_token_t *token);

#endif
