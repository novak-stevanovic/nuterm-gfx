#include <assert.h>

#include "core/object/shared/ntg_object_sizes.h"
#include "core/object/shared/_ntg_object_map.h"
#include "shared/sarena.h"

struct ntg_object_sizes
{
    ntg_object_map __base;
};

ntg_object_sizes* ntg_object_sizes_new(size_t child_count, sarena* arena)
{
    sa_err err;
    ntg_object_sizes* new = (ntg_object_sizes*)sarena_malloc(arena,
            sizeof(ntg_object_sizes), &err);
    assert(new != NULL);

    __ntg_object_map_init__(&new->__base, child_count,
            sizeof(size_t), arena);

    return new;
}

void ntg_object_sizes_set(ntg_object_sizes* map,
        const ntg_object* object,
        size_t size)
{
    assert(map != NULL);
    assert(object != NULL);

    ntg_object_map_set(&map->__base, object, &size);
}

size_t ntg_object_sizes_get(
        const ntg_object_sizes* map,
        const ntg_object* object)
{
    assert(map != NULL);
    assert(object != NULL);

    size_t* result = (size_t*)
        ntg_object_map_get(&map->__base, object);
    assert(result != NULL);

    return (result != NULL) ? *result : 0;
}
