#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_object_fwd.h"

ntg_box_t* ntg_box_new();
void ntg_box_destroy(ntg_box_t* box);

void ntg_box_add_child(ntg_box_t* box, ntg_object_t* object);

#endif // _NTG_BOX_H
