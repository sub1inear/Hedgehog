#include <stdbool.h>

#define FS_IMPLEMENTATION
#include <fs.h>

#include "init.h"
#include "cmd_args.h"
#include "msg.h"
#include "main.h"
#include "utils.h"

static const char main_file_txt[] =
    "fn main() {\n"
    "    println(\"Hello, world!\");\n"
    "}\n";

bool hhg_init(hhg_cmd_args_init_t *init)
{
    const char *main_filename;
    char main_path[FS_MAX_PATH];

    if (init->name == NULL) {
        char cwd[FS_MAX_PATH];
        if (!fs_current_dir(cwd, sizeof(cwd)))
            hhg_fatal_error("failed to get current directory");

        main_filename = HHG_MAIN_FILENAME;
    } else {
        if (fs_exist(init->name))
            hhg_fatal_error(
                "project directory already exists: %s",
                init->name
            );
        
        if (!fs_make_dir(init->name))
            hhg_fatal_error(
                "failed to create project directory: %s",
                init->name
            );
        
        hhg_utils_join_path(
            main_path,
            HHG_ARR_LEN(main_path),
            init->name,
            HHG_MAIN_FILENAME
        );

        main_filename = main_path;
    }

    FILE *main_file = hhg_utils_fopen(main_filename, "w");
    fputs(main_file_txt, main_file);
    fclose(main_file);

    return true;
}
