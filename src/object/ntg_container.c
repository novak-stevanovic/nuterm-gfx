#include "object/def/ntg_container_def.h"
#include "object/ntg_object.h"
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

void _ntg_container_add_child(ntg_container_t* container, ntg_object_t* object)
{
    if(container == NULL) return;

    ntg_object_t* _container = (ntg_object_t*)container;

    size_t _status;
    ntg_object_vec_append(&_container->_children, object);

    object->_parent = _container;
}

void _ntg_container_remove_child(ntg_container_t* container, ntg_object_t* object)
{
    if(container == NULL) return;

    ntg_object_t* _container = (ntg_object_t*)container;

    ntg_object_vec_remove(&_container->_children, object);

    object->_parent = NULL;
}
