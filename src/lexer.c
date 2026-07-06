#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#include <stb_ds.h>

#include "lexer.h"
#include "file_src.h"
#include "file_pos.h"
#include "file_range.h"
#include "msg.h"
#include "utils.h"
#include "mem.h"

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

static int hhg_lexer_next_char(hhg_lexer_t *lexer);
static void hhg_lexer_back_char(hhg_lexer_t *lexer);
static void hhg_lexer_lex_id(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_num(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_str_literal(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_char_literal(hhg_lexer_t *lexer, int c);
static bool hhg_lexer_lex_default(hhg_lexer_t *lexer, int c);

static const hhg_op_data_t op_data[] = {                                         
    { { '<' , '<' , '='  },  HHG_TOKEN_LSHIFT_EQ, 1  },
    { { '>' , '>' , '='  },  HHG_TOKEN_RSHIFT_EQ, 1  },
                                                  
    { { '+' , '=' , '\0' },  HHG_TOKEN_ADD_EQ,    1  },
    { { '-' , '=' , '\0' },  HHG_TOKEN_SUB_EQ,    1  },
    { { '*' , '=' , '\0' },  HHG_TOKEN_MUL_EQ,    1  },
    { { '/' , '=' , '\0' },  HHG_TOKEN_DIV_EQ,    1  },
    { { '%' , '=' , '\0' },  HHG_TOKEN_MOD_EQ,    1  },
    { { '&' , '=' , '\0' },  HHG_TOKEN_AND_EQ,    1  },
    { { '|' , '=' , '\0' },  HHG_TOKEN_OR_EQ,     1  },
    { { '^' , '=' , '\0' },  HHG_TOKEN_XOR_EQ,    1  },
                                                  
    { { '<' , '<' , '\0' },  HHG_TOKEN_LSHIFT,    10 },
    { { '>' , '>' , '\0' },  HHG_TOKEN_RSHIFT,    10 },
                                                  
    { { '+' , '+' , '\0' },  HHG_TOKEN_INC,       13 },
    { { '-' , '-' , '\0' },  HHG_TOKEN_DEC,       13 },
                                                  
    { { '<' , '=' , '\0' },  HHG_TOKEN_LT_EQ,     9  },
    { { '>' , '=' , '\0' },  HHG_TOKEN_GT_EQ,     9  },
                                                  
    { { '=' , '=' , '\0' },  HHG_TOKEN_EQ,        8  },
    { { '!' , '=' , '\0' },  HHG_TOKEN_NOT_EQ,    8  },
                                                  
    { { '*' , '\0', '\0' },  '*',                 12 },
    { { '/' , '\0', '\0' },  '/',                 12 },
    { { '%' , '\0', '\0' },  '%',                 12 },
                                                  
    { { '+' , '\0', '\0' },  '+',                 11 },
    { { '-' , '\0', '\0' },  '-',                 11 },
                                                  
    { { '<' , '\0', '\0' },  '<',                 9  },
    { { '>' , '\0', '\0' },  '>',                 9  },
                                                  
    { { '&' , '\0', '\0' },  '&',                 7  },
    { { '^' , '\0', '\0' },  '^',                 6  },
    { { '|' , '\0', '\0' },  '|',                 5  },
};

static const hhg_keyword_data_t keyword_data[] = {
    { "if",       HHG_TOKEN_IF       },
    { "while",    HHG_TOKEN_WHILE    },
    { "for",      HHG_TOKEN_FOR      },
      
    { "break",    HHG_TOKEN_BREAK    },
    { "continue", HHG_TOKEN_CONTINUE },
      
    { "and",      HHG_TOKEN_AND      },
    { "or",       HHG_TOKEN_OR       },
    { "not",      HHG_TOKEN_NOT      },
      
    { "true",     HHG_TOKEN_TRUE     },
    { "false",    HHG_TOKEN_FALSE    },
      
    { "in",       HHG_TOKEN_IN       },
    { "range",    HHG_TOKEN_RANGE    },
      
    { "enum",     HHG_TOKEN_ENUM     },
      
    { "def",      HHG_TOKEN_DEF      },
    { "return",   HHG_TOKEN_RETURN   },

    { "class",    HHG_TOKEN_CLASS    },
    
    // ----------------------------- //

    { "i8",       HHG_TOKEN_I8       },
    { "u8",       HHG_TOKEN_U8       },

    { "i16",      HHG_TOKEN_I16      },
    { "u16",      HHG_TOKEN_U16      },
                                        
    { "i32",      HHG_TOKEN_I32      },
    { "u32",      HHG_TOKEN_U32      },
                                        
    { "i64",      HHG_TOKEN_I64      },
    { "u64",      HHG_TOKEN_U64      },
                                        
    { "int",      HHG_TOKEN_INT      },
                                        
    { "f32",      HHG_TOKEN_F32      },
    { "f64",      HHG_TOKEN_F64      },
      
    { "float",    HHG_TOKEN_FLOAT    },
      
    { "bool",     HHG_TOKEN_BOOL     },
    { "char",     HHG_TOKEN_CHAR     },
      
    { "isize",    HHG_TOKEN_ISIZE    },
    { "usize",    HHG_TOKEN_USIZE    },

    // ----------------------------- //
    
    { "const",    HHG_TOKEN_CONST    },
    { "volatile", HHG_TOKEN_VOLATILE },
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

    lexer->newline = false;

    lexer->msg_ctx = msg_ctx;

    hhg_token_init(&lexer->token);
}

void hhg_lexer_next(hhg_lexer_t *lexer)
{
    lexer->newline = false;
    hhg_token_reset_aux(&lexer->token);
    while (true) {
        int c = hhg_lexer_next_char(lexer);
        if (c == ' ' || c == '\t' || c == '\r')
            ;
        else if (c == '\n')
            lexer->newline = true;
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
                hhg_lexer_lex_str_literal(lexer, c);
                break;
            } else if (c == '\'') {
                hhg_lexer_lex_char_literal(lexer, c);
                break;
            } else if (c == EOF) {
                lexer->token.type = EOF;
                break;
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
    bool seen_decimal = false;
    do {
        if (c == '.')
            seen_decimal = true;
        hhg_str_append_char(&lexer->token.str, c);
        c = hhg_lexer_next_char(lexer);
    } while (isdigit(c) || (c == '.' && !seen_decimal));

    hhg_lexer_back_char(lexer);

    lexer->token.type = HHG_TOKEN_INT_LITERAL;

    if (seen_decimal)
        lexer->token.type = HHG_TOKEN_FLOAT_LITERAL;

    return;
}

static void hhg_lexer_lex_str_literal(hhg_lexer_t *lexer, int c)
{
    hhg_str_append_char(&lexer->token.str, c);
    while (true) {
        c = hhg_lexer_next_char(lexer);
        // not using switch to break out of loop
        if (c == EOF) {
            hhg_lexer_error(
                lexer,
                "unexpected EOF in string literal",
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
    lexer->token.type = HHG_TOKEN_STRING_LITERAL;
}

static void hhg_lexer_lex_char_literal(hhg_lexer_t *lexer, int c)
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

    lexer->token.type = HHG_TOKEN_CHAR_LITERAL;
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
        const hhg_op_data_t *data = &op_data[i];
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
    
    lexer->token.type = c;
    hhg_lexer_back_char(lexer);
    hhg_lexer_back_char(lexer);
    return true;
}
