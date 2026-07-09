#ifndef NTG_BORDER_STYLE_H
#define NTG_BORDER_STYLE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_border_style
{
    void (*draw_fn)(
            const void* data,
            struct ntg_xy size,
            struct ntg_insets border_size,
            ntg_object_tmp_drawing* out_drawing);
    void* data;
    void (*free_fn)(void* data);
};

/* Gets the built-in default border style.
 *
 * RETURN VALUE:
 * A pointer to a process-lifetime, read-only default style. */
NTG_API const struct ntg_border_style*
ntg_border_style_def();

struct ntg_border_style_9x_sym
{
    uint32_t top_left, top, top_right, right,
            bottom_right, bottom, bottom_left,
            left, padding;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* BASE */
/* ------------------------------------------------------ */

/* Calls the style-specific data destructor and clears the style. Passing `NULL`
 * has no effect. */
NTG_API void
ntg_border_style_deinit(struct ntg_border_style* style);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_border_style_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_border_style_deinit_(void* _style);

/* ------------------------------------------------------ */
/* 9X */
/* ------------------------------------------------------ */

/* Initializes a solid-color border style made of full space cells whose
 * background is `color`.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_monochrome(
        struct ntg_border_style* style,
        struct nt_color color,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a border style that uses one code point and graphics for all
 * border and padding positions. `overlay` selects overlay cells instead of full
 * cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_basic(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a border style that uses `cp` on corners and edges and a space
 * code point for padding positions. `overlay` selects overlay cells instead of
 * full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_basic_edge(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a Unicode single-line border style. `overlay` selects overlay
 * cells instead of full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_single(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a Unicode double-line border style. `overlay` selects overlay
 * cells instead of full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_double(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a Unicode single-line border style with rounded corners.
 * `overlay` selects overlay cells instead of full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_rounded(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a Unicode heavy-line border style. `overlay` selects overlay
 * cells instead of full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_heavy(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a Unicode dashed-line border style. `overlay` selects overlay
 * cells instead of full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_dashed(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes an ASCII border style. `overlay` selects overlay cells instead of
 * full cells.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_ascii(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a border style that leaves all underlying cells unchanged.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: construction of the underlying 9-slice style fails
 *   unexpectedly. */
NTG_API void
ntg_border_style_init_transparent(
        struct ntg_border_style* style,
        int* out_status);

/* ------------------------------------------------------ */

/* Initializes a 9-slice border style from the supplied virtual-cell type,
 * graphics, and symbols. The symbol table is copied; `NULL` selects blank
 * default symbols.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `style` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: style-specific data cannot be allocated. */
NTG_API void
ntg_border_style_init_custom_9x(
        struct ntg_border_style* style,
        ntg_vcell_type type,
        struct nt_gfx gfx,
        const struct ntg_border_style_9x_sym* symbols,
        int* out_status);

#endif // NTG_BORDER_STYLE_H
