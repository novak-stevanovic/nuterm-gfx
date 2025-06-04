#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "ntg_base/fwd/ntg_cell_fwd.h"
#include "ntg_object/fwd/ntg_object_fwd.h"
#include "ntg_object/ntg_object_vec.h"
#include <stddef.h>

/* -------------------------------------------------------------------------- */

ntg_object_t* ntg_object_get_parent(const ntg_object_t* object);

const ntg_object_vec_t* ntg_object_get_children(const ntg_object_t* object);

void ntg_object_constrain(ntg_object_t* object);
void ntg_object_measure(ntg_object_t* object);
void ntg_object_arrange(ntg_object_t* object);

struct ntg_xy ntg_object_get_position_abs(const ntg_object_t* object);
struct ntg_xy ntg_object_get_position_rel(const ntg_object_t* object);

const ntg_object_content_t* ntg_object_get_content(const ntg_object_t* object);

/* -------------------------------------------------------------------------- */

const ntg_cell_t* ntg_object_content_at(const ntg_object_content_t* content,
        struct ntg_xy pos);

struct ntg_xy ntg_object_content_get_size(const ntg_object_content_t* content);

#endif // _NTG_OBJECT_H_
