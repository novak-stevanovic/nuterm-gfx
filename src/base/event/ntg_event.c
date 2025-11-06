#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#include "base/event/ntg_event.h"

static uint __id_gen = 0;

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data)
{
    assert(event != NULL);

    event->_type = type;
    event->_source = source;
    event->_data = data;
    event->_id = __id_gen;
    __id_gen++;
}

void __ntg_event_deinit__(ntg_event* event)
{
    assert(event != NULL);

    event->_type = UINT_MAX;
    event->_id = UINT_MAX;
    event->_data = NULL;
    event->_source = NULL;
}
