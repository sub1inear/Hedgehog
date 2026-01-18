#ifndef HHG_DEBUG_H
#define HHG_DEBUG_H

#include <stdbool.h>

typedef enum hhg_stage {
    HHG_STAGE_LEXER,
    HHG_STAGE_PARSER,
    HHG_STAGE_SEM_AN,
    HHG_STAGE_MIR,
    HHG_STAGE_RUN,
} hhg_stage_t;

bool hhg_debug(const char *filename, hhg_stage_t stage);

#endif
