#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "repl.h"
#include "main.h"
#include "utils.h"

static void hhg_repl_sigint(int signum);

void hhg_repl(hhg_cfg_t *cfg)
{
    HHG_UNUSED(cfg);
    
    // catch Ctrl+C to exit
    signal(SIGINT, hhg_repl_sigint);
    
    puts(
        "Hedgehog REPL v" HHG_VERSION "\n"
        "Ctrl+C to exit"
    );

    while (true) {
        fputs("> ", stdout);
        
        char buf[256];
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            break;
        buf[strcspn(buf, "\n")] = '\0';

        puts(buf);

        // ...
    }
}

static void hhg_repl_sigint(int signum)
{
    HHG_UNUSED(signum);
    abort();
}

