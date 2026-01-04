#include "ntg.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static FILE *log_file = NULL;

void _ntg_log_init_(const char* filepath)
{
    if (log_file != NULL)
    {
        /* Already initialized: close previous file first */
        fclose(log_file);
    }

    log_file = fopen(filepath, "w");
    if (log_file == NULL)
    {
        exit(EXIT_FAILURE);
    }

    ntg_log_log("NTG LOG BEGINNING");
}

void _ntg_log_deinit_()
{
    if (log_file != NULL)
    {
        ntg_log_log("NTG LOG END");
        fflush(log_file);
        fclose(log_file);
        log_file = NULL;
    }
}

void ntg_log_log(const char* fmt, ...)
{
    if (log_file == NULL) return;

    va_list args;
    va_start(args, fmt);

    char timebuf[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(log_file, "[%s] - ", timebuf);

    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    fflush(log_file);

    va_end(args);
}
