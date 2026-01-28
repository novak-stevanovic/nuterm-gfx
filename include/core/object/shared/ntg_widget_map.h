#ifndef NTG_WIDGET_MAP_H
#define NTG_WIDGET_MAP_H

#include "shared/ntg_typedef.h"

struct ntg_widget_map
{
    const ntg_widget** __keys;
    char* __values;
    size_t __data_size, __count, __capacity;
};

void ntg_widget_map_init(
        ntg_widget_map* ctx,
        size_t capacity,
        size_t data_size,
        sarena* arena);

void ntg_widget_map_set(ntg_widget_map* ctx, const ntg_widget* widget, void* data);

void* ntg_widget_map_get(const ntg_widget_map* ctx, const ntg_widget* widget);

#endif // NTG_WIDGET_MAP_H
