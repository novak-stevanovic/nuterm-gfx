#include "object/def/ntg_container_def.h"
#include "object/ntg_container.h"

void __ntg_container_init__(ntg_container_t* container,
        ntg_nsize_fn nsize_fn, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn)
        
{
    __ntg_object_init__((ntg_object_t*)container, nsize_fn,
            constrain_fn, measure_fn, arrange_fn);
}

void __ntg_container_deinit__(ntg_container_t* container)
{
    __ntg_object_deinit__((ntg_object_t*)container);
}
