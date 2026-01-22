#ifndef NTG_WIDGET_SIZE_MAP_H
#define NTG_WIDGET_SIZE_MAP_H

#include "shared/ntg_typedef.h"

ntg_widget_size_map* ntg_widget_size_map_new(size_t child_count, sarena* arena);

void ntg_widget_size_map_set(ntg_widget_size_map* map, const ntg_widget* widget,
                             size_t size);

size_t ntg_widget_size_map_get(const ntg_widget_size_map* map,
                               const ntg_widget* widget);

#endif // NTG_WIDGET_SIZE_MAP_H
