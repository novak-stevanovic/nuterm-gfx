#ifndef NTG_SHARED_H
#define NTG_SHARED_H

#ifdef NTG_EXPORT
#define NTG_API __attribute__((visibility("default")))
#else
#define NTG_API
#endif

#endif // NTG_SHARED_H
