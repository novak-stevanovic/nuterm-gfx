#ifndef NTG_CLEANUP_H
#define NTG_CLEANUP_H

#include "shared/ntg_shared.h"

struct ntg_cleanup_pair
{
    void* object;
    void (*cleanup_fn)(void* object);
};

ntg_cleanup_batch* ntg_cleanup_batch_new();
void ntg_cleanup_batch_destroy(ntg_cleanup_batch* batch);

void ntg_cleanup_batch_add(ntg_cleanup_batch* batch, struct ntg_cleanup_pair pair);
void ntg_cleanup_batch_add_array(
        ntg_cleanup_batch* batch,
        const struct ntg_cleanup_pair* pairs,
        size_t count);

#define NTG_CLEANUP_CAST_FN(fn_name, fn_called)                                \
static void fn_name(void* data)                                                \
{                                                                              \
    fn_called(data);                                                           \
}                                                                              \

#define NTG_CLEANUP_PAIR(obj, c_fn)                                            \
    ((struct ntg_cleanup_pair) {                                               \
        .object = (obj),                                                       \
        .cleanup_fn = c_fn                                                     \
    })

#define ntg_cleanup_batch_add_many(batch, ...)                                                      \
    ntg_cleanup_batch_add_array(                                                                    \
        (batch),                                                                                    \
        (const struct ntg_cleanup_pair[]){ __VA_ARGS__ },                                           \
        sizeof((const struct ntg_cleanup_pair[]){ __VA_ARGS__ }) / sizeof(struct ntg_cleanup_pair)) \

#endif // NTG_CLEANUP_H
