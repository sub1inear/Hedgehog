#ifndef HHG_TYPE_H
#define HHG_TYPE_H

#include <stdio.h>
#include <stdbool.h>

#include "token.h"

typedef struct hhg_sym hhg_sym_t;
typedef struct hhg_node hhg_node_t;
typedef struct arena hhg_arena_t;

#define HHG_BUILTIN_TYPE_START HHG_TYPE_I8
#define HHG_BUILTIN_TYPE_END HHG_TYPE_VOID + 1
#define HHG_BUILTIN_TYPE_COUNT (HHG_BUILTIN_TYPE_END - HHG_BUILTIN_TYPE_START)

typedef enum hhg_base_type {
    HHG_TYPE_NONE,
    HHG_TYPE_I8,
    HHG_TYPE_U8,

    HHG_TYPE_I16,
    HHG_TYPE_U16,

    HHG_TYPE_I32,
    HHG_TYPE_U32,

    HHG_TYPE_I64,
    HHG_TYPE_U64,

    HHG_TYPE_F32,
    HHG_TYPE_F64,

    HHG_TYPE_BOOL,
    HHG_TYPE_CHAR,

    HHG_TYPE_ISIZE,
    HHG_TYPE_USIZE,

    HHG_TYPE_VOID,

    HHG_TYPE_REF,
    HHG_TYPE_ARR,

    HHG_TYPE_FN,
} hhg_base_type_t;

typedef enum hhg_ref_qual {
    HHG_REF_QUAL_NONE,
    HHG_REF_QUAL_IMMUT,
    HHG_REF_QUAL_MUT,
} hhg_ref_qual_t;

typedef struct hhg_type hhg_type_t;

typedef struct hhg_type_ref {
    hhg_type_t *base;
    hhg_ref_qual_t qual;
} hhg_type_ref_t;

typedef struct hhg_type_arr {
    hhg_type_t *elem;
    hhg_node_t *size;
} hhg_type_arr_t;

typedef struct hhg_type_fn {
    hhg_sym_t *sym;
    hhg_type_t *ret;
    hhg_type_t **params;
} hhg_type_fn_t;

typedef union hhg_type_value {
    hhg_type_ref_t ref;
    hhg_type_arr_t arr;
    hhg_type_fn_t fn;
} hhg_type_value_t;

struct hhg_type {
    hhg_base_type_t type;
    hhg_type_value_t value;
};

void hhg_base_type_print(hhg_base_type_t base);
void hhg_base_type_fprint(hhg_base_type_t base, FILE *stream);
const char *hhg_base_type_to_str(hhg_base_type_t base);

hhg_base_type_t hhg_token_type_to_base_type(hhg_token_type_t token_type);
bool hhg_base_type_is_arith(hhg_token_type_t token_type);

void hhg_ref_qual_print(hhg_ref_qual_t qual);
void hhg_ref_qual_fprint(hhg_ref_qual_t qual, FILE *stream);
const char *hhg_ref_qual_to_str(hhg_ref_qual_t qual);

void hhg_type_init(hhg_type_t *type, hhg_base_type_t base);
hhg_type_t *hhg_type_new(hhg_base_type_t base, hhg_arena_t *arena);

bool hhg_type_eq(hhg_type_t *l, hhg_type_t *r);

void hhg_type_print(hhg_type_t *type);
void hhg_type_fprint(hhg_type_t *type, FILE *stream);

void hhg_type_del(hhg_type_t *type);

#endif
