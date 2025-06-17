#ifndef _NTG_LOG_H_
#define _NTG_LOG_H_

void __ntg_log_init__(const char* filepath);
void __ntg_log_deinit__();

void ntg_log_log(const char* fmt, ...);

#endif // _NTG_LOG_H_
