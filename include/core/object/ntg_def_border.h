// #ifndef _NTG_BORDER_H_
// #define _NTG_BORDER_H_
//
// #include "core/object/ntg_padding.h"
//
// typedef struct ntg_def_border ntg_def_border;
//
// struct ntg_def_border_opts
// {
//     ntg_cell top_left, top, top_right,
//              right, bottom_right, bottom,
//              bottom_left, left, padding;
// };
//
// struct ntg_def_border
// {
//     ntg_padding __base;
//
//     struct ntg_def_border_opts __opts;
// };
//
// void __ntg_def_border_init__(
//         ntg_def_border* def_border,
//         struct ntg_def_border_opts opts,
//         struct ntg_padding_width init_width,
//         void* data);
// void __ntg_def_border_deinit__(ntg_def_border* def_border);
//
// #endif // _NTG_BORDER_H_
