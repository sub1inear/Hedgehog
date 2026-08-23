#ifndef HHG_SEMA_H
#define HHG_SEMA_H

typedef struct hhg_sym_tab hhg_sym_tab_t;
typedef struct hhg_type_ctx hhg_type_ctx_t;
typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct arena hhg_arena_t;
typedef struct hhg_node hhg_node_t;

typedef struct hhg_sema {
    hhg_sym_tab_t *sym_tab;
    hhg_type_ctx_t *type_ctx;
    hhg_msg_ctx_t *msg_ctx;
    hhg_arena_t *arena;
} hhg_sema_t;

void hhg_sema_init(hhg_sema_t *sema, hhg_sym_tab_t *sym_tab,
                   hhg_type_ctx_t *type_ctx, hhg_msg_ctx_t *msg_ctx,
                   hhg_arena_t *arena);

void hhg_sema_run(hhg_sema_t *sema, hhg_node_t *node);

void hhg_sema_deinit(hhg_sema_t *sema);

#endif
