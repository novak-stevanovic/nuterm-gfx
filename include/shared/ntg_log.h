#ifndef NTG_LOG_H
#define NTG_LOG_H

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Closes any previous log, opens `filepath` in write mode, and records a log-start message. */
/* Failure to open the file terminates the process with `EXIT_FAILURE`. */
void ntg_log_init(const char* filepath);
/* Closes the current log file and disables logging. Calling it when logging is inactive has no */
/* effect. */
void ntg_log_deinit();

/* Formats a timestamped message with `printf`-style arguments, appends it to the active log, and */
/* flushes immediately. Calls are ignored when logging is inactive; `fmt` must be non-`NULL`. */
void ntg_log_log(const char* fmt, ...);

#endif // NTG_LOG_H
