#ifndef NTG_SHARED_H
#define NTG_SHARED_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include "thirdparty/sarena.h"

#ifdef NTG_EXPORT
#define NTG_API __attribute__((visibility("default")))
#else
#define NTG_API
#endif

/* ========================================================================== */
/* FORWARD DECLARE */
/* ========================================================================== */

struct ntg_event;
struct nt_event;
struct ntg_xy;
struct ntg_object_measure;

/* -------------------------------------------------------------------------- */
/* SHARED */
/* -------------------------------------------------------------------------- */

typedef unsigned int ntg_status;

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

typedef struct ntg_cell_grid ntg_cell_grid;
typedef struct ntg_vcell_grid ntg_vcell_grid;
typedef struct ntg_cell_vecgrid ntg_cell_vecgrid;
typedef struct ntg_vcell_vecgrid ntg_vcell_vecgrid;
typedef enum ntg_vcell_type ntg_vcell_type;
typedef enum ntg_orient ntg_orient;
typedef enum ntg_align ntg_align;
typedef struct ntg_vecgrid ntg_vecgrid;
typedef struct ntg_type ntg_type;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_event_binding ntg_event_binding;
typedef struct ntg_cleanup_batch ntg_cleanup_batch;

/* -------------------------------------------------------------------------- */
/* CORE */
/* -------------------------------------------------------------------------- */

typedef struct ntg_loop ntg_loop;
typedef enum ntg_loop_status ntg_loop_status;
typedef enum ntg_loop_stop_mode ntg_loop_stop_mode;
typedef enum ntg_loop_exit_status ntg_loop_exit_status;
typedef struct ntg_task_runner ntg_task_runner;
typedef struct ntg_task_list ntg_task_list;
typedef struct ntg_ptask_list ntg_ptask_list;
typedef struct ntg_platform ntg_platform;

typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_def_renderer ntg_def_renderer;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_stage_drawing ntg_stage_drawing;

typedef struct ntg_scene ntg_scene;
typedef struct ntg_scene_layer ntg_scene_layer;

typedef struct ntg_object ntg_object;
typedef enum ntg_object_dcr_enable ntg_object_dcr_enable;
typedef enum ntg_object_type ntg_object_type;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object_pos_map ntg_object_pos_map;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_object_tmp_drawing ntg_object_tmp_drawing;

typedef struct ntg_widget ntg_widget;
typedef struct ntg_widget_vec ntg_widget_vec;

typedef struct ntg_label ntg_label;
typedef enum ntg_label_wrap ntg_label_wrap;
typedef enum ntg_label_align ntg_label_align;
typedef struct ntg_color_block ntg_color_block;
typedef struct ntg_prog_bar ntg_prog_bar;
typedef struct ntg_box ntg_box;
typedef struct ntg_border_box ntg_border_box;

typedef void* (*ntg_object_lctx_init_fn)(
        const ntg_object* object);

typedef void (*ntg_object_lctx_deinit_fn)(
        void* lctx,
        const ntg_object* object);

typedef struct ntg_object_measure (*ntg_object_hmeasure_fn)(
        const ntg_object* object,
        void* lctx,
        sarena* arena);

typedef struct ntg_object_measure (*ntg_object_vmeasure_fn)(
        const ntg_object* object,
        void* lctx,
        sarena* arena);

typedef void (*ntg_object_hconstrain_fn)(
        const ntg_object* object,
        ntg_object_size_map* out_size_map,
        void* lctx,
        sarena* arena);

typedef void (*ntg_object_vconstrain_fn)(
        const ntg_object* object,
        ntg_object_size_map* out_size_map,
        void* lctx,
        sarena* arena);

typedef void (*ntg_object_arrange_fn)(
        const ntg_object* object,
        ntg_object_pos_map* out_pos_map,
        void* lctx,
        sarena* arena);

typedef void (*ntg_object_draw_fn)(
        const ntg_object* object,
        ntg_object_tmp_drawing* out_drawing,
        void* lctx,
        sarena* arena);

/* If `stage_drawing` is NULL, render empty */
typedef void (*ntg_renderer_render_fn)(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

#endif // NTG_SHARED_H
