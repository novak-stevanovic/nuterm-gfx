#ifndef NTG_SHARED_H
#define NTG_SHARED_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include "thirdparty/sarena.h"

#ifdef NTG_EXPORT
#define NTG_API __attribute__((visibility("default")))
#else
#define NTG_API
#endif

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FORWARD DECLARATIONS */
/* ========================================================================== */

struct ntg_event;
struct nt_event;
struct nt_mouse;
struct nt_key;
struct ntg_xy;
struct ntg_insets;
struct ntg_object_measure;

/* ------------------------------------------------------ */
/* BASE */
/* ------------------------------------------------------ */

typedef struct ntg_cell_grid ntg_cell_grid;
typedef struct ntg_vcell_grid ntg_vcell_grid;
typedef struct ntg_cell_vecgrid ntg_cell_vecgrid;
typedef struct ntg_vcell_vecgrid ntg_vcell_vecgrid;
typedef struct ntg_type ntg_type;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_event_binding ntg_event_binding;
typedef struct ntg_cleanup_batch ntg_cleanup_batch;
typedef struct ntg_keybind ntg_keybind;

/* ------------------------------------------------------ */
/* CORE */
/* ------------------------------------------------------ */

typedef struct ntg_platform ntg_platform;

typedef struct ntg_task_runner ntg_task_runner;
typedef struct ntg_task_cancel_token ntg_task_cancel_token;

typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_default_renderer ntg_default_renderer;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_stage_drawing ntg_stage_drawing;

typedef struct ntg_scene ntg_scene;
typedef struct ntg_fcs_manager ntg_fcs_manager;
typedef struct ntg_fcs_scope_list ntg_fcs_scope_list;
typedef struct ntg_fcs_scope ntg_fcs_scope;
typedef struct ntg_object ntg_object;
typedef struct ntg_border_style ntg_border_style;
typedef struct ntg_anchor_policy ntg_anchor_policy;
typedef struct ntg_float ntg_float;
typedef struct ntg_sidefloat ntg_sidefloat;
typedef struct ntg_border_9x ntg_border_9x;
typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object_pos_map ntg_object_pos_map;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_tmp_object_drawing ntg_tmp_object_drawing;
typedef struct ntg_object_tmp_drawing ntg_object_tmp_drawing;

typedef struct ntg_widget ntg_widget;
typedef struct ntg_widget_vec ntg_widget_vec;

typedef struct ntg_text ntg_text;
typedef struct ntg_label ntg_label;
typedef struct ntg_button ntg_button;
typedef struct ntg_clr_block ntg_clr_block;
typedef struct ntg_prog_bar ntg_prog_bar;
typedef struct ntg_box ntg_box;
typedef struct ntg_main_panel ntg_main_panel;

#endif // NTG_SHARED_H
