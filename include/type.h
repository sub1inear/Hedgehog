#ifndef HHG_TYPE_H
#define HHG_TYPE_H

#include <stdbool.h>

#include "token.h"

#define HHG_TYPE_START 0

enum _hhg_base_type_t {
    HHG_TYPE_NONE = HHG_TYPE_START,
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

    HHG_TYPE_TIME_T,

    HHG_TYPE_FUNC,
    HHG_TYPE_CLASS,
    HHG_TYPE_ENUM,
};
// guarantee being unsigned to not overflow bit-field
typedef unsigned int hhg_base_type_t;

#define HHG_TYPE_END TYPE_ENUM

typedef struct _hhg_type_t {
    hhg_base_type_t type : 5;
    unsigned int is_const : 1;
    unsigned int is_volatile : 1;
    unsigned int ref_count : 25;
} hhg_type_t;


hhg_type_t hhg_type_from_token_type(hhg_token_type_t type);
void hhg_type_print(hhg_type_t type);

#endif
