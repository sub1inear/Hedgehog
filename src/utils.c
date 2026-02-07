#include <string.h>

#include "utils.h"

char *hhg_utils_path_trunc(char *path)
{
    char *trunc = strrchr(path, '/');
    if (trunc == NULL) trunc = strrchr(path, '\\');
    return trunc == NULL ? path : trunc + 1;
}