#ifndef HHG_LEXER_H
#define HHG_LEXER_H

#include <stdio.h>
#include <stdbool.h>

#include "token.h"
#include "file_pos.h"

typedef struct hhg_msg_ctx hhg_msg_ctx_t;

typedef struct hhg_src {
    char *txt; // terminated with '\0' but EOF is used for tokens
    const char *filename;
    int32_t *line_starts;
} hhg_src_t;

typedef struct hhg_lexer {
    hhg_src_t src;

    int32_t txt_idx;
    int32_t end_idx; // index from the end of the text
    
    hhg_file_pos_t pos;
    hhg_file_pos_t last_pos;

    bool newline;

    hhg_token_t token;

    hhg_msg_ctx_t *msg_ctx;
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

void hhg_lexer_init(
    hhg_lexer_t *lexer,
    hhg_msg_ctx_t *msg_ctx,
    const char *filename
);

void hhg_lexer_next(hhg_lexer_t *lexer);

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type);
void hhg_lexer_match_va(
    hhg_lexer_t *lexer,
    const char *summary,
    int32_t count,
    ...
);

void hhg_lexer_del(hhg_lexer_t *lexer);

#endif
