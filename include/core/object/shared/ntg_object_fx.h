#ifndef _NTG_OBJECT_FX_H_
#define _NTG_OBJECT_FX_H_

#include<stdbool.h>

typedef struct ntg_object ntg_object;
typedef struct ntg_drawing ntg_drawing;
struct ntg_object_fx;

typedef bool (*ntg_object_fx_apply_fn)(
        const ntg_object* object,
        const struct ntg_object_fx fx,
        ntg_drawing* drawing);

struct ntg_object_fx
{
    ntg_object_fx_apply_fn apply_fn;
    void* data; /* dynamically allocated data */
};

struct ntg_object_fx ntg_object_fx_brighten_bg(int brightness_adj);

#endif // _NTG_OBJECT_FX_H_
