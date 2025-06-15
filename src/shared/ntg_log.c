#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "shared/ntg_log.h"

static FILE *log_file = NULL;

void __ntg_log_init__(const char* filepath) {
    if (log_file) {
        /* Already initialized: close previous file first */
        fclose(log_file);
    }

    log_file = fopen(filepath, "w");
    if (!log_file) {
        fprintf(stderr, "Failed to open log file '%s'\n", filepath);
        exit(EXIT_FAILURE);
    }

    ntg_log_log("NTG LOG BEGINNING");
}

void __ntg_log_deinit__() {

    if (log_file) {
        ntg_log_log("NTG LOG END");
        fflush(log_file);
        fclose(log_file);
        log_file = NULL;
    }
}

void ntg_log_log(const char* fmt, ...) {
    if (!log_file) {
        /* If not initialized, default to stderr */
        return;
    }

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
