#ifndef _NTG_PLATFORM_H_
#define _NTG_PLATFORM_H_

typedef struct ntg_platform ntg_platform;

struct ntg_platform_req
{
    void* data;
    void (*action_fn)(void* data);
};

struct ntg_platform
{
};

#endif // _NTG_PLATFORM_H_
