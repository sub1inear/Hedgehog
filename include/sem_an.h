#ifndef HHG_SEM_AN_H
#define HHG_SEM_AN_H

#include "sym_tab.h"
#include "node.h"
#include "type_ctx.h"

typedef struct hhg_sem_an {
    hhg_sym_tab_t *sym_tab;
    hhg_type_ctx_t *type_ctx;
    hhg_arena_t *arena;
} hhg_sem_an_t;

void hhg_sem_an_init(
    hhg_sem_an_t *sem_an,
    hhg_sym_tab_t *sym_tab,
    hhg_type_ctx_t *type_ctx,
    hhg_arena_t *arena
);

void hhg_sem_an_run(hhg_sem_an_t *sem_an, hhg_node_t *node);

#endif