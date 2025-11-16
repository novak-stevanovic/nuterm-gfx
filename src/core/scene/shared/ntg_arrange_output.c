#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "core/scene/shared/_ntg_arrange_output.h"
#include "shared/_ntg_vec_map.h"

struct ntg_arrange_output
{
    ntg_vec_map __base;
};

static int __key_cmp_fn(const void* _it_pair, const void* key)
{
    assert(_it_pair != NULL);
    assert(key != NULL);

    struct ntg_vec_map_pair* it_pair = (struct ntg_vec_map_pair*)_it_pair;

    return memcmp(it_pair->key, key, sizeof(void*));
}

ntg_arrange_output* ntg_arrange_output_new()
{
    ntg_arrange_output* new = (ntg_arrange_output*)malloc(
            sizeof(ntg_arrange_output));
    assert(new != NULL);

    __ntg_vec_map_init__(
            &new->__base,
            sizeof(ntg_drawable*),
            sizeof(struct ntg_arrange_result),
            __key_cmp_fn);

    return new;
}

void ntg_arrange_output_destroy(ntg_arrange_output* output)
{
    assert(output != NULL);

    __ntg_vec_map_deinit__(&output->__base);

    free(output);
}

struct ntg_arrange_result ntg_arrange_output_get(
        const ntg_arrange_output* output,
        const ntg_drawable* child)
{
    assert(output != NULL);
    assert(child != NULL);

    void* data = ntg_vec_map_get(&output->__base, &child);
    assert(data != NULL);

    return *((struct ntg_arrange_result*)data);
}

void ntg_arrange_output_set(
        ntg_arrange_output* output,
        const ntg_drawable* child,
        struct ntg_arrange_result data)
{
    assert(output != NULL);
    assert(child != NULL);

    ntg_vec_map_add(&output->__base, &child, &data);
}
