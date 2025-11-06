#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

#include <sys/types.h>

#define NTG_EVENT_TYPE_INVALID UINT_MAX 
#define NTG_EVENT_ID_INVALID UINT_MAX;

typedef struct ntg_event ntg_event;
typedef void (*ntg_event_handler)(void* subscriber, ntg_event* event);

struct ntg_event
{
    uint _id;
    uint _type;
    void* _source;
    void* _data;
};

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data);
void __ntg_event_deinit__(ntg_event* event);

struct ntg_event_sub
{
    void* subscriber;
    ntg_event_handler handler;
};

#endif // _NTG_EVENT_H_
