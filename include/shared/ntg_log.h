#ifndef NTG_LOG_H
#define NTG_LOG_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */


NTG_API int
ntg_log_init(const char* filepath);


NTG_API void
ntg_log_deinit(void);


NTG_API void
ntg_log_log(const char* fmt, ...);

#endif // NTG_LOG_H
