#ifndef HHG_LEXER_H
#define HHG_LEXER_H

#include <stdio.h>
#include <stdbool.h>

#include "token.h"

typedef struct _hhg_lexer_t {
    FILE *file;
    hhg_token_t token;
    bool peeked;
} hhg_lexer_t;

typedef struct _hhg_op_data_t {
    char str[3];
    hhg_token_type_t type;
    int32_t prec;
} hhg_op_data_t;

typedef struct _hhg_keyword_data_t {
    char *str;
    hhg_token_type_t type;
} hhg_keyword_data_t;

void hhg_lexer_init(hhg_lexer_t *lexer, const char *filename);
void hhg_lexer_del(hhg_lexer_t *lexer);

void hhg_lexer_next(hhg_lexer_t *lexer);
void hhg_lexer_peek(hhg_lexer_t *lexer);

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type);
void hhg_lexer_match_va(hhg_lexer_t *lexer, ...);
void hhg_lexer_match_type(hhg_lexer_t *lexer);


#endif