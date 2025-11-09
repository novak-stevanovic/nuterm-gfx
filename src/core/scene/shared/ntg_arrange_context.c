#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "core/scene/shared/ntg_arrange_context.h"
#include "shared/_ntg_vec_map.h"

struct ntg_arrange_context
{
    ntg_vec_map __base;
};

static int __key_cmp_fn(const void* _it_pair, const void* key)
{
    assert(_it_pair != NULL);
    assert(key != NULL);

    struct ntg_vec_map_pair* it_pair = (struct ntg_vec_map_pair*)_it_pair;

    return memcmp(it_pair, key, sizeof(void*));
}

ntg_arrange_context* ntg_arrange_context_new()
{
    ntg_arrange_context* new = (ntg_arrange_context*)malloc(
            sizeof(ntg_arrange_context));
    assert(new != NULL);

    __ntg_vec_map_init__(
            &new->__base,
            sizeof(ntg_drawable*),
            sizeof(struct ntg_arrange_data),
            __key_cmp_fn);

    return new;
}

void ntg_arrange_context_destroy(ntg_arrange_context* context)
{
    assert(context != NULL);

    __ntg_vec_map_deinit__(&context->__base);

    free(context);
}

struct ntg_arrange_data ntg_arrange_context_get(
        const ntg_arrange_context* context,
        const ntg_drawable* child)
{
    assert(context != NULL);
    assert(child != NULL);

    void* data = ntg_vec_map_get(&context->__base, &child);
    assert(data != NULL);

    return *((struct ntg_arrange_data*)data);
}

void ntg_arrange_context_set(
        ntg_arrange_context* context,
        const ntg_drawable* child,
        struct ntg_arrange_data data)
{
    assert(context != NULL);
    assert(child != NULL);

    ntg_vec_map_add(&context->__base, &child, &data);
}
