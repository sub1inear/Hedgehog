#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <fs.h>

#include "clean.h"
#include "main.h"
#include "cfg.h"
#include "msg.h"
#include "mem.h"
#include "utils.h"

#define hhg_clean_basic_error(msg_ctx, ...) \
    hhg_basic_msg(msg_ctx, HHG_MSG_ERROR, __VA_ARGS__)
#define hhg_clean_basic_warning(msg_ctx, ...) \
    hhg_basic_msg(msg_ctx, HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_clean_basic_info(msg_ctx, ...) \
    hhg_basic_msg(msg_ctx, HHG_MSG_INFO, __VA_ARGS__)

static void hhg_clean_iter_out_dir(
    void (*func)(const char *subpath, hhg_cfg_t *cfg, hhg_msg_ctx_t *msg_ctx),
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx
);
static void hhg_clean_del_file(
    const char *subpath,
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx
);
static void hhg_clean_check_dir(
    const char *subpath,
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx
);

bool hhg_clean(hhg_cfg_t *cfg, hhg_msg_ctx_t *msg_ctx)
{
    if (!fs_exist(cfg->build.out_dir))
        hhg_fatal_error(
            "output directory `%s` does not exist\n"
            "help: to create it, run `hhg build`",
            cfg->build.out_dir
        );

    char sentinel_filename[FS_MAX_PATH];
    hhg_utils_join_path(
        sentinel_filename,
        HHG_ARR_SIZE(sentinel_filename),
        cfg->build.out_dir,
        ".hhg_out_dir"
    );

    if (!fs_exist(sentinel_filename))
        hhg_fatal_error(
            "output directory `%s` is not a valid Hedgehog build output directory\n"
            "help: if the output directory is correct, delete it and rerun `hhg build`",
            cfg->build.out_dir
        );
    
    hhg_clean_iter_out_dir(hhg_clean_check_dir, cfg, msg_ctx);
    
    if (!cfg->clean.force) {
        // not a hhg_clean_basic_* because those may be buffered
        printf(
            "delete files in output directory `%s`? [y/N]: ",
            cfg->build.out_dir
        );
        char input[4];
        if (fgets(input, sizeof(input), stdin) == NULL)
            hhg_fatal_error("failed to read input");
        
        if (input[0] != 'y' && input[0] != 'Y') {
            hhg_clean_basic_info(msg_ctx, "aborting clean");
            return true;
        }
    }

    hhg_clean_iter_out_dir(hhg_clean_del_file, cfg, msg_ctx);
    
    if (cfg->clean.dry_run)
        hhg_clean_basic_info(
            msg_ctx,
            "would delete output directory: `%s`",
            cfg->build.out_dir
        );
    else if (!fs_delete_dir(cfg->build.out_dir))
        hhg_fatal_error(
            "failed to delete output directory: %s",
            cfg->build.out_dir
        );

    return false;
}

static void hhg_clean_iter_out_dir(
    void (*func)(const char *subpath, hhg_cfg_t *cfg, hhg_msg_ctx_t *msg_ctx),
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx
)
{
    struct fs_directory_iterator *it = fs_open_dir(cfg->build.out_dir);
    if (it == NULL)
        hhg_fatal_error(
            "failed to open output directory: %s",
            cfg->build.out_dir
        );
    while (fs_read_dir(it)) {
        const char *dir_subpath = it->path;
        if (strcmp(dir_subpath, ".")  == 0 ||
            strcmp(dir_subpath, "..") == 0)
            continue;

        char subpath[FS_MAX_PATH];
        hhg_utils_join_path(
            subpath,
            HHG_ARR_SIZE(subpath),
            cfg->build.out_dir,
            dir_subpath
        );
        
        func(subpath, cfg, msg_ctx);
    }
    fs_close_dir(it);
}

static void hhg_clean_check_dir(
    const char *subpath,
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx
)
{
    HHG_UNUSED(msg_ctx);
    if (fs_is_dir(subpath))
        hhg_fatal_error(
            "output directory `%s` contains unexpected subdirectory `%s`\n"
            "help: if the output directory is correct, delete it manually and rerun `hhg build`",
            cfg->build.out_dir,
            subpath
        );
}

static void hhg_clean_del_file(
    const char *subpath,
    hhg_cfg_t *cfg, 
    hhg_msg_ctx_t *msg_ctx
)
{
    if (cfg->clean.dry_run) {
        hhg_clean_basic_info(
            msg_ctx,
            "would delete file: `%s`",
            subpath
        );
        return;
    }
    if (!fs_delete_file(subpath))
        hhg_fatal_error(
            "failed to delete file during clean: %s",
            subpath
        );
}
