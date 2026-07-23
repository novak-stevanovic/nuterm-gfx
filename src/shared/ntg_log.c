#define _POSIX_C_SOURCE 200809L

#include "ntg.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* DATA */
/* -------------------------------------------------------------------------- */

static FILE* log_file = NULL;
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void _ntg_log_vwrite_unlocked(const char* fmt, va_list args)
{
    char timebuf[64] = "unknown-time";
    time_t now = time(NULL);
    struct tm tm_info;

    if((now != (time_t)-1) && (localtime_r(&now, &tm_info) != NULL))
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm_info);

    fprintf(log_file, "[%s] - ", timebuf);
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    fflush(log_file);
}

static void _ntg_log_write_unlocked(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    _ntg_log_vwrite_unlocked(fmt, args);

    va_end(args);
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_log_init(const char* filepath, int* out_status)
{
    ntg_init_status(out_status);

    if(filepath == NULL)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    pthread_mutex_lock(&log_lock);

    FILE* new_log_file = fopen(filepath, "w");
    if(new_log_file == NULL)
    {
        pthread_mutex_unlock(&log_lock);
        ntg_vreturn(out_status, NTG_ERR_LOG_INIT_FAIL);
    }

    if(log_file != NULL)
    {
        _ntg_log_write_unlocked("NTG LOG END");
        fclose(log_file);
    }

    log_file = new_log_file;
    _ntg_log_write_unlocked("NTG LOG BEGINNING");

    pthread_mutex_unlock(&log_lock);
}

void ntg_log_deinit(void)
{
    pthread_mutex_lock(&log_lock);

    if(log_file != NULL)
    {
        _ntg_log_write_unlocked("NTG LOG END");
        fclose(log_file);
        log_file = NULL;
    }

    pthread_mutex_unlock(&log_lock);
}

void ntg_log_log(const char* fmt, ...)
{
    if(fmt == NULL) return;

    pthread_mutex_lock(&log_lock);

    if(log_file != NULL)
    {
        va_list args;
        va_start(args, fmt);

        _ntg_log_vwrite_unlocked(fmt, args);

        va_end(args);
    }

    pthread_mutex_unlock(&log_lock);
}
