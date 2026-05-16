#ifndef HHG_CLEAN_H
#define HHG_CLEAN_H

#include <stdbool.h>

// typedef struct hhg_msg_ctx hhg_msg_ctx_t;
#include "msg.h"
/*
safety features:
checks for sentinel file marking output directory
checks for subdirectories and errors out if they exist
(output directory should never contain subdirectories)
if --force/-f is not provided,
prompts user for confirmation with the output directory name before deleting
never recursively deletes through directories
*/
bool hhg_clean(hhg_cfg_t *cfg, hhg_msg_ctx_t *msg_ctx);

#endif
