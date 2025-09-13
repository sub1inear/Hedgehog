#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "error.h"
#include "utils.h"

static const hhg_op_data_t op_data[] = {
    { { '+' , '+' , '\0' },  INC,        13 },
    { { '-' , '-' , '\0' },  DEC,        13 },
                                                 
    { { '*' , '\0', '\0' },  '*',        12 },
    { { '/' , '\0', '\0' },  '/',        12 },
    { { '%' , '\0', '\0' },  '%',        12 },
                                                 
    { { '+' , '\0', '\0' },  '+',        11 },
    { { '-' , '\0', '\0' },  '-',        11 },
                                         
    { { '<' , '<' , '\0' },  LSHIFT,     10 },
    { { '>' , '>' , '\0' },  RSHIFT,     10 },
                                         
    { { '<' , '\0', '\0' },  '<',        9  },
    { { '>' , '\0', '\0' },  '>',        9  },
    { { '<' , '=' , '\0' },  LT_EQ,      9  },
    { { '>' , '=' , '\0' },  GT_EQ,      9  },
                                         
    { { '=' , '=' , '\0' },  EQ,         8  },
    { { '!' , '=' , '\0' },  NOT_EQ,     8  },
                                         
    { { '&' , '\0', '\0' },  '&',        7  },
    { { '^' , '\0', '\0' },  '^',        6  },
    { { '|' , '\0', '\0' },  '|',        5  },
                                         
    { { '+' , '=' , '\0' },  PLUS_EQ,    1  },
    { { '-' , '=' , '\0' },  SUB_EQ,     1  },
    { { '*' , '=' , '\0' },  MUL_EQ,     1  },
    { { '/' , '=' , '\0' },  DIV_EQ,     1  },
    { { '%' , '=' , '\0' },  MOD_EQ,     1  },
    { { '&' , '=' , '\0' },  AND_EQ,     1  },
    { { '|' , '=' , '\0' },  OR_EQ,      1  },
    { { '^' , '=' , '\0' },  XOR_EQ,     1  },
    { { '<' , '<' , '='  },  LSHIFT_EQ,  1  },
    { { '>' , '>' , '='  },  RSHIFT_EQ,  1  },
};

static const hhg_keyword_data_t keyword_data[] = {
    { "if",       IF       },
    { "while",    WHILE    },
    { "for",      FOR      },
      
    { "break",    BREAK    },
    { "continue", CONTINUE },
      
    { "and",      AND      },
    { "or",       OR       },
    { "not",      NOT      },
      
    { "true",     TRUE     },
    { "false",    FALSE    },
      
    { "in",       IN       },
    { "range",    RANGE    },
      
    { "enum",     ENUM     },
      
    { "def",      DEF      },
    
    // ------------------------------- //

    { "i8",       I8       },
    { "u8",       U8       },

    { "i16",      I16      },
    { "u16",      U16      },
                                        
    { "i32",      I32      },
    { "u32",      U32      },
                                        
    { "i64",      I64      },
    { "u64",      U64      },
                                        
    { "int",      INT      },
                                        
    { "f32",      F32      },
    { "f64",      F64      },
      
    { "float",    FLOAT    },
      
    { "bool",     BOOL     },
    { "char",     CHAR     },
      
    { "isize",    ISIZE    },
    { "usize",    USIZE    },
      
    { "time_t",   TIME_T   },
};

static void hhg_lexer_lex(hhg_lexer_t *lexer);
static void hhg_lexer_lex_core(hhg_lexer_t *lexer);

void hhg_lexer_init(hhg_lexer_t *lexer, const char *filename)
{
    lexer->file = fopen(filename, "r");
    if (lexer->file == NULL)
        hhg_fatal_error("error opening file: %s", strerror(errno));

    hhg_token_init(&lexer->token);
}

void hhg_lexer_next(hhg_lexer_t *lexer)
{
    lexer->peeked = false;
    if (lexer->peeked)
        return;

    hhg_lexer_lex(lexer);
}

void hhg_lexer_peek(hhg_lexer_t *lexer)
{
    if (lexer->peeked)
        return;

    lexer->peeked = true;
    hhg_lexer_lex(lexer);
}

void hhg_lexer_match(hhg_lexer_t *lexer, hhg_token_type_t type)
{
    if (lexer->token.type == type) {
        hhg_lexer_next(lexer);
        return;
    }

    hhg_error("unexpected token");
}

void hhg_lexer_match_va(hhg_lexer_t *lexer, int32_t count, ...)
{
    va_list va;
    va_start(va, count);

    int32_t i;

    for (i = 0; i < count; i++) {
        hhg_token_type_t type = va_arg(va, hhg_token_type_t);

        if (lexer->token.type == type)
            break;
    }

    if (i == count) {
        hhg_error("unexpected token");
        return;
    }

    hhg_lexer_next(lexer);
}

void hhg_lexer_match_type(hhg_lexer_t *lexer)
{
    hhg_lexer_match_va(lexer, 17,
        I8, U8, I16, U16, I32,
        U32, I64, U64, INT, F32, F64, FLOAT,
        BOOL, CHAR, ISIZE, USIZE, TIME_T);
}

void hhg_lexer_del(hhg_lexer_t *lexer)
{
    hhg_token_del(&lexer->token);
    fclose(lexer->file);
}

static void hhg_lexer_lex(hhg_lexer_t *lexer)
{
    hhg_lexer_lex_core(lexer);

    if (lexer->token.type == NEWLINE) {
        long pos;

        do {
            pos = ftell(lexer->file);
            hhg_lexer_lex_core(lexer);
        } while (lexer->token.type == NEWLINE);
        
        fseek(lexer->file, pos, SEEK_SET);

        lexer->token.type = NEWLINE;

    }
}

static void hhg_lexer_lex_id(hhg_lexer_t *lexer, char c)
{
    do {
        hhg_str_append_char(&lexer->token.str, c);
        c = fgetc(lexer->file);
    } while (isalnum(c) || c == '_');

    ungetc(c, lexer->file);

    lexer->token.type = ID;

    for (int32_t i = 0; i < ARR_SIZE(keyword_data); i++)
        if (!strcmp(keyword_data[i].str, lexer->token.str.str)) {
            lexer->token.type = keyword_data[i].type;
            return;
        }
}

static void hhg_lexer_lex_num(hhg_lexer_t *lexer, char c)
{
    do {
        hhg_str_append_char(&lexer->token.str, c);
        c = fgetc(lexer->file);
    } while (isdigit(c));

    ungetc(c, lexer->file);

    lexer->token.type = INT_LITERAL;

    char *decimal_pos = strchr(lexer->token.str.str, '.');
    if (decimal_pos != NULL)
        lexer->token.type = FLOAT_LITERAL;

    return;
}

static void hhg_lexer_lex_str_literal(hhg_lexer_t *lexer, char c)
{
    hhg_str_append_char(&lexer->token.str, c);
    while (true) {
        c = fgetc(lexer->file);
        // not using switch to break out of loop
        if (c == EOF) {
            hhg_error("unexpected EOF in string literal");
            break;
        } else if (c == '\\') {
            hhg_str_append_char(&lexer->token.str, c);
            c = fgetc(lexer->file);
        } else if (c == '"') {
            hhg_str_append_char(&lexer->token.str, c);
            break;
        } else if (c == '\n') {
            hhg_str_append_str(&lexer->token.str, "\n", 2);
            continue;
        }
        hhg_str_append_char(&lexer->token.str, c);;
    }
    lexer->token.type = STRING_LITERAL;
}

static void hhg_lexer_lex_char_literal(hhg_lexer_t *lexer, char c)
{
    hhg_str_append_char(&lexer->token.str, c);
    c = fgetc(lexer->file);
    hhg_str_append_char(&lexer->token.str, c);

    if (c == '\\') {
        c = fgetc(lexer->file);
        hhg_str_append_char(&lexer->token.str, c);
    }

    if (c == '\\')
        hhg_str_append_char(&lexer->token.str, c);
    else
        hhg_error("character constant is too long");
}

static bool hhg_lexer_lex_default(hhg_lexer_t *lexer, char c)
{
    long pos = ftell(lexer->file); // save position for double ungetc
    char c2 = fgetc(lexer->file);
            
    if (c == '/') {
        if (c2 == '/') { // single-line comment
            do
                c = fgetc(lexer->file);
            while (c != '\n' && c != EOF);
            ungetc(c, lexer->file);

            return false;
        } else if (c2 == '*') { // multi-line comment
            do {
                c2 = c;
                c = fgetc(lexer->file);
                if (c == EOF) {
                    hhg_error("unexpected EOF in multi-line comment");
                    break;
                }
            } while (c2 != '*' && c != '/');

            ungetc(c, lexer->file);
            return false;
        }
    }

    char c3 = fgetc(lexer->file);

    for (size_t i = 0; i < ARR_SIZE(op_data); i++) {
        const hhg_op_data_t *data = &op_data[i];
        if (data->str[0] == c && 
            (data->str[1] == c2 || data->str[1] == '\0') &&
            (data->str[2] == c3 || data->str[2] == '\0')) {
            
            if (data->str[2] == c3)
                ; // consumed three chars, nothing to do
            else if (data->str[1] == c2)
                ungetc(c3, lexer->file); // consumed two chars, give back last
            else
                fseek(lexer->file, pos, SEEK_SET); // consumed one char, give back two

            lexer->token.type = data->type;
            lexer->token.prec = data->prec;

            return true;
        }
    }

    lexer->token.type = c;
    fseek(lexer->file, pos, SEEK_SET); // consumed one char, give back two
    
    return true;
}

static void hhg_lexer_lex_core(hhg_lexer_t *lexer)
{
    hhg_token_reset_aux(&lexer->token);
    while (true) {
        char c = fgetc(lexer->file);
        if (c == ' ' || c == '\t')
            ;
        else if (c == '\n') {
            lexer->token.type = NEWLINE;
            return;
        } else if (isalpha(c) || c == '_') {
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
        } else {
            if (hhg_lexer_lex_default(lexer, c))
                return;
        }
    }
}