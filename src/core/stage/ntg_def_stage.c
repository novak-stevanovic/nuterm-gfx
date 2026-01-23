#include "ntg.h"
#include <assert.h>

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_stage* ntg_def_stage_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_STAGE,
        .deinit_fn = (ntg_entity_deinit_fn)ntg_def_stage_deinit,
        .system = system
    };

    ntg_def_stage* new = (ntg_def_stage*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_def_stage_init(ntg_def_stage* stage)
{
    assert(stage != NULL);

    ntg_stage_init((ntg_stage*)stage, _ntg_def_stage_compose_fn);

    stage->data = NULL;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_stage_deinit(ntg_def_stage* stage)
{
    stage->data = NULL;

    ntg_stage_deinit((ntg_stage*)stage);
}

struct collect_data
{
    ntg_object** array;
    size_t counter;
    sarena* arena;
};

static void collect_by_z_tree(ntg_object* object, void* _data)
{
    struct collect_data* data = _data;
    ntg_object** array = data->array;
    size_t* counter = &(data->counter);
    sarena* arena = data->arena;

    array[*counter] = object;
    (*counter)++;

    const ntg_object_vec* children = &(object->_children);

    if(children->size == 0) return;

    ntg_object** tmp_children = sarena_malloc(arena, sizeof(void*) *
                                              children->size);
    
    size_t i, j;

    for(i = 0; i < children->size; i++)
        tmp_children[i] = children->data[i];

    ntg_object* tmp_obj;
    for(i = 0; i < children->size - 1; i++)
    {
        for(j = i + 1; j < children->size; j++)
        {
            if((tmp_children[j])->_z_index < (tmp_children[i])->_z_index)
            {
                tmp_obj = tmp_children[i];
                tmp_children[i] = tmp_children[j];
                tmp_children[j] = tmp_obj;
            }
        }
    }

    for(i = 0; i < children->size; i++)
    {
        collect_by_z_tree(tmp_children[i], _data);
    }
}

static void draw(ntg_object* object, ntg_def_stage* stage)
{
    ntg_stage* _stage = (ntg_stage*)stage;
    const ntg_object_drawing* drawing = &(object->_drawing);
    struct ntg_xy size = object->_size;
    struct ntg_xy pos = ntg_object_get_pos_abs(object);

    ntg_object_drawing_place_(drawing, ntg_xy(0, 0), size, &_stage->_drawing, pos);
}

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size,
                               sarena* arena)
{
    assert(_stage != NULL);

    ntg_def_stage* stage = (ntg_def_stage*)_stage;

    size_t i, j;
    struct ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_stage_drawing_at_(&_stage->_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_cell_default();
        }
    }

    if((_stage->_scene != NULL) && (_stage->_scene->_root != NULL))
    {
        if(_stage->_scene->_root != NULL)
        {
            ntg_object* root = ntg_widget_get_group_root_(_stage->_scene->_root);

            size_t tree_size = ntg_object_get_tree_size(root);
            ntg_object** array = sarena_malloc(arena, sizeof(void*) *
                                               tree_size);
            struct collect_data data = {
                .array = array,
                .counter = 0,
                .arena = arena
            };
            collect_by_z_tree(root, &data);

            for(i = 0; i < tree_size; i++) draw(array[i], stage);
        }
    }
}
