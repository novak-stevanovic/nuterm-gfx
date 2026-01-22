#ifndef NTG_LOG_H
#define NTG_LOG_H

void ntg_log_init(const char* filepath);
void ntg_log_deinit();

void ntg_log_log(const char* fmt, ...);

#endif // NTG_LOG_H
