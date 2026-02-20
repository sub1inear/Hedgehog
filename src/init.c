#include <stdbool.h>

#include "init.h"
#include "cfg.h"
#include "msg.h"
#include "fs.h"
#include "main.h"
#include "utils.h"

static const char cfg_txt[] =
    "# Hedgehog Configuration File"                                                                                   "\n"
                                                                                                                      "\n"
    "[project]"                                                                                                       "\n"
    "name = \"%s\""                                                                                                   "\n"
    "version = \"%s\" # project version in semver"                                                                    "\n"
    "std = \"%s\" # standard version in semver"                                                                       "\n"
                                                                                                                      "\n"
    "[global]"                                                                                                        "\n"
    "color = true # enable or disable color in output"                                                                "\n"
                                                                                                                      "\n"
    "[build]"                                                                                                         "\n"
    "entry = \"main.hhg\" # entry point of the program/compiler"                                                      "\n"
    "out-dir = \"out\" # output directory for build artifacts"                                                        "\n"
    "mode = \"debug\" # debug | release"                                                                              "\n"
                                                                                                                      "\n"
    "stage = \"none\" # stop after specified stage: none | lexer | parser | sem_an | mir-gen | code_gen | ext-build"  "\n"
    "debug-stage = \"none\" # debug specified stage: none | lexer | parser | sem_an | mir-gen | code_gen | ext-build" "\n"
                                                                                                                      "\n"
    "target = \"auto\" # cross-target triple: triple | auto"                                                          "\n"
    "backend = \"cpp\" # backend format: cpp | qbe"                                                                   "\n"
    "incremental = false # incremental compilation (only with cpp backend)"                                           "\n"
                                                                                                                      "\n"
    "warnings = \"default\" # warnings settings: default | all | none | pedantic"                                     "\n"
    "error-warnings = false # treat warnings as errors"                                                               "\n"
    ""                                                                                                                "\n"
    "[run]"                                                                                                           "\n"
    "args = [] # arguments to pass to program"                                                                        "\n"
                                                                                                                      "\n"
    "[test]"                                                                                                          "\n"
    "test-dir = \"test\" # directory with test cases"                                                                 "\n"
    "list = false # list test cases without running them"                                                             "\n"
    "fail-fast = false # stop on first failure"                                                                       "\n"
    "threads = -1 # number of threads to use (-1 => auto)"                                                            "\n"
    "filter = \"\" # filter test cases (regex)"                                                                       "\n"
                                                                                                                      "\n"
    "[clean]"                                                                                                         "\n"
    "mode = \"all\" # clean mode: all | build | gen"                                                                  "\n"
    "force = false # force clean without confirmation"                                                                "\n"
    "dry-run = false # print files to be deleted without deleting them"                                               "\n"
                                                                                                                      "\n"
    "[repl]"                                                                                                          "\n"
    "tmp-dir = \"tmp\" # temporary directory for repl"                                                                "\n"
    "target = \"auto\" # cross-target triple: triple | auto"                                                          "\n"
    "backend = \"cpp\" # backend format: cpp | qbe"                                                                   "\n";

bool hhg_init(hhg_cfg_t *cfg)
{
    const char *cfg_filename;
    char cfg_path[LIBFS_MAX_PATH];

    if (cfg->init.name == NULL) {
        char cwd[LIBFS_MAX_PATH];
        if (!fs_current_dir(cwd, LIBFS_MAX_PATH))
            hhg_fatal_error("failed to get current directory");
        cfg->init.name = (char *)fs_basename(cwd);

        if (fs_exist(HHG_CONFIG_FILENAME))
            hhg_fatal_error("config file already exists: " HHG_CONFIG_FILENAME);
        cfg_filename = HHG_CONFIG_FILENAME;
    } else {
        if (fs_exist(cfg->init.name))
            hhg_fatal_error(
                "project directory already exists: %s",
                cfg->init.name
            );
        
        if (!fs_make_dir(cfg->init.name))
            hhg_fatal_error(
                "failed to create project directory: %s",
                cfg->init.name
            );

        int result =
            fs_join_path(
                cfg_path,
                LIBFS_MAX_PATH,
                cfg->init.name,
                HHG_CONFIG_FILENAME
            );
        if (result >= LIBFS_MAX_PATH)
            hhg_fatal_error(
                "path too long: %s/" HHG_CONFIG_FILENAME,
                cfg->init.name
            );

        // must not exist, we just created the directory
        cfg_filename = cfg_path;
    }

    FILE *cfg_file = hhg_utils_fopen(cfg_filename, "w");
    fprintf(
        cfg_file,
        cfg_txt,
        cfg->init.name,
        cfg->init.version,
        cfg->init.std
    );
    fclose(cfg_file);
    return false;
}
