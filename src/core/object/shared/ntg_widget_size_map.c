#include "core/object/widget/shared/ntg_widget_map.h"
#include "ntg.h"
#include <assert.h>

struct ntg_widget_size_map
{
    ntg_widget_map __base;
};

ntg_widget_size_map* ntg_widget_size_map_new(size_t child_count, sarena* arena)
{
    ntg_widget_size_map* new = (ntg_widget_size_map*)sarena_malloc(
            arena, sizeof(ntg_widget_size_map));
    assert(new != NULL);

    ntg_widget_map_init(&new->__base, child_count, sizeof(size_t), arena);

    return new;
}

void ntg_widget_size_map_set(ntg_widget_size_map* map,
        const ntg_widget* widget, size_t size)
{
    assert(map != NULL);
    assert(widget != NULL);

    ntg_widget_map_set(&map->__base, widget, &size);
}

size_t ntg_widget_size_map_get(const ntg_widget_size_map* map,
        const ntg_widget* widget)
{
    assert(map != NULL);
    assert(widget != NULL);

    size_t* result = (size_t*)ntg_widget_map_get(&map->__base, widget);
    assert(result != NULL);

    return (result != NULL) ? *result : 0;
}
