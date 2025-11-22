#ifndef _NTG_OBJECT_FX_H_
#define _NTG_OBJECT_FX_H_

#include <stdbool.h>

typedef struct ntg_object ntg_object;
typedef struct ntg_drawing ntg_drawing;
typedef struct ntg_object_fx ntg_object_fx;

/* Returns if the effect has been applied successfully. */
typedef bool (*ntg_object_fx_apply_fn)(
        const ntg_object* object,
        const ntg_object_fx* fx,
        void* object_fx_interface);

typedef void (*ntg_object_fx_deinit_fn)(ntg_object_fx* fx);

struct ntg_object_fx
{
    unsigned int _object_type; // read-only

    ntg_object_fx_apply_fn __apply_fn;

    /* If the fx doesn't use any resources, this should be NULL */
    ntg_object_fx_deinit_fn __deinit_fn;
};

void __ntg_object_fx_init__(
        ntg_object_fx* fx,
        unsigned int object_type,
        ntg_object_fx_apply_fn apply_fn,
        ntg_object_fx_deinit_fn deinit_fn);
void ntg_object_fx_destroy(ntg_object_fx* fx);

/* Returns if the effect has been applied successfully. */
bool ntg_object_fx_apply(
        const ntg_object* object,
        const ntg_object_fx* fx,
        void* object_fx_interface);

#endif // _NTG_OBJECT_FX_H_
