#include "core/object/widget/shared/ntg_widget_map.h"
#include "ntg.h"
#include <assert.h>

struct ntg_widget_xy_map
{
    ntg_widget_map __base;
};

ntg_widget_xy_map* ntg_widget_xy_map_new(size_t child_count, sarena* arena)
{
    ntg_widget_xy_map* new = (ntg_widget_xy_map*)sarena_malloc(
            arena, sizeof(ntg_widget_xy_map));
    assert(new != NULL);

    ntg_widget_map_init(&new->__base, child_count,
            sizeof(struct ntg_xy), arena);

    return new;
}

void ntg_widget_xy_map_set(ntg_widget_xy_map* map, const ntg_widget* widget,
                           struct ntg_xy xy)
{
    assert(map != NULL);
    assert(widget != NULL);

    ntg_widget_map_set(&map->__base, widget, &xy);
}

struct ntg_xy 
ntg_widget_xy_map_get(const ntg_widget_xy_map* map, const ntg_widget* widget)
{
    assert(map != NULL);
    assert(widget != NULL);

    struct ntg_xy* result = (struct ntg_xy*)ntg_widget_map_get(&map->__base, widget);
    assert(result != NULL);

    return (result != NULL) ? *result : (struct ntg_xy) {0};
}
