#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_object.h"

typedef enum ntg_box_orientation 
{ 
    NTG_BOX_ORIENTATION_HORIZONTAL,
    NTG_BOX_ORIENTATION_VERTICAL 
} ntg_box_orientation_t;

typedef struct ntg_box ntg_box_t;

ntg_box_t* ntg_box_new(ntg_box_orientation_t orientation);
void ntg_box_destroy(ntg_box_t* box);

void ntg_box_add_child(ntg_box_t* box, ntg_object_t* object);
void ntg_box_remove_child(ntg_box_t* box, ntg_object_t* object);

#endif // _NTG_BOX_H
