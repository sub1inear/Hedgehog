#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stb_ds.h>

#include "lexer.h"
#include "file_src.h"
#include "file_pos.h"
#include "file_range.h"
#include "msg.h"
#include "str.h"
#include "utils.h"

#define hhg_lexer_msg(lexer, type, ...) \
    hhg_msg(                            \
        lexer->msg_ctx,                 \
        type,                           \
        &lexer->src,                    \
        &lexer->token.range,            \
        __VA_ARGS__                     \
    )

#define hhg_lexer_error(lexer, ...) \
    hhg_lexer_msg(lexer, HHG_MSG_ERROR, __VA_ARGS__) 
#define hhg_lexer_warning(lexer, ...) \
    hhg_lexer_msg(lexer, HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_lexer_info(lexer, ...) \
    hhg_lexer_msg(lexer, HHG_MSG_INFO, __VA_ARGS__)

typedef struct hhg_lexer_op_data {
    char str[3];
    hhg_token_type_t type;
    int32_t prec;
} hhg_lexer_op_data_t;

typedef struct hhg_lexer_keyword_data {
    char *str;
    hhg_token_type_t type;
} hhg_lexer_keyword_data_t;

static int hhg_lexer_next_char(hhg_lexer_t *lexer);
static void hhg_lexer_back_char(hhg_lexer_t *lexer);
static void hhg_lexer_lex_id(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_num(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_str_lit(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_char_lit(hhg_lexer_t *lexer, int c);
static bool hhg_lexer_lex_default(hhg_lexer_t *lexer, int c);

static const hhg_lexer_op_data_t op_data[] = {
    // must go in order of length
    { { '<', '<',  '=',  },  HHG_TOKEN_LSHIFT_EQ,    HHG_PREC_NONE, },
    { { '>', '>',  '=',  },  HHG_TOKEN_RSHIFT_EQ,    HHG_PREC_NONE, },
    { { '.', '.',  '=',  },  HHG_TOKEN_DOT_DOT_EQ,   HHG_PREC_NONE, },

    { { '<', '<',  '\0', },  HHG_TOKEN_LSHIFT,       8,             },
    { { '>', '>',  '\0', },  HHG_TOKEN_RSHIFT,       8,             },
    { { '+', '=',  '\0', },  HHG_TOKEN_PLUS_EQ,      HHG_PREC_NONE, },
    { { '-', '=',  '\0', },  HHG_TOKEN_MINUS_EQ,     HHG_PREC_NONE, },
    { { '*', '=',  '\0', },  HHG_TOKEN_STAR_EQ,      HHG_PREC_NONE, },
    { { '/', '=',  '\0', },  HHG_TOKEN_SLASH_EQ,     HHG_PREC_NONE, },
    { { '%', '=',  '\0', },  HHG_TOKEN_PERCENT_EQ,   HHG_PREC_NONE, },
    { { '&', '=',  '\0', },  HHG_TOKEN_AMPERSAND_EQ, HHG_PREC_NONE, },
    { { '|', '=',  '\0', },  HHG_TOKEN_PIPE_EQ,      HHG_PREC_NONE, },
    { { '^', '=',  '\0', },  HHG_TOKEN_CARET_EQ,     HHG_PREC_NONE, },
    { { '=', '=',  '\0', },  HHG_TOKEN_EQ_EQ,        6,             },
    { { '!', '=',  '\0', },  HHG_TOKEN_NOT_EQ,       6,             },
    { { '<', '=',  '\0', },  HHG_TOKEN_LT_EQ,        7,             },
    { { '>', '=',  '\0', },  HHG_TOKEN_GT_EQ,        7,             },
    { { '-', '>',  '\0', },  HHG_TOKEN_ARROW,        HHG_PREC_NONE, },
    { { '=', '>',  '\0', },  HHG_TOKEN_FAT_ARROW,    HHG_PREC_NONE, },
    { { '.', '.',  '\0', },  HHG_TOKEN_DOT_DOT,      HHG_PREC_NONE, },

    { { '+', '\0', '\0', },  HHG_TOKEN_PLUS,         9,             },
    { { '-', '\0', '\0', },  HHG_TOKEN_MINUS,        9,             },
    { { '*', '\0', '\0', },  HHG_TOKEN_STAR,         10,            },
    { { '/', '\0', '\0', },  HHG_TOKEN_SLASH,        10,            },
    { { '%', '\0', '\0', },  HHG_TOKEN_PERCENT,      10,            },
    { { '&', '\0', '\0', },  HHG_TOKEN_AMPERSAND,    5,             },
    { { '|', '\0', '\0', },  HHG_TOKEN_PIPE,         3,             },
    { { '^', '\0', '\0', },  HHG_TOKEN_CARET,        4,             },
    { { '~', '\0', '\0', },  HHG_TOKEN_TILDE,        HHG_PREC_NONE, },
    { { '=', '\0', '\0', },  HHG_TOKEN_EQ,           HHG_PREC_NONE, },
    { { '<', '\0', '\0', },  HHG_TOKEN_LT,           7,             },
    { { '>', '\0', '\0', },  HHG_TOKEN_GT,           7,             },
    { { '.', '\0', '\0', },  HHG_TOKEN_DOT,          HHG_PREC_NONE, },
    { { '?', '\0', '\0', },  HHG_TOKEN_QUESTION,     HHG_PREC_NONE, },
    { { '!', '\0', '\0', },  HHG_TOKEN_BANG,         HHG_PREC_NONE, },
    { { ':', '\0', '\0', },  HHG_TOKEN_COLON,        HHG_PREC_NONE, },
    { { '@', '\0', '\0', },  HHG_TOKEN_AT,           HHG_PREC_NONE, },
    { { '(', '\0', '\0', },  HHG_TOKEN_LPAREN,       HHG_PREC_NONE, },
    { { ')', '\0', '\0', },  HHG_TOKEN_RPAREN,       HHG_PREC_NONE, },
    { { '{', '\0', '\0', },  HHG_TOKEN_LBRACE,       HHG_PREC_NONE, },
    { { '}', '\0', '\0', },  HHG_TOKEN_RBRACE,       HHG_PREC_NONE, },
    { { '[', '\0', '\0', },  HHG_TOKEN_LBRACKET,     HHG_PREC_NONE, },
    { { ']', '\0', '\0', },  HHG_TOKEN_RBRACKET,     HHG_PREC_NONE, },
    { { ',', '\0', '\0', },  HHG_TOKEN_COMMA,        HHG_PREC_NONE, },
};
static const hhg_lexer_keyword_data_t keyword_data[] = {
    { "let",                 HHG_TOKEN_LET,                         },
    { "mut",                 HHG_TOKEN_MUT,                         },
    { "const",               HHG_TOKEN_CONST,                       },
                                                                   
    { "fn",                  HHG_TOKEN_FN,                          },
    { "class",               HHG_TOKEN_CLASS,                       },
    { "enum",                HHG_TOKEN_ENUM,                        },
    { "interface",           HHG_TOKEN_INTERFACE,                   },
                                                                   
    { "if",                  HHG_TOKEN_IF,                          },
    { "while",               HHG_TOKEN_WHILE,                       },
    { "for",                 HHG_TOKEN_FOR,                         },
    { "type",                HHG_TOKEN_TYPE,                        },
    { "import",              HHG_TOKEN_IMPORT,                      },
    { "from",                HHG_TOKEN_FROM,                        },
    { "as",                  HHG_TOKEN_AS,                          },
                                                                   
    { "if",                  HHG_TOKEN_IF,                          },
    { "else",                HHG_TOKEN_ELSE,                        },
    { "while",               HHG_TOKEN_WHILE,                       },
    { "for",                 HHG_TOKEN_FOR,                         },
    { "in",                  HHG_TOKEN_IN,                          },
    { "match",               HHG_TOKEN_MATCH,                       },
    { "break",               HHG_TOKEN_BREAK,                       },
    { "return",              HHG_TOKEN_RETURN,                      },
                                                                   
    { "and",                 HHG_TOKEN_AND,                         },
    { "or",                  HHG_TOKEN_OR,                          },
    { "not",                 HHG_TOKEN_NOT,                         },
                                                                    
    { "true",                HHG_TOKEN_TRUE,                        },
    { "false",               HHG_TOKEN_FALSE,                       },
    { "null",                HHG_TOKEN_NULL,                        },
    { "self",                HHG_TOKEN_SELF,                        },
                                                                    
    { "static",              HHG_TOKEN_STATIC,                      },
    { "unsafe",              HHG_TOKEN_UNSAFE,                      },
    { "pub",                 HHG_TOKEN_PUB,                         },
                                                                    
    { "i8",                  HHG_TOKEN_I8,                          },
    { "u8",                  HHG_TOKEN_U8,                          },
                                                                    
    { "i16",                 HHG_TOKEN_I16,                         },
    { "u16",                 HHG_TOKEN_U16,                         },
                                                                     
    { "i32",                 HHG_TOKEN_I32,                         },
    { "u32",                 HHG_TOKEN_U32,                         },
                                                                      
    { "i64",                 HHG_TOKEN_I64,                         },
    { "u64",                 HHG_TOKEN_U64,                         },
                                                                                                            
    { "f32",                 HHG_TOKEN_F32,                         },
    { "f64",                 HHG_TOKEN_F64,                         },
                                                                   
    { "bool",                HHG_TOKEN_BOOL,                        },
    { "char",                HHG_TOKEN_CHAR,                        },
                                                                   
    { "isize",               HHG_TOKEN_ISIZE,                       },
    { "usize",               HHG_TOKEN_USIZE,                       },
};

static const bool newline_data[] = {
    // special tokens
    [HHG_TOKEN_NONE] = false,
    [HHG_TOKEN_ID] = true,
    [HHG_TOKEN_EOF] = false,
    [HHG_TOKEN_NEWLINE] = false,

    // arithmetic operators
    [HHG_TOKEN_PLUS] = false,
    [HHG_TOKEN_MINUS] = false,
    [HHG_TOKEN_STAR] = false,
    [HHG_TOKEN_SLASH] = false,
    [HHG_TOKEN_PERCENT] = false,
    
    // bitwise operators
    [HHG_TOKEN_AMPERSAND] = false,
    [HHG_TOKEN_PIPE] = false,
    [HHG_TOKEN_CARET] = false,
    [HHG_TOKEN_TILDE] = false,
    [HHG_TOKEN_LSHIFT] = false,
    [HHG_TOKEN_RSHIFT] = false,

    // assignment operators
    [HHG_TOKEN_EQ] = false,
    [HHG_TOKEN_PLUS_EQ] = false,
    [HHG_TOKEN_MINUS_EQ] = false,
    [HHG_TOKEN_STAR_EQ] = false,
    [HHG_TOKEN_SLASH_EQ] = false,
    [HHG_TOKEN_PERCENT_EQ] = false,
    [HHG_TOKEN_AMPERSAND_EQ] = false,
    [HHG_TOKEN_PIPE_EQ] = false,
    [HHG_TOKEN_CARET_EQ] = false,
    [HHG_TOKEN_LSHIFT_EQ] = false,
    [HHG_TOKEN_RSHIFT_EQ] = false,
    
    // comparison operators
    [HHG_TOKEN_EQ_EQ] = false,
    [HHG_TOKEN_NOT_EQ] = false,
    [HHG_TOKEN_LT] = false,
    [HHG_TOKEN_LT_EQ] = false,
    [HHG_TOKEN_GT] = false,
    [HHG_TOKEN_GT_EQ] = false,

    // punctuation
    [HHG_TOKEN_ARROW] = false,
    [HHG_TOKEN_FAT_ARROW] = false,

    [HHG_TOKEN_DOT] = false,
    [HHG_TOKEN_DOT_DOT] = false,
    [HHG_TOKEN_DOT_DOT_EQ] = false,
    [HHG_TOKEN_QUESTION] = false,
    [HHG_TOKEN_BANG] = false,
    [HHG_TOKEN_COLON] = false,
    [HHG_TOKEN_AT] = false,

    [HHG_TOKEN_LPAREN] = false,
    [HHG_TOKEN_RPAREN] = true,

    [HHG_TOKEN_LBRACE] = false,
    [HHG_TOKEN_RBRACE] = true,

    [HHG_TOKEN_LBRACKET] = false,
    [HHG_TOKEN_RBRACKET] = true,

    [HHG_TOKEN_COMMA] = false,

    // literals
    [HHG_TOKEN_INT_LIT] = true,
    [HHG_TOKEN_FLOAT_LIT] = true,
    [HHG_TOKEN_CHAR_LIT] = true,
    [HHG_TOKEN_STR_LIT] = true,

    // keywords
    [HHG_TOKEN_LET] = false,
    [HHG_TOKEN_MUT] = false,
    [HHG_TOKEN_CONST] = false,

    [HHG_TOKEN_FN] = false,
    [HHG_TOKEN_CLASS] = false,
    [HHG_TOKEN_ENUM] = false,
    [HHG_TOKEN_INTERFACE] = false,
    [HHG_TOKEN_TYPE] = false,
    [HHG_TOKEN_IMPORT] = false,
    [HHG_TOKEN_FROM] = false,
    [HHG_TOKEN_AS] = false,

    [HHG_TOKEN_IF] = false,
    [HHG_TOKEN_ELSE] = false,
    [HHG_TOKEN_WHILE] = false,
    [HHG_TOKEN_FOR] = false,
    [HHG_TOKEN_IN] = false,
    [HHG_TOKEN_MATCH] = false,
    [HHG_TOKEN_BREAK] = true,
    [HHG_TOKEN_CONTINUE] = true,
    [HHG_TOKEN_RETURN] = true,

    [HHG_TOKEN_AND] = false,
    [HHG_TOKEN_OR] = false,
    [HHG_TOKEN_NOT] = false,

    [HHG_TOKEN_TRUE] = true,
    [HHG_TOKEN_FALSE] = true,
    [HHG_TOKEN_NULL] = true,
    [HHG_TOKEN_SELF] = true,

    [HHG_TOKEN_STATIC] = false,
    [HHG_TOKEN_UNSAFE] = false,
    [HHG_TOKEN_PUB] = false,

    // types
    [HHG_TOKEN_I8] = true,
    [HHG_TOKEN_U8] = true,

    [HHG_TOKEN_I16] = true,
    [HHG_TOKEN_U16] = true,
    [HHG_TOKEN_I32] = true,
    [HHG_TOKEN_U32] = true,

    [HHG_TOKEN_I64] = true,
    [HHG_TOKEN_U64] = true,
    [HHG_TOKEN_F32] = true,
    [HHG_TOKEN_F64] = true,

    [HHG_TOKEN_BOOL] = true,

    [HHG_TOKEN_CHAR] = true,

    [HHG_TOKEN_ISIZE] = true,
    [HHG_TOKEN_USIZE] = true,
};

void hhg_lexer_init(
    hhg_lexer_t *lexer,
    hhg_msg_ctx_t *msg_ctx,
    const char *filename
)
{
    hhg_file_src_init(&lexer->src, filename);

    lexer->txt_idx = 0;
    lexer->end_idx = 0;

    hhg_file_pos_init(&lexer->pos);
    hhg_file_pos_init(&lexer->last_pos);

    lexer->msg_ctx = msg_ctx;

    hhg_token_init(&lexer->token);
}

void hhg_lexer_next(hhg_lexer_t *lexer)
{
    hhg_token_reset_aux(&lexer->token);
    while (true) {
        int c = hhg_lexer_next_char(lexer);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v')
            ;
        else {
            // past whitespace, start lexing token
            lexer->token.range.start = (hhg_file_pos_t){
                .line = lexer->pos.line,
                // first character has already been consumed so decrement
                // exception: EOF is not consumed
                .col = c == EOF ? lexer->pos.col : lexer->pos.col - 1,
            };
            if (isalpha(c) || c == '_') {
                hhg_lexer_lex_id(lexer, c);
                break;
            } else if (isdigit(c))  {
                hhg_lexer_lex_num(lexer, c);
                break;
            } else if (c == '"') {
                hhg_lexer_lex_str_lit(lexer, c);
                break;
            } else if (c == '\'') {
                hhg_lexer_lex_char_lit(lexer, c);
                break;
            } else if (c == EOF) {
                lexer->token.type = HHG_TOKEN_EOF;
                break;
            } else if (c == '\n') {
                if (newline_data[lexer->token.type]) {
                    lexer->token.type = HHG_TOKEN_NEWLINE;
                    break;
                } else
                    continue;
            } else if (hhg_lexer_lex_default(lexer, c))
                break;
        }
    }
    lexer->last_pos = lexer->token.range.end;
    lexer->token.range.end = lexer->pos;
}

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type)
{
    if (lexer->token.type != type)
        hhg_lexer_error(
            lexer,
            "expected `%t`, got `%t`",
            "here",
            type,
            lexer->token.type
        );

    hhg_lexer_next(lexer);
}

void hhg_lexer_del(hhg_lexer_t *lexer)
{
    hhg_token_del(&lexer->token);
    hhg_file_src_del(&lexer->src);
}

static int hhg_lexer_next_char(hhg_lexer_t *lexer)
{
    char c = lexer->src.txt[lexer->txt_idx];
    if (c == '\0') {
        // '\0' (returns EOF) should not be consumed
        // end_idx allows backing up from EOF to get the last character
        lexer->end_idx++;
        return EOF;
    } else {
        lexer->txt_idx++;
        if (c == '\n') {
            lexer->pos.col = 0;
            lexer->pos.line++;
            // store the line start for errors and hhg_lexer_back_char
            arrput(lexer->src.line_starts, lexer->txt_idx);
        } else
            lexer->pos.col++;
    }
    return c;
}

static void hhg_lexer_back_char(hhg_lexer_t *lexer)
{
    // end_idx counts characters past EOF
    // if end_idx > 0, back up from EOF without changing pos or txt_idx
    if (lexer->end_idx > 0) {
        lexer->end_idx--;
        return;
    }

    lexer->txt_idx--;

    if (lexer->pos.col == 0) {
        // backing up from the start of a line
        // set col to the end of the previous line
        lexer->pos.col = 
            lexer->src.line_starts[lexer->pos.line] -
            lexer->src.line_starts[lexer->pos.line - 1] - 1;
        lexer->pos.line--;
        HHG_UNUSED(arrpop(lexer->src.line_starts));
    } else
        lexer->pos.col--;
}

static void hhg_lexer_lex_id(hhg_lexer_t *lexer, int c)
{
    do {
        hhg_str_append_char(&lexer->token.str, c);
        c = hhg_lexer_next_char(lexer);
    } while (isalnum(c) || c == '_');

    hhg_lexer_back_char(lexer);

    lexer->token.type = HHG_TOKEN_ID;

    for (size_t i = 0; i < HHG_ARR_LEN(keyword_data); i++)
        if (!strcmp(keyword_data[i].str, lexer->token.str.str)) {
            lexer->token.type = keyword_data[i].type;
            return;
        }
}

static void hhg_lexer_lex_num(hhg_lexer_t *lexer, int c)
{
    lexer->token.type = HHG_TOKEN_INT_LIT;

    if (c == '0') {
        c = hhg_lexer_next_char(lexer);
        switch (c) {
        case 'x':
        case 'b':
            hhg_str_append_fmt(&lexer->token.str, "0%c", c);
            break;
        default:
            if (isalpha(c)) {
                hhg_msg(
                    lexer->msg_ctx,
                    HHG_MSG_ERROR,
                    &lexer->src,
                    &(hhg_file_range_t) {
                        .start = (hhg_file_pos_t) {
                            .col = lexer->pos.col - 1,
                            .line = lexer->pos.line,
                        },
                        .end = (hhg_file_pos_t) {
                            .col = lexer->pos.col,
                            .line = lexer->pos.line,
                        },
                    },
                    "unknown base prefix `%c`",
                    NULL,
                    c
                );
                do
                    c = hhg_lexer_next_char(lexer);
                while (isdigit(c));
                hhg_lexer_back_char(lexer);
                return;
            }
            hhg_str_append_char(&lexer->token.str, '0');
            if (isdigit(c))
                break; // octal literal, continue
            else {
                hhg_lexer_back_char(lexer);
                return; // single zero literal, done
            }
        }
    }

    bool seen_decimal = false;
    do {
        int next_c = hhg_lexer_next_char(lexer);
        if (next_c == '.') {
            if (seen_decimal)
                break;

            int next_next_c = hhg_lexer_next_char(lexer);
            hhg_lexer_back_char(lexer);

            if (isdigit(next_next_c))
                seen_decimal = true;
            else
                break;
        }

        hhg_str_append_char(&lexer->token.str, c);
        c = next_c;
    } while (isdigit(c) || c == '.');

    hhg_lexer_back_char(lexer);

    if (seen_decimal)
        lexer->token.type = HHG_TOKEN_FLOAT_LIT;

    return;
}

static void hhg_lexer_lex_str_lit(hhg_lexer_t *lexer, int c)
{
    hhg_str_append_char(&lexer->token.str, c);
    while (true) {
        c = hhg_lexer_next_char(lexer);
        // not using switch to break out of loop
        if (c == EOF) {
            hhg_lexer_error(
                lexer,
                "unexpected EOF in string lit",
                NULL
            );
            break;
        } else if (c == '\\') {
            hhg_str_append_char(&lexer->token.str, '\\');
            c = hhg_lexer_next_char(lexer);
        } else if (c == '"') {
            hhg_str_append_char(&lexer->token.str, '"');
            break;
        } else if (c == '\n') {
            hhg_str_append_str(&lexer->token.str, "\\n");
            continue;
        }
        hhg_str_append_char(&lexer->token.str, c);
    }
    lexer->token.type = HHG_TOKEN_STR_LIT;
}

static void hhg_lexer_lex_char_lit(hhg_lexer_t *lexer, int c)
{
    hhg_str_append_char(&lexer->token.str, c);
    c = hhg_lexer_next_char(lexer);
    hhg_str_append_char(&lexer->token.str, c);

    if (c == '\\') {
        c = hhg_lexer_next_char(lexer);
        hhg_str_append_char(&lexer->token.str, c);
    }

    c = hhg_lexer_next_char(lexer);

    if (c == '\'')
        hhg_str_append_char(&lexer->token.str, c);
    else
        hhg_lexer_error(
            lexer,
            "character constant is too long",
            NULL
        );

    lexer->token.type = HHG_TOKEN_CHAR_LIT;
}

static bool hhg_lexer_lex_default(hhg_lexer_t *lexer, int c)
{
    int c2 = hhg_lexer_next_char(lexer);

    if (c == '/') {
        if (c2 == '/') { // single-line comment
            do
                c = hhg_lexer_next_char(lexer);
            while (c != '\n' && c != EOF);
            hhg_lexer_back_char(lexer);

            return false;
        } else if (c2 == '*') { // multi-line comment
            do {
                c2 = c;
                c = hhg_lexer_next_char(lexer);
                if (c == EOF) {
                    hhg_lexer_error(
                        lexer,
                        "unexpected EOF in multi-line comment",
                        NULL
                    );
                    break;
                }
            } while (c2 != '*' || c != '/');
            return false;
        }
    }

    int c3 = hhg_lexer_next_char(lexer);

    for (size_t i = 0; i < HHG_ARR_LEN(op_data); i++) {
        const hhg_lexer_op_data_t *data = &op_data[i];
        if (data->str[0] == c && 
            (data->str[1] == c2 || data->str[1] == '\0') &&
            (data->str[2] == c3 || data->str[2] == '\0')) {
            
            if (data->str[2] == c3)
                ; // consumed three chars, nothing to do
            else if (data->str[1] == c2)
                hhg_lexer_back_char(lexer); // consumed two chars, give back last
            else {
                // consumed one char, give back two
                hhg_lexer_back_char(lexer);
                hhg_lexer_back_char(lexer);
            }
            lexer->token.type = data->type;
            lexer->token.prec = data->prec;

            return true;
        }
    }
    hhg_lexer_back_char(lexer);
    hhg_lexer_back_char(lexer);
    hhg_lexer_error(
        lexer,
        "unexpected character `%i`",
        "here",
        c
    );
    return false;
}
