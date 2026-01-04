#include "core/object/shared/ntg_object_map.h"
#include "ntg.h"
#include <assert.h>

struct ntg_object_size_map
{
    ntg_object_map __base;
};

ntg_object_size_map* ntg_object_size_map_new(size_t child_count, sarena* arena)
{
    ntg_object_size_map* new = (ntg_object_size_map*)sarena_malloc(
            arena, sizeof(ntg_object_size_map));
    assert(new != NULL);

    ntg_object_map_init(&new->__base, child_count,
            sizeof(size_t), arena);

    return new;
}

void ntg_object_size_map_set(ntg_object_size_map* map,
        const ntg_object* object,
        size_t size)
{
    assert(map != NULL);
    assert(object != NULL);

    ntg_object_map_set(&map->__base, object, &size);
}

size_t ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object)
{
    assert(map != NULL);
    assert(object != NULL);

    size_t* result = (size_t*)
        ntg_object_map_get(&map->__base, object);
    assert(result != NULL);

    return (result != NULL) ? *result : 0;
}
