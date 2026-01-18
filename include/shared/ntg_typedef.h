#ifndef NTG_TYPEDEF_H
#define NTG_TYPEDEF_H

#include <stdbool.h>
#include <stddef.h>

#include "shared/sarena.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARE */
/* -------------------------------------------------------------------------- */

struct ntg_event;
struct nt_event;
struct ntg_xy;

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

typedef enum ntg_orient ntg_orient;
typedef enum ntg_align ntg_align;

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
typedef struct ntg_entity_vec ntg_entity_vec;
typedef struct ntg_entity_system ntg_entity_system;
typedef struct ntg_entity_map ntg_entity_map;
typedef struct ntg_event_obs_vec ntg_event_obs_vec;
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
typedef enum ntg_scene_event_mode ntg_scene_event_mode;

typedef struct ntg_object ntg_object;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_object_map ntg_object_map;
typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object_xy_map ntg_object_xy_map;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_tmp_object_drawing ntg_tmp_object_drawing;

typedef struct ntg_widget ntg_widget;
typedef struct ntg_widget_vec ntg_widget_vec;
typedef struct ntg_widget_map ntg_widget_map;
typedef struct ntg_widget_size_map ntg_widget_size_map;
typedef struct ntg_widget_xy_map ntg_widget_xy_map;

typedef struct ntg_decorator ntg_decorator;
typedef enum ntg_decorator_enable_mode ntg_decorator_enable_mode;
typedef struct ntg_def_padding ntg_def_padding;
typedef struct ntg_def_border ntg_def_border;

typedef struct ntg_label ntg_label;
typedef struct ntg_color_block ntg_color_block;
typedef struct ntg_prog_bar ntg_prog_bar;
typedef struct ntg_box ntg_box;
typedef struct ntg_border_box ntg_border_box;

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

typedef void* (*ntg_object_layout_data_init)(const ntg_object* object);
typedef void (*ntg_object_layout_data_deinit)(void* data, const ntg_object* object);

/* Measures how much space the object would require along one axis,
 * if the size is (un)constrained for the other axis. */
typedef struct ntg_object_measure (*ntg_object_measure_fn)(
        const ntg_object* object,
        void* _layout_data,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

/* Determines the children's sizes for given `orient`. */
typedef void (*ntg_object_constrain_fn)(
        const ntg_object* object,
        void* _layout_data,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena);

/* Determines children poss. */
typedef void (*ntg_object_arrange_fn)(
        const ntg_object* object,
        void* _layout_data,
        ntg_object_xy_map* out_pos_map,
        sarena* arena);

/* Creates the `object` drawing. */
typedef void (*ntg_object_draw_fn)(
        const ntg_object* object,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena);

typedef bool (*ntg_widget_event_fn)(
        ntg_widget* widget,
        struct nt_event event,
        ntg_loop_ctx* ctx);

typedef void* (*ntg_widget_layout_data_init)(const ntg_widget* widget);
typedef void (*ntg_widget_layout_data_deinit)(void* data, const ntg_widget* widget);

typedef struct ntg_object_measure (*ntg_widget_measure_fn)(
        const ntg_widget* widget,
        void* _layout_data,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

typedef void (*ntg_widget_constrain_fn)(
        const ntg_widget* widget,
        void* _layout_data,
        ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena);

typedef void (*ntg_widget_arrange_fn)(
        const ntg_widget* widget,
        void* _layout_data,
        ntg_widget_xy_map* out_pos_map,
        sarena* arena);

typedef void (*ntg_widget_draw_fn)(
        const ntg_widget* widget,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
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

#endif // NTG_TYPEDEF_H
