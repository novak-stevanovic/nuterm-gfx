#ifndef NTG_SHARED_H
#define NTG_SHARED_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

#include "thirdparty/sarena.h"
#include "thirdparty/genc.h"
#include "nt_gfx.h"
#include "nt_event.h"

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

/* Shared optional values */

GENC_OPT_INLINE_DEF(ntg_int_opt, int)
GENC_OPT_INLINE_DEF(ntg_uint_opt, unsigned int)
GENC_OPT_INLINE_DEF(ntg_uint8_opt, uint8_t)
GENC_OPT_INLINE_DEF(ntg_size_opt, size_t)

/* ========================================================================== */
/* FORWARD DECLARATIONS */
/* ========================================================================== */

struct ntg_event;
struct nt_event;
struct ntg_widget_measure;

/* ------------------------------------------------------ */
/* BASE */
/* ------------------------------------------------------ */

typedef struct ntg_xy ntg_xy;
typedef struct ntg_dxy ntg_dxy;
typedef struct ntg_insets ntg_insets;
typedef struct ntg_cell_grid ntg_cell_grid;
typedef struct ntg_vcell_grid ntg_vcell_grid;
typedef struct ntg_cell_vecgrid ntg_cell_vecgrid;
typedef struct ntg_vcell_vecgrid ntg_vcell_vecgrid;
typedef struct ntg_type ntg_type;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_event_binding ntg_event_binding;
typedef struct ntg_garbage ntg_garbage;
typedef struct ntg_object ntg_object;

/* ------------------------------------------------------ */
/* CORE */
/* ------------------------------------------------------ */

typedef struct ntg_platform ntg_platform;

typedef struct ntg_task_runner ntg_task_runner;
typedef struct ntg_task_cancel_token ntg_task_cancel_token;

typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_db_renderer ntg_db_renderer;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_stage_draw ntg_stage_draw;

typedef struct ntg_scene ntg_scene;
typedef struct ntg_fcs_manager ntg_fcs_manager;
typedef struct ntg_widget ntg_widget;
typedef struct ntg_border_style ntg_border_style;
typedef struct ntg_anchor_policy ntg_anchor_policy;
typedef struct ntg_float ntg_float;
typedef struct ntg_sidefloat ntg_sidefloat;
typedef struct ntg_border_9x ntg_border_9x;
typedef struct ntg_widget_size_map ntg_widget_size_map;
typedef struct ntg_widget_pos_map ntg_widget_pos_map;
typedef struct ntg_widget_draw ntg_widget_draw;
typedef struct ntg_tmp_widget_draw ntg_tmp_widget_draw;
typedef struct ntg_widget_tmp_draw ntg_widget_tmp_draw;

typedef struct ntg_widget_vec ntg_widget_vec;

typedef struct ntg_text ntg_text;
typedef struct ntg_label ntg_label;
typedef struct ntg_button ntg_button;
typedef struct ntg_clr_block ntg_clr_block;
typedef struct ntg_prog_bar ntg_prog_bar;
typedef struct ntg_box ntg_box;
typedef struct ntg_panel ntg_panel;

/* ========================================================================== */
/* OP */
/* ========================================================================== */

static inline size_t ntg_clamp_size(size_t min, size_t mid, size_t max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

static inline int ntg_clamp_int(int min, int mid, int max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

static inline size_t ntg_max2_size(size_t x, size_t y)
{
    return (x > y) ? x : y;
}

static inline size_t ntg_min2_size(size_t x, size_t y)
{
    return (x > y) ? y : x;
}

static inline unsigned int ntg_max2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? x : y;
}

static inline unsigned int ntg_min2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? y : x;
}

static inline size_t ntg_max3_size(size_t x, size_t y, size_t z)
{
    size_t tmp = ntg_max2_size(x, y);

    return ntg_max2_size(tmp, z);
}

static inline size_t ntg_min3_size(size_t x, size_t y, size_t z)
{
    size_t tmp = ntg_min2_size(x, y);

    return ntg_min2_size(tmp, z);
}

static inline ssize_t ntg_max2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? x : y;
}


static inline ssize_t ntg_min2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? y : x;
}

static inline double ntg_min2_double(double x, double y)
{
    return (x > y) ? y : x;
}

static inline double ntg_max2_double(double x, double y)
{
    return (x < y) ? y : x;
}

static inline bool ntg_double_are_eql(double a, double b)
{
    const double eps = 1e-9;
    double diff = a - b;

    return (diff > -eps) && (diff < eps);
}

static inline size_t ntg_sub2_size(size_t x, size_t y)
{
    return (x > y) ? x - y : 0;
}

static inline unsigned int ntg_sub2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? x - y : 0;
}

static inline unsigned long long
ntg_sub2_ull(unsigned long long int x, unsigned long long int y)
{
    return (x > y) ? x - y : 0;
}

static inline size_t ntg_sub3_size(size_t x, size_t y, size_t z)
{
    return ((x >= (y + z)) ? ((x - y) - z) : 0);
}

#endif // NTG_SHARED_H
