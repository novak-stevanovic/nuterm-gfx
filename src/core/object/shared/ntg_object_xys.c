#include <assert.h>

#include "core/object/shared/ntg_object_xys.h"
#include "core/object/shared/_ntg_object_map.h"
#include "shared/sarena.h"

struct ntg_object_xys
{
    ntg_object_map __base;
};

ntg_object_xys* ntg_object_xys_new(size_t child_count, sarena* arena)
{
    sa_err err;
    ntg_object_xys* new = (ntg_object_xys*)sarena_malloc(arena,
            sizeof(ntg_object_xys), &err);
    assert(new != NULL);

    __ntg_object_map_init__(&new->__base, child_count,
            sizeof(struct ntg_xy), arena);

    return new;
}

void ntg_object_xys_set(ntg_object_xys* map,
        const ntg_object* object,
        struct ntg_xy measure)
{
    assert(map != NULL);
    assert(object != NULL);

    ntg_object_map_set(&map->__base, object, &measure);
}

struct ntg_xy ntg_object_xys_get(
        const ntg_object_xys* map,
        const ntg_object* object)
{
    assert(map != NULL);
    assert(object != NULL);

    struct ntg_xy* result = (struct ntg_xy*)
        ntg_object_map_get(&map->__base, object);
    assert(result != NULL);

    return (result != NULL) ? *result : (struct ntg_xy) {0};
}
