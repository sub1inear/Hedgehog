#ifndef HHG_LEXER_H
#define HHG_LEXER_H

#include <stdio.h>
#include <stdbool.h>

#include "token.h"
#include "file_pos.h"


typedef struct hhg_lexer {
    char *text;
    int32_t text_idx;
    int32_t end_idx; // index from the end of the text

    const char *filename;

    hhg_file_pos_t pos;    
    long *line_starts;

    bool newline;
    hhg_token_t token;
} hhg_lexer_t;

typedef struct hhg_op_data {
    char str[3];
    hhg_token_type_t type;
    int32_t prec;
} hhg_op_data_t;

typedef struct hhg_keyword_data {
    char *str;
    hhg_token_type_t type;
} hhg_keyword_data_t;

void hhg_lexer_init(hhg_lexer_t *lexer, const char *filename);
void hhg_lexer_del(hhg_lexer_t *lexer);

void hhg_lexer_next(hhg_lexer_t *lexer);

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type);
void hhg_lexer_match_va(hhg_lexer_t *lexer, const char *summary, int32_t count, ...);

#endif
