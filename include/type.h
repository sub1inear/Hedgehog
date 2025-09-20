#ifndef HHG_TYPE_H
#define HHG_TYPE_H

#include <stdbool.h>

#include "token.h"

#define HHG_TYPE_START 0

typedef enum _hhg_base_type_t {
    TYPE_NONE = HHG_TYPE_START,
    TYPE_I8,
    TYPE_U8,

    TYPE_I16,
    TYPE_U16,
    
    TYPE_I32,
    TYPE_U32,

    TYPE_I64,
    TYPE_U64,

    TYPE_INT,

    TYPE_F32,
    TYPE_F64,

    TYPE_FLOAT,

    TYPE_BOOL,

    TYPE_CHAR,

    TYPE_ISIZE,
    TYPE_USIZE,

    TYPE_TIME_T,
} hhg_base_type_t;

#define HHG_TYPE_END TYPE_TIME_T

typedef struct _hhg_type_t {
    hhg_base_type_t type : 5;
    unsigned int is_const : 1;
    unsigned int is_volatile : 1;
    unsigned int ref_count : 25;
} hhg_type_t;


hhg_type_t hhg_type_from_token_type(hhg_token_type_t type);
void hhg_type_print(hhg_type_t type);

#endif