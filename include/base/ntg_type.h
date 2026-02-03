#ifndef NTG_TYPE_H
#define NTG_TYPE_H

#include "shared/ntg_shared.h"

struct ntg_type
{
    const ntg_type* __parent;
    size_t _size;
};

bool ntg_type_instance_of(const ntg_type* type, const ntg_type* ancestor);
bool ntg_type_are_equal(const ntg_type* type1, const ntg_type* type2);

extern const ntg_type NTG_TYPE_OBJECT, NTG_TYPE_PROG_BAR, NTG_TYPE_COLOR_BLOCK,
        NTG_TYPE_LABEL, NTG_TYPE_BOX, NTG_TYPE_MAIN_PANEL;

extern const ntg_type NTG_TYPE_SCENE;

extern const ntg_type NTG_TYPE_STAGE;

extern const ntg_type NTG_TYPE_RENDERER, NTG_TYPE_DEF_RENDERER;

extern const ntg_type NTG_TYPE_LOOP;

#endif // NTG_TYPE_H
