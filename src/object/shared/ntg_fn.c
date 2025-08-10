#include <assert.h>

#include "object/shared/ntg_fn.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_vec.h"

/* -------------------------------------------------------------------------- */
/* GENERIC */
/* -------------------------------------------------------------------------- */

struct ntg_map
{
    const ntg_object** objects;
    void* data;
    size_t data_size;
    size_t count;
    SArena* arena;
};

static void __ntg_map_init__(struct ntg_map* map, const ntg_object_vec* objects,
        size_t data_size, SArena* arena)
{
    assert(map != NULL);
    assert(arena != NULL);
    assert(data_size != 0);

    map->objects = sarena_malloc(arena, sizeof(ntg_object*) * objects->_count, NULL);
    assert(map->objects != NULL);
    map->data = sarena_calloc(arena, data_size * objects->_count, NULL);
    assert(map->data != NULL);

    size_t i;
    for(i = 0; i < objects->_count; i++)
    {
        map->objects[i] = objects->_data[i];
    }

    map->data_size = data_size;
    map->count = objects->_count;
    map->arena = arena;
}

static void __ntg_map_deinit__(struct ntg_map* map)
{
    assert(map != NULL);

    map->objects = NULL;
    map->data = NULL;
    map->data_size = 0;
    map->count = 0;
    map->arena = NULL;
}

static void* __ntg_map_get(struct ntg_map* map, const ntg_object* child)
{
    assert(map != NULL);
    assert(child != NULL);

    size_t i;
    for(i = 0; i < map->count; i++)
    {
        if(map->objects[i] == child)
            return &(map->data[i]);
    }

    return NULL;
}

static void __ntg_map_set(struct ntg_map* map, const ntg_object* child, void* data)
{
    assert(map != NULL);
    assert(child != NULL);
    assert(data != NULL);

    size_t i;
    for(i = 0; i < map->count; i++)
    {
        if(map->objects[i] == child)
            memcpy(&(map->data[i]), data, map->data_size);
    }
}

/* -------------------------------------------------------------------------- */
/* MEASURE */
/* -------------------------------------------------------------------------- */

struct ntg_measure_context
{
    struct ntg_map _base;
};

ntg_measure_context* ntg_measure_context_new(const ntg_object* parent, SArena* arena)
{
    ntg_measure_context* new = sarena_malloc(arena,
            sizeof(struct ntg_measure_context), NULL);

    assert(new != NULL);

    __ntg_map_init__((struct ntg_map*)new,
            ntg_object_get_children(parent),
            sizeof(struct ntg_measure_data),
            arena);

    return new;
}

void ntg_measure_context_destroy(ntg_measure_context* context)
{
    __ntg_map_deinit__((struct ntg_map*)context);
}

struct ntg_measure_data ntg_measure_context_get(const ntg_measure_context* context,
        const ntg_object* child)
{
    void* result = __ntg_map_get((struct ntg_map*)context, child);
    assert(result != NULL);

    return *((struct ntg_measure_data*)result);
}

void ntg_measure_context_set(ntg_measure_context* context,
        const ntg_object* child,
        struct ntg_measure_data data)
{
    __ntg_map_set((struct ntg_map*)context, child, &data);
}

/* -------------------------------------------------------------------------- */
/* CONSTRAIN */
/* -------------------------------------------------------------------------- */

struct ntg_constrain_context
{
    struct ntg_map _base;
};

ntg_constrain_context* ntg_constrain_context_new(const ntg_object* parent, SArena* arena)
{
    ntg_constrain_context* new = sarena_malloc(arena,
            sizeof(struct ntg_constrain_context), NULL);

    assert(new != NULL);

    __ntg_map_init__((struct ntg_map*)new,
            ntg_object_get_children(parent),
            sizeof(struct ntg_constrain_data),
            arena);

    return new;
}

void ntg_constrain_context_destroy(ntg_constrain_context* context)
{
    __ntg_map_deinit__((struct ntg_map*)context);
}

struct ntg_constrain_data ntg_constrain_context_get(const ntg_constrain_context* context,
        ntg_object* child)
{
    void* result = __ntg_map_get((struct ntg_map*)context, child);
    assert(result != NULL);

    return *((struct ntg_constrain_data*)result);
}

void ntg_constrain_context_set(ntg_constrain_context* context,
        const ntg_object* child,
        struct ntg_constrain_data data)
{
    __ntg_map_set((struct ntg_map*)context, child, &data);
}

struct ntg_constrain_output
{
    struct ntg_map _base;
};

ntg_constrain_output* ntg_constrain_output_new(const ntg_object* parent, SArena* arena)
{
    ntg_constrain_output* new = sarena_malloc(arena,
            sizeof(struct ntg_constrain_output), NULL);

    assert(new != NULL);

    __ntg_map_init__((struct ntg_map*)new,
            ntg_object_get_children(parent),
            sizeof(struct ntg_constrain_result),
            arena);

    return new;
}

void ntg_constrain_output_destroy(ntg_constrain_output* output)
{
    __ntg_map_deinit__((struct ntg_map*)output);
}

struct ntg_constrain_result ntg_constrain_output_get(
        const ntg_constrain_output* output,
        const ntg_object* child)
{
    void* result = __ntg_map_get((struct ntg_map*)output, child);
    assert(result != NULL);

    return *((struct ntg_constrain_result*)result);
}

void ntg_constrain_output_set(ntg_constrain_output* output,
        const ntg_object* child,
        struct ntg_constrain_result data)
{
    __ntg_map_set((struct ntg_map*)output, child, &data);
}

/* -------------------------------------------------------------------------- */
/* ARRANGE */
/* -------------------------------------------------------------------------- */

struct ntg_arrange_context
{
    struct ntg_map _base;
};

ntg_arrange_context* ntg_arrange_context_new(const ntg_object* parent, SArena* arena)
{
    ntg_arrange_context* new = sarena_malloc(arena,
            sizeof(struct ntg_arrange_context), NULL);

    assert(new != NULL);

    __ntg_map_init__((struct ntg_map*)new,
            ntg_object_get_children(parent),
            sizeof(struct ntg_arrange_data),
            arena);

    return new;
}

void ntg_arrange_context_destroy(ntg_arrange_context* context)
{
    __ntg_map_deinit__((struct ntg_map*)context);
}

struct ntg_arrange_data ntg_arrange_context_get(const ntg_arrange_context* context,
        const ntg_object* child)
{
    void* result = __ntg_map_get((struct ntg_map*)context, child);
    assert(result != NULL);

    return *((struct ntg_arrange_data*)result);
}

void ntg_arrange_context_set(ntg_arrange_context* context,
        const ntg_object* child,
        struct ntg_arrange_data data)
{
    __ntg_map_set((struct ntg_map*)context, child, &data);
}

struct ntg_arrange_output
{
    struct ntg_map _base;
};

ntg_arrange_output* ntg_arrange_output_new(const ntg_object* parent, SArena* arena)
{
    ntg_arrange_output* new = sarena_malloc(arena,
            sizeof(struct ntg_arrange_output), NULL);

    assert(new != NULL);

    __ntg_map_init__((struct ntg_map*)new,
            ntg_object_get_children(parent),
            sizeof(struct ntg_arrange_result),
            arena);

    return new;
}

void ntg_arrange_output_destroy(ntg_arrange_output* output)
{
    __ntg_map_deinit__((struct ntg_map*)output);
}

struct ntg_arrange_result ntg_arrange_output_get(const ntg_arrange_output* output,
        const ntg_object* child)
{
    void* result = __ntg_map_get((struct ntg_map*)output, child);
    assert(result != NULL);

    return *((struct ntg_arrange_result*)result);
}

void ntg_arrange_output_set(ntg_arrange_output* output,
        const ntg_object* child,
        struct ntg_arrange_result data)
{
    __ntg_map_set((struct ntg_map*)output, child, &data);
}
