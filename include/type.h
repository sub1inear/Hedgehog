#ifndef HHG_TYPE_H
#define HHG_TYPE_H

#include <stdbool.h>

#include "token.h"

typedef struct hhg_sym hhg_sym_t;
typedef struct hhg_type_ctx hhg_type_ctx_t;
typedef struct arena hhg_arena_t;

// must have <= 32 types to fit in 5 bits in hhg_type_t
enum hhg_base_type {
    HHG_TYPE_NONE,
    HHG_TYPE_I8,
    HHG_TYPE_U8,

    HHG_TYPE_I16,
    HHG_TYPE_U16,

    HHG_TYPE_I32,
    HHG_TYPE_U32,

    HHG_TYPE_I64,
    HHG_TYPE_U64,

    HHG_TYPE_INT,

    HHG_TYPE_F32,
    HHG_TYPE_F64,

    HHG_TYPE_FLOAT,

    HHG_TYPE_BOOL,

    HHG_TYPE_CHAR,

    HHG_TYPE_ISIZE,
    HHG_TYPE_USIZE,

    HHG_TYPE_REF,
    HHG_TYPE_ARR,

    HHG_TYPE_FUNC,
    HHG_TYPE_CLASS,
    HHG_TYPE_ENUM,

    // used for placeholder types in the parser
    HHG_TYPE_ID,
};

#define HHG_BUILTIN_TYPE_START HHG_TYPE_I8
#define HHG_BUILTIN_TYPE_END (HHG_TYPE_ID + 1)
#define HHG_BUILTIN_TYPE_COUNT (HHG_BUILTIN_TYPE_END - HHG_BUILTIN_TYPE_START)

// guarantee being unsigned to not overflow bit-field
typedef unsigned int hhg_base_type_t;

typedef struct hhg_type hhg_type_t;

typedef struct hhg_type_ref_info {
    hhg_type_t *base;
} hhg_type_ref_info_t;

typedef struct hhg_type_arr_info {
    hhg_type_t *elem;
    size_t size; // 0 for dynamic arrays
} hhg_type_arr_info_t;

typedef struct hhg_type_func_info {
    hhg_sym_t *sym;
    hhg_type_t *ret;
    hhg_type_t **params;
} hhg_type_func_info_t;

typedef struct hhg_type_class_field {
    char *key;
    hhg_type_t *value;
} hhg_type_class_field_t;

typedef struct hhg_type_class_info {
    hhg_sym_t *sym;
    hhg_type_class_field_t *fields; // string hash map
} hhg_type_class_info_t;

typedef union hhg_type_info {
    hhg_type_ref_info_t ref;     // HHG_TYPE_REF
    hhg_type_arr_info_t arr;     // HHG_TYPE_ARR
    hhg_type_func_info_t func;   // HHG_TYPE_FUNC
    hhg_type_class_info_t class; // HHG_TYPE_CLASS
    char *id;                    // HHG_TYPE_ID
} hhg_type_info_t;

struct hhg_type {
    hhg_base_type_t type : 5;
    unsigned int is_const : 1;
    unsigned int is_volatile : 1;
    hhg_type_info_t info;
};

void hhg_type_init(hhg_type_t *type, hhg_base_type_t base);
hhg_type_t *hhg_type_new(hhg_base_type_t base, hhg_arena_t *arena);

hhg_base_type_t hhg_token_type_to_base_type(hhg_token_type_t token_type);

bool hhg_base_type_is_arith(hhg_token_type_t token_type);

// checks type equality (regardless of const/volatile qualifiers)
bool hhg_type_eq(hhg_type_t *l, hhg_type_t *r);

void hhg_type_print(hhg_type_t *type);

void hhg_type_fprint(hhg_type_t *type, FILE *stream);

void hhg_type_del(hhg_type_t *type);

#endif
