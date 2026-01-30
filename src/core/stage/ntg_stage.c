// #include "ntg.h"
// #include <assert.h>
// #include "shared/ntg_shared_internal.h"

// struct collect_data
// {
//     ntg_object** array;
//     size_t counter;
//     sarena* arena;
// };
// 
// static void collect_by_z_tree(ntg_object* object, void* _data)
// {
//     struct collect_data* data = _data;
//     ntg_object** array = data->array;
//     size_t* counter = &(data->counter);
//     sarena* arena = data->arena;
// 
//     array[*counter] = object;
//     (*counter)++;
// 
//     size_t children_size = object->_children.size;
// 
//     if(children_size == 0) return;
// 
//     ntg_object** sorted_children = sarena_malloc(arena, sizeof(ntg_object*) *
//             children_size);
//     assert(sorted_children);
//     ntg_object_get_children_by_z(object, sorted_children);
// 
//     size_t i;
//     for(i = 0; i < children_size; i++)
//     {
//         collect_by_z_tree(sorted_children[i], _data);
//     }
// }
// 
// static void draw(ntg_object* object, ntg_stage* stage)
// {
//     const ntg_object_drawing* drawing = &(object->_drawing);
//     struct ntg_xy size = object->_size;
//     struct ntg_dxy _pos = ntg_object_map_to_scene_space(object, ntg_dxy(0, 0));
//     struct ntg_xy pos = ntg_xy_from_dxy(_pos);
// 
//     ntg_object_drawing_place_(drawing, ntg_xy(0, 0), size, &stage->_drawing, pos);
// }
// 
// static void init_default(ntg_stage* stage)
// {
//     stage->_scene = NULL;
//     stage->__compose = true;
//     stage->_drawing = (struct ntg_stage_drawing) {0};
//     stage->__on_key_fn = ntg_stage_dispatch_key;
//     stage->__on_mouse_fn = ntg_stage_dispatch_mouse;
//     stage->_loop = NULL;
//     stage->data = NULL;
// }
// 
// void ntg_stage_init(ntg_stage* stage)
// {
//     assert(stage != NULL);
// 
//     init_default(stage);
// 
//     ntg_stage_drawing_init(&stage->_drawing);
// }
// 
// void ntg_stage_deinit(ntg_stage* stage)
// {
//     if(stage->_loop)
//     {
//         ntg_loop_set_stage(stage->_loop, NULL);
//     }
// 
//     if(stage->_scene)
//     {
//         ntg_stage_set_scene(stage, NULL);
//     }
// 
//     ntg_stage_drawing_deinit(&stage->_drawing);
// 
//     init_default(stage);
// }
// 
// 
// void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size, sarena* arena)
// {
//     assert(stage != NULL);
// 
//     struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
//     ntg_stage_drawing_set_size(&stage->_drawing, size, size_cap);
// 
//     bool compose = stage->__compose;
//     if(stage->_scene != NULL)
//         compose |= ntg_scene_layout(stage->_scene, size, arena);
// 
//     if(compose)
//     {
//         size_t i, j;
//         struct ntg_cell* it_cell;
//         for(i = 0; i < size.y; i++)
//         {
//             for(j = 0; j < size.x; j++)
//             {
//                 it_cell = ntg_stage_drawing_get_(&stage->_drawing, ntg_xy(j, i));
//                 (*it_cell) = ntg_cell_default();
//             }
//         }
// 
//         // if((stage->_scene != NULL) && (stage->_scene->_root != NULL))
//         // {
//         //     ntg_object* root = ntg_widget_get_group_root_(stage->_scene->_root);
// 
//         //     size_t tree_size = ntg_object_get_tree_size(root);
//         //     ntg_object** array = sarena_malloc(arena, sizeof(ntg_object*) *
//         //             tree_size);
//         //     struct collect_data data = {
//         //         .array = array,
//         //         .counter = 0,
//         //         .arena = arena
//         //     };
//         //     collect_by_z_tree(root, &data);
// 
//         //     for(i = 0; i < tree_size; i++) draw(array[i], stage);
//         // }
//     }
// }
// 
// void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
// {
//     assert(stage != NULL);
//     if(stage->_scene == scene) return;
// 
//     stage->__compose = true;
// 
//     ntg_scene* old_scene = stage->_scene;
// 
//     if(old_scene)
//     {
//         _ntg_scene_set_stage(old_scene, NULL);
//     }
// 
//     stage->_scene = scene;
// 
//     if(scene)
//     {
//         ntg_stage* old_stage = scene->_stage;
// 
//         if(old_stage)
//         {
//             ntg_stage_set_scene(old_stage, NULL);
//         }
// 
//         _ntg_scene_set_stage(scene, stage);
//     }
// }
// 
// bool ntg_stage_dispatch_key(ntg_stage* stage, struct nt_key_event key)
// {
//     assert(stage != NULL);
// 
//     if(stage->_scene)
//         return ntg_scene_on_key(stage->_scene, key);
//     else
//         return false;
// }
// 
// bool ntg_stage_dispatch_mouse(ntg_stage* stage, struct nt_mouse_event mouse)
// {
//     assert(stage != NULL);
// 
//     if(stage->_scene)
//         return ntg_scene_on_mouse(stage->_scene, mouse);
//     else
//         return false;
// }
// 
// void ntg_stage_set_on_key_fn(ntg_stage* stage,
//         bool (*on_key_fn)(ntg_stage* stage, struct nt_key_event key))
// {
//     assert(stage != NULL);
//     
//     stage->__on_key_fn = on_key_fn;
// }
// 
// bool ntg_stage_on_key(ntg_stage* stage, struct nt_key_event key)
// {
//     assert(stage != NULL);
// 
//     if(stage->__on_key_fn)
//         return stage->__on_key_fn(stage, key);
//     else
//         return false;
// }
// 
// void ntg_stage_set_on_mouse_fn(ntg_stage* stage,
//         bool (*on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse))
// {
//     assert(stage != NULL);
// 
//     stage->__on_mouse_fn = on_mouse_fn;
// }
// 
// bool ntg_stage_on_mouse(ntg_stage* stage, struct nt_mouse_event mouse)
// {
//     assert(stage != NULL);
// 
//     if(stage->__on_mouse_fn)
//         return stage->__on_mouse_fn(stage, mouse);
//     else
//         return false;
// }
// 
// /* -------------------------------------------------------------------------- */
// /* INTERNAL/PROTECTED */
// /* -------------------------------------------------------------------------- */
// 
// void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop)
// {
//     assert(stage != NULL);
// 
//     stage->_loop = loop;
// }
