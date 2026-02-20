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
    if (cfg->project.version == NULL)
        cfg->project.version = "0.1.0";
    if (cfg->project.std == NULL)
        cfg->project.std = HHG_VERSION;

    const char *cfg_filename;

    if (cfg->project.name == NULL) {
        char cwd[LIBFS_MAX_PATH];
        if (!fs_current_dir(cwd, LIBFS_MAX_PATH))
            hhg_fatal_error("failed to get current directory");
        cfg->project.name = (char *)fs_basename(cwd);

        cfg_filename = HHG_CONFIG_FILENAME;
    } else {
        {
            bool result = fs_make_dir(cfg->project.name);
            if (!result)
                hhg_fatal_error(
                    "failed to create project directory: %s",
                    cfg->project.name
                );
        }
        {
            char cfg_path[LIBFS_MAX_PATH];
            int result =
                fs_join_path(
                    cfg_path,
                    LIBFS_MAX_PATH,
                    cfg->project.name,
                    HHG_CONFIG_FILENAME
                );
            if (result >= LIBFS_MAX_PATH)
                hhg_fatal_error(
                    "path too long: %s/" HHG_CONFIG_FILENAME,
                    cfg->project.name
                );

            cfg_filename = cfg_path;
        }
    }
    
    if (fs_exist(cfg_filename))
        hhg_fatal_error("config file already exists: %s", cfg_filename);

    FILE *cfg_file = hhg_utils_fopen(cfg_filename, "w");

    fprintf(
        cfg_file,
        cfg_txt,
        cfg->project.name,
        cfg->project.version,
        cfg->project.std
    );
    fclose(cfg_file);
    return false;
}
