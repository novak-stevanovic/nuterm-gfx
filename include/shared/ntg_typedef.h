#ifndef _NTG_TYPEDEF_H_
#define _NTG_TYPEDEF_H_

#include <stdbool.h>
#include <stddef.h>

#include "sarena.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARE */
/* -------------------------------------------------------------------------- */

struct ntg_event;
struct nt_event;
struct ntg_xy;
struct ntg_object_measure_ctx;
struct ntg_object_measure_out;
struct ntg_object_constrain_ctx;
struct ntg_object_constrain_out;
struct ntg_object_arrange_ctx;
struct ntg_object_arrange_out;
struct ntg_object_draw_ctx;
struct ntg_object_draw_out;;

/* -------------------------------------------------------------------------- */
/* STRUCT & ENUM */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* SHARED */
/* ------------------------------------------------------ */

typedef struct ntg_vec ntg_vec;
typedef struct ntg_vecgrid ntg_vecgrid;
typedef struct ntg_fwd_list ntg_fwd_list;

typedef unsigned int ntg_status;

typedef enum ntg_orientation ntg_orientation;
typedef enum ntg_alignment ntg_alignment;

/* ------------------------------------------------------ */
/* BASE */
/* ------------------------------------------------------ */

typedef struct ntg_cell_grid ntg_cell_grid;
typedef struct ntg_vcell_grid ntg_vcell_grid;
typedef struct ntg_cell_vecgrid ntg_cell_vecgrid;
typedef struct ntg_vcell_vecgrid ntg_vcell_vecgrid;
typedef enum ntg_vcell_type ntg_vcell_type;

/* ------------------------------------------------------ */
/* CORE */
/* ------------------------------------------------------ */

typedef struct ntg_entity ntg_entity;
typedef struct ntg_entity_system ntg_entity_system;
typedef struct ntg_entity_map ntg_entity_map;
typedef struct ntg_event_observe_vec ntg_event_obs_vec;
typedef struct ntg_entity_vec ntg_entity_vec;
typedef struct ntg_const_entity_vec ntg_const_entity_vec;
typedef struct ntg_entity_type ntg_entity_type;

typedef struct ntg_loop ntg_loop;
typedef struct ntg_loop_ctx ntg_loop_ctx;
typedef enum ntg_loop_event_mode ntg_loop_event_mode;
typedef struct ntg_task_runner ntg_task_runner;
typedef struct ntg_task_list ntg_task_list;
typedef struct ntg_ptask_list ntg_ptask_list;
typedef struct ntg_platform ntg_platform;

typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_def_renderer ntg_def_renderer;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_def_stage ntg_def_stage;
typedef struct ntg_stage_drawing ntg_stage_drawing;
typedef enum ntg_stage_event_mode ntg_stage_event_mode;

typedef struct ntg_scene ntg_scene;
typedef struct ntg_def_scene ntg_def_scene;
typedef struct ntg_scene_graph ntg_scene_graph;
typedef struct ntg_focuser ntg_focuser;
typedef struct ntg_single_focuser ntg_single_focuser;
typedef enum ntg_scene_event_mode ntg_scene_event_mode;

typedef struct ntg_object ntg_object;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_const_object_vec ntg_const_object_vec;
typedef struct ntg_object_measure_map ntg_object_measure_map;
typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object_xy_map ntg_object_xy_map;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_object_map ntg_object_map;
typedef struct ntg_label ntg_label;
typedef struct ntg_color_block ntg_color_block;
typedef struct ntg_prog_bar ntg_prog_bar;
typedef struct ntg_box ntg_box;
typedef struct ntg_border_box ntg_border_box;
typedef struct ntg_padding ntg_padding;
typedef enum ntg_padding_enable_mode ntg_padding_enable_mode;
typedef struct ntg_def_padding ntg_def_padding;
typedef struct ntg_def_border ntg_def_border;

/* -------------------------------------------------------------------------- */
/* FUNCTION */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* SHARED */
/* ------------------------------------------------------ */

typedef int (*ntg_vec_cmp_fn)(const void* data1, const void* data2);

/* ------------------------------------------------------ */
/* BASE */
/* ------------------------------------------------------ */

typedef void (*ntg_entity_deinit_fn)(ntg_entity* entity);
typedef void (*ntg_event_handler_fn)(ntg_entity* observer, struct ntg_event event);

/* ------------------------------------------------------ */
/* CORE */
/* ------------------------------------------------------ */

/* LOOP ------------------------------------------------- */

typedef bool (*ntg_loop_event_fn)(
        ntg_loop_ctx* ctx,
        struct nt_event event);

/* OBJECT ----------------------------------------------- */

typedef bool (*ntg_object_event_fn)(
        ntg_object* object,
        struct nt_event event,
        ntg_loop_ctx* ctx);

/* Dynamically allocates and initializes an object that will be used in the
 * layout process for `object`. */
typedef void* (*ntg_object_layout_init_fn)(const ntg_object* object);

/* Frees the resources associated with `layout_data` object. */
typedef void (*ntg_object_layout_deinit_fn)(
        const ntg_object* object,
        void* layout_data);

/* Measures how much space the object would require along one axis,
 * if the size is constrained for the other axis. */
typedef struct ntg_object_measure (*ntg_object_measure_fn)(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

/* Determines the children's sizes for given `orientation` and `size`. */
typedef void (*ntg_object_constrain_fn)(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena);

/* Determines children positions for given `size`. */
typedef void (*ntg_object_arrange_fn)(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena);

/* Creates the `object` drawing. */
typedef void (*ntg_object_draw_fn)(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena);

typedef void (*ntg_label_post_draw_fn)(
        ntg_label* label,
        ntg_object_drawing* out_drawing,
        struct ntg_xy size,
        sarena* arena);

/* SCENE ------------------------------------------------ */

/* Performs logical layout of the scene. */
typedef void (*ntg_scene_layout_fn)(
        ntg_scene* scene,
        struct ntg_xy size,
        sarena* arena);

typedef bool (*ntg_scene_event_fn)(
        ntg_scene* scene,
        struct nt_event event,
        ntg_loop_ctx* loop_ctx);

typedef bool (*ntg_focuser_dispatch_fn)(
        ntg_focuser* focuser,
        struct nt_event event,
        ntg_loop_ctx* ctx);

/* STAGE ------------------------------------------------ */

/* Composes the scene's logical drawing into an ntg_stage_drawing */
typedef void (*ntg_stage_compose_fn)(
        ntg_stage* stage,
        struct ntg_xy size,
        sarena* arena);

typedef bool (*ntg_stage_event_fn)(
        ntg_stage* stage,
        struct nt_event event,
        ntg_loop_ctx* loop_ctx);

/* RENDERER --------------------------------------------- */

/* If `stage_drawing` is NULL, render empty */
typedef void (*ntg_renderer_render_fn)(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

#endif // _NTG_TYPEDEF_H_
