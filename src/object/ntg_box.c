#include "object/def/ntg_box_def.h"
#include "object/ntg_box.h"

#include "object/ntg_object_fwd.h"

// TODO

static void __nsize_fn(ntg_object_t* object)
{
}

static void __constrain_fn(ntg_object_t* object)
{
}

static void __measure_fn(ntg_object_t* object)
{
}

static void __arrange_fn(ntg_object_t* object)
{
}

void __ntg_box_init__(ntg_box_t* box)
{
    __ntg_container_init__((ntg_container_t*)box, __nsize_fn, __constrain_fn,
            __measure_fn, __arrange_fn);
}

void __ntg_box_deinit__(ntg_box_t* box)
{
    __ntg_container_deinit__((ntg_container_t*)box);
}

