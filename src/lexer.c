#include <ctype.h>
#include <stb_ds.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "file_pos.h"
#include "file_range.h"
#include "file_src.h"
#include "lexer.h"
#include "msg.h"
#include "str.h"
#include "utils.h"

#define hhg_lexer_msg(lexer, type, ...)                                        \
    do {                                                                       \
        hhg_msg(lexer->msg_ctx, type, &lexer->src, &lexer->token.range,        \
                __VA_ARGS__);                                                  \
        hhg_lexer_resync(lexer);                                               \
    } while (0)
#define hhg_lexer_error(lexer, ...)                                            \
    hhg_lexer_msg(lexer, HHG_MSG_ERROR, __VA_ARGS__)
#define hhg_lexer_warning(lexer, ...)                                          \
    hhg_lexer_msg(lexer, HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_lexer_info(lexer, ...)                                             \
    hhg_lexer_msg(lexer, HHG_MSG_INFO, __VA_ARGS__)

typedef struct hhg_op_data {
    char str[3];
    hhg_token_type_t type;
} hhg_op_data_t;

typedef struct hhg_keyword_data {
    char *str;
    hhg_token_type_t type;
} hhg_keyword_data_t;

static int hhg_lexer_next_char(hhg_lexer_t *lexer);
static void hhg_lexer_back_char(hhg_lexer_t *lexer);
static bool hhg_lexer_ends_stmt(hhg_token_type_t type);
static void hhg_lexer_lex_id(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_num(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_str_lit(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_char_lit(hhg_lexer_t *lexer, int c);
static bool hhg_lexer_lex_default(hhg_lexer_t *lexer, int c);

// clang-format off
static const hhg_op_data_t op_data[] = {
    // must go in order of length
    { { '<', '<',  '=',  },  HHG_TOKEN_LSHIFT_EQ,    },
    { { '>', '>',  '=',  },  HHG_TOKEN_RSHIFT_EQ,    },
    { { '.', '.',  '=',  },  HHG_TOKEN_DOT_DOT_EQ,   },
                                                     
    { { '<', '<',  '\0', },  HHG_TOKEN_LSHIFT,       },
    { { '>', '>',  '\0', },  HHG_TOKEN_RSHIFT,       },
    { { '+', '=',  '\0', },  HHG_TOKEN_PLUS_EQ,      },
    { { '-', '=',  '\0', },  HHG_TOKEN_MINUS_EQ,     },
    { { '*', '=',  '\0', },  HHG_TOKEN_STAR_EQ,      },
    { { '/', '=',  '\0', },  HHG_TOKEN_SLASH_EQ,     },
    { { '%', '=',  '\0', },  HHG_TOKEN_PERCENT_EQ,   },
    { { '&', '=',  '\0', },  HHG_TOKEN_AMPERSAND_EQ, },
    { { '|', '=',  '\0', },  HHG_TOKEN_PIPE_EQ,      },
    { { '^', '=',  '\0', },  HHG_TOKEN_CARET_EQ,     },
    { { '=', '=',  '\0', },  HHG_TOKEN_EQ_EQ,        },
    { { '!', '=',  '\0', },  HHG_TOKEN_NOT_EQ,       },
    { { '<', '=',  '\0', },  HHG_TOKEN_LT_EQ,        },
    { { '>', '=',  '\0', },  HHG_TOKEN_GT_EQ,        },
    { { '-', '>',  '\0', },  HHG_TOKEN_ARROW,        },
    { { '=', '>',  '\0', },  HHG_TOKEN_FAT_ARROW,    },
    { { '.', '.',  '\0', },  HHG_TOKEN_DOT_DOT,      },
                                                     
    { { '+', '\0', '\0', },  HHG_TOKEN_PLUS,         },
    { { '-', '\0', '\0', },  HHG_TOKEN_MINUS,        },
    { { '*', '\0', '\0', },  HHG_TOKEN_STAR,         },
    { { '/', '\0', '\0', },  HHG_TOKEN_SLASH,        },
    { { '%', '\0', '\0', },  HHG_TOKEN_PERCENT,      },
    { { '&', '\0', '\0', },  HHG_TOKEN_AMPERSAND,    },
    { { '|', '\0', '\0', },  HHG_TOKEN_PIPE,         },
    { { '^', '\0', '\0', },  HHG_TOKEN_CARET,        },
    { { '~', '\0', '\0', },  HHG_TOKEN_TILDE,        },
    { { '=', '\0', '\0', },  HHG_TOKEN_EQ,           },
    { { '<', '\0', '\0', },  HHG_TOKEN_LT,           },
    { { '>', '\0', '\0', },  HHG_TOKEN_GT,           },
    { { '.', '\0', '\0', },  HHG_TOKEN_DOT,          },
    { { '?', '\0', '\0', },  HHG_TOKEN_QUESTION,     },
    { { '!', '\0', '\0', },  HHG_TOKEN_BANG,         },
    { { ':', '\0', '\0', },  HHG_TOKEN_COLON,        },
    { { '@', '\0', '\0', },  HHG_TOKEN_AT,           },
    { { '(', '\0', '\0', },  HHG_TOKEN_LPAREN,       },
    { { ')', '\0', '\0', },  HHG_TOKEN_RPAREN,       },
    { { '{', '\0', '\0', },  HHG_TOKEN_LBRACE,       },
    { { '}', '\0', '\0', },  HHG_TOKEN_RBRACE,       },
    { { '[', '\0', '\0', },  HHG_TOKEN_LBRACKET,     },
    { { ']', '\0', '\0', },  HHG_TOKEN_RBRACKET,     },
    { { ',', '\0', '\0', },  HHG_TOKEN_COMMA,        },
};                                                   
                                                     
static const hhg_keyword_data_t keyword_data[] = {   
    { "let",                 HHG_TOKEN_LET,          },
    { "mut",                 HHG_TOKEN_MUT,          },
    { "const",               HHG_TOKEN_CONST,        },
                                                     
    { "fn",                  HHG_TOKEN_FN,           },
    { "class",               HHG_TOKEN_CLASS,        },
    { "enum",                HHG_TOKEN_ENUM,         },
    { "interface",           HHG_TOKEN_INTERFACE,    },                                                       
    { "type",                HHG_TOKEN_TYPE,         },
    { "import",              HHG_TOKEN_IMPORT,       },
    { "from",                HHG_TOKEN_FROM,         },
    { "as",                  HHG_TOKEN_AS,           },
                                                     
    { "if",                  HHG_TOKEN_IF,           },
    { "else",                HHG_TOKEN_ELSE,         },
    { "while",               HHG_TOKEN_WHILE,        },
    { "for",                 HHG_TOKEN_FOR,          },
    { "in",                  HHG_TOKEN_IN,           },
    { "match",               HHG_TOKEN_MATCH,        },
    { "break",               HHG_TOKEN_BREAK,        },
    { "return",              HHG_TOKEN_RETURN,       },
                                                     
    { "and",                 HHG_TOKEN_AND,          },
    { "or",                  HHG_TOKEN_OR,           },
    { "not",                 HHG_TOKEN_NOT,          },
                                                     
    { "true",                HHG_TOKEN_TRUE,         },
    { "false",               HHG_TOKEN_FALSE,        },
    { "null",                HHG_TOKEN_NULL,         },
    { "self",                HHG_TOKEN_SELF,         },
                                              
    { "static",              HHG_TOKEN_STATIC,       },
    { "unsafe",              HHG_TOKEN_UNSAFE,       },
    { "pub",                 HHG_TOKEN_PUB,          },
                                                     
    { "i8",                  HHG_TOKEN_I8,           },
    { "u8",                  HHG_TOKEN_U8,           },
                                                     
    { "i16",                 HHG_TOKEN_I16,          },
    { "u16",                 HHG_TOKEN_U16,          },
                                                     
    { "i32",                 HHG_TOKEN_I32,          },
    { "u32",                 HHG_TOKEN_U32,          },
                                                     
    { "i64",                 HHG_TOKEN_I64,          },
    { "u64",                 HHG_TOKEN_U64,          },
                                                     
    { "f32",                 HHG_TOKEN_F32,          },
    { "f64",                 HHG_TOKEN_F64,          },
                                                     
    { "bool",                HHG_TOKEN_BOOL,         },
    { "char",                HHG_TOKEN_CHAR,         },
                                                     
    { "isize",               HHG_TOKEN_ISIZE,        },
    { "usize",               HHG_TOKEN_USIZE,        },
                                                     
    { "void",                HHG_TOKEN_VOID,         },
};
// clang-format on

void hhg_lexer_init(hhg_lexer_t *lexer, hhg_msg_ctx_t *msg_ctx,
                    const char *filename)
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
    hhg_str_reset(&lexer->token.str);

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
            } else if (isdigit(c)) {
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
                if (hhg_lexer_ends_stmt(lexer->token.type)) {
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

void hhg_lexer_resync(hhg_lexer_t *lexer)
{
    do
        hhg_lexer_next(lexer);
    while (lexer->token.type != HHG_TOKEN_NEWLINE &&
           lexer->token.type != HHG_TOKEN_EOF);
    hhg_lexer_next(lexer);
}

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type)
{
    if (lexer->token.type != type) {
        hhg_lexer_error(lexer, "expected `%t`, got `%t`", "here", type,
                        lexer->token.type);
    } else
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
        lexer->pos.col = lexer->src.line_starts[lexer->pos.line] -
                         lexer->src.line_starts[lexer->pos.line - 1] - 1;
        lexer->pos.line--;
        HHG_UNUSED(arrpop(lexer->src.line_starts));
    } else
        lexer->pos.col--;
}

static bool hhg_lexer_ends_stmt(hhg_token_type_t type)
{
    switch (type) {
    case HHG_TOKEN_ID:
    case HHG_TOKEN_INT_LIT:
    case HHG_TOKEN_FLOAT_LIT:
    case HHG_TOKEN_CHAR_LIT:
    case HHG_TOKEN_STR_LIT:
    case HHG_TOKEN_RPAREN:
    case HHG_TOKEN_RBRACE:
    case HHG_TOKEN_RBRACKET:
    case HHG_TOKEN_BREAK:
    case HHG_TOKEN_CONTINUE:
    case HHG_TOKEN_RETURN:
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
    case HHG_TOKEN_NULL:
    case HHG_TOKEN_SELF:
    case HHG_TOKEN_I8:
    case HHG_TOKEN_U8:
    case HHG_TOKEN_I16:
    case HHG_TOKEN_U16:
    case HHG_TOKEN_I32:
    case HHG_TOKEN_U32:
    case HHG_TOKEN_I64:
    case HHG_TOKEN_U64:
    case HHG_TOKEN_F32:
    case HHG_TOKEN_F64:
    case HHG_TOKEN_BOOL:
    case HHG_TOKEN_CHAR:
    case HHG_TOKEN_ISIZE:
    case HHG_TOKEN_USIZE:
    case HHG_TOKEN_VOID:
        return true;
    default:
        return false;
    }
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
                hhg_msg(lexer->msg_ctx, HHG_MSG_ERROR, &lexer->src,
                        &(hhg_file_range_t){
                            .start =
                                (hhg_file_pos_t){
                                    .col = lexer->pos.col - 1,
                                    .line = lexer->pos.line,
                                },
                            .end =
                                (hhg_file_pos_t){
                                    .col = lexer->pos.col,
                                    .line = lexer->pos.line,
                                },
                        },
                        "unknown base prefix `%c`", NULL, c);
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
            hhg_lexer_error(lexer, "unexpected EOF in string literal", NULL);
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
        hhg_lexer_error(lexer, "character constant is too long", NULL);

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
                        lexer, "unexpected EOF in multi-line comment", NULL);
                    break;
                }
            } while (c2 != '*' || c != '/');
            return false;
        }
    }

    int c3 = hhg_lexer_next_char(lexer);

    for (size_t i = 0; i < HHG_ARR_LEN(op_data); i++) {
        const hhg_op_data_t *data = &op_data[i];
        if (data->str[0] == c && (data->str[1] == c2 || data->str[1] == '\0') &&
            (data->str[2] == c3 || data->str[2] == '\0')) {

            if (data->str[2] == c3)
                ; // consumed three chars, nothing to do
            else if (data->str[1] == c2)
                hhg_lexer_back_char(
                    lexer); // consumed two chars, give back last
            else {
                // consumed one char, give back two
                hhg_lexer_back_char(lexer);
                hhg_lexer_back_char(lexer);
            }
            lexer->token.type = data->type;
            return true;
        }
    }
    hhg_lexer_back_char(lexer);
    hhg_lexer_back_char(lexer);
    hhg_lexer_error(lexer, "unexpected character `%i`", "here", c);
    return false;
}
