#ifndef NTG_WIDGET_XY_MAP_H
#define NTG_WIDGET_XY_MAP_H

#include "shared/ntg_typedef.h"

ntg_widget_xy_map* ntg_widget_xy_map_new(size_t child_count, sarena* arena);

void ntg_widget_xy_map_set(
        ntg_widget_xy_map* map,
        const ntg_widget* widget,
        struct ntg_xy xy);

struct ntg_xy ntg_widget_xy_map_get(
        const ntg_widget_xy_map* map,
        const ntg_widget* widget);

#endif // NTG_WIDGET_XY_MAP_H
