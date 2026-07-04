#ifndef NTG_LOG_H
#define NTG_LOG_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Starts logging to `filepath`. If logging is already active, the current file
 * is closed after a final message and replaced by the new one.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `filepath` is `NULL`.
 * - `NTG_ERR_LOG_INIT_FAIL`: the file cannot be opened. */
NTG_API void
ntg_log_init(const char* filepath, int* out_status);

/* Writes the final log message, closes the active file, and disables logging.
 * Calling it when logging is inactive has no effect. */
NTG_API void
ntg_log_deinit(void);

/* Formats a timestamped message with `printf`-style arguments, appends it to
 * the active log, and flushes immediately. The function is safe to call from
 * multiple threads. Calls are ignored when logging is inactive or `fmt` is
 * `NULL`. */
NTG_API void
ntg_log_log(const char* fmt, ...);

#endif // NTG_LOG_H
