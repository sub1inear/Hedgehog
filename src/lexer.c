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
#include "file_pos.h"
#include "msg.h"
#include "utils.h"
#include "mem.h"

#define hhg_lexer_error(...) hhg_error(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_lexer_warning(...) hhg_warning(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_lexer_info(...) hhg_info(lexer->pos, lexer->filename, __VA_ARGS__)

typedef struct hhg_file_pos_save {
    hhg_file_pos_t pos;
    long fpos;
} hhg_file_pos_save_t;

static const hhg_op_data_t op_data[] = {                                         
    { { '<' , '<' , '='  },  HHG_TOKEN_LSHIFT_EQ, 1  },
    { { '>' , '>' , '='  },  HHG_TOKEN_RSHIFT_EQ, 1  },
                                                  
    { { '+' , '=' , '\0' },  HHG_TOKEN_PLUS_EQ,   1  },
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
      
    { "time_t",   HHG_TOKEN_TIME_T   },

    // ----------------------------- //
    
    { "const",    HHG_TOKEN_CONST    },
    { "volatile", HHG_TOKEN_VOLATILE },
};

static void hhg_lexer_save_pos(hhg_lexer_t *lexer,
                               hhg_file_pos_save_t *save);
static void hhg_lexer_restore_pos(hhg_lexer_t *lexer,
                                  hhg_file_pos_save_t *save);

static int hhg_lexer_next_char(hhg_lexer_t *lexer);
static void hhg_lexer_back_char(hhg_lexer_t *lexer);
static void hhg_lexer_lex_id(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_num(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_str_literal(hhg_lexer_t *lexer, int c);
static void hhg_lexer_lex_char_literal(hhg_lexer_t *lexer, int c);
static bool hhg_lexer_lex_default(hhg_lexer_t *lexer, int c);

void hhg_lexer_init(hhg_lexer_t *lexer, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        hhg_fatal_error("%s: %s", filename, strerror(errno));

    // read file into lexer->text
    int seek = fseek(file, 0, SEEK_END);
    if (seek != 0)
        hhg_fatal_error("seeking in %s: %s", filename, strerror(errno));
    
    long fsize = ftell(file);
    if (fsize == -1)
        hhg_fatal_error(
            "getting file position in %s: %s",
            filename,
            strerror(errno)
        );

    rewind(file);

    lexer->text = hhg_malloc(fsize + 1); // leave space for null char
    
    size_t read = fread(lexer->text, sizeof(char), fsize, file);
    if (ferror(file) && read != fsize) // check for read error, not just EOF
        hhg_fatal_error("reading %s: %s", filename, strerror(errno));

    lexer->text[fsize] = '\0';

    lexer->text_idx = 0;
    lexer->end_idx = 0;

    lexer->filename = filename;

    hhg_file_pos_init(&lexer->pos);
    
    lexer->line_starts = NULL;
    arrput(lexer->line_starts, 0);

    lexer->newline = false;

    hhg_token_init(&lexer->token);
}

void hhg_lexer_next(hhg_lexer_t *lexer)
{
    hhg_token_reset_aux(&lexer->token);
    lexer->newline = false;
    while (true) {
        int c = hhg_lexer_next_char(lexer);
        if (c == ' ' || c == '\t' || c == '\r')
            ;
        else if (c == '\n')
            lexer->newline = true;
        else if (isalpha(c) || c == '_') {
            hhg_lexer_lex_id(lexer, c);
            return;
        } else if (isdigit(c))  {
            hhg_lexer_lex_num(lexer, c);
            return;
        } else if (c == '"') {
            hhg_lexer_lex_str_literal(lexer, c);
            return;
        } else if (c == '\'') {
            hhg_lexer_lex_char_literal(lexer, c);
            return;
        } else if (c == EOF) {
            lexer->token.type = EOF;
            return;
        } else if (hhg_lexer_lex_default(lexer, c))
            return;
    }
}

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type)
{
    if (lexer->token.type != type)
        hhg_lexer_error(
            "expected \"%s\", got \"%s\"",
            hhg_token_type_to_str(type),
            hhg_token_type_to_str(lexer->token.type)
        );    

    hhg_lexer_next(lexer);
}

void hhg_lexer_match_va(hhg_lexer_t *lexer, const char *summary, int32_t count, ...)
{
    va_list va;
    va_start(va, count);

    int32_t i;

    for (i = 0; i < count; i++) {
        hhg_token_type_t type = va_arg(va, hhg_token_type_t);
        
        if (lexer->token.type == type)
            break;
    }

    va_end(va);

    if (i == count) {
        hhg_lexer_error(
            "expected %s, got token \"%s\"",
            summary,
            hhg_token_type_to_str(lexer->token.type)
        );
        return;
    }

    hhg_lexer_next(lexer);
}

void hhg_lexer_del(hhg_lexer_t *lexer)
{
    hhg_token_del(&lexer->token);
    hhg_free(lexer->text);
}

static int hhg_lexer_next_char(hhg_lexer_t *lexer)
{
    char c = lexer->text[lexer->text_idx];
    if (c == '\0') {
        lexer->end_idx++;
        return EOF;
    } else {
        lexer->text_idx++;
        if (c == '\n') {
            lexer->pos.col = 1;
            lexer->pos.line++;
            arrput(lexer->line_starts, lexer->text_idx);
        } else
            lexer->pos.col++;
    }
    return c;
}

static void hhg_lexer_back_char(hhg_lexer_t *lexer)
{
    if (lexer->end_idx > 0) {
        lexer->end_idx--;
        return;
    }

    lexer->text_idx--;

    if (lexer->pos.col == 1) {
        lexer->pos.line--;
        // line starts at 1 but array is 0-indexed so lookup after decrement
        lexer->pos.col = lexer->line_starts[lexer->pos.line] - 1;
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

    for (size_t i = 0; i < ARR_SIZE(keyword_data); i++)
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

    char *decimal_pos = strchr(lexer->token.str.str, '.');
    if (decimal_pos != NULL)
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
            hhg_lexer_error("unexpected EOF in string literal");
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
        hhg_lexer_error("character constant is too long");

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
                    hhg_lexer_error("unexpected EOF in multi-line comment");
                    break;
                }
            } while (c2 != '*' || c != '/');
            return false;
        }
    }

    int c3 = hhg_lexer_next_char(lexer);

    for (size_t i = 0; i < ARR_SIZE(op_data); i++) {
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
