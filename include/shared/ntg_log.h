#ifndef _NTG_LOG_H_
#define _NTG_LOG_H_

void _ntg_log_init_(const char* filepath);
void _ntg_log_deinit_();

void ntg_log_log(const char* fmt, ...);

#endif // _NTG_LOG_H_
