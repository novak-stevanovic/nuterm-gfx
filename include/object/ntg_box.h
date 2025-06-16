#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_object.h"

typedef enum ntg_box_orientation 
{ 
    NTG_BOX_ORIENTATION_HORIZONTAL,
    NTG_BOX_ORIENTATION_VERTICAL 
} ntg_box_orientation_t;

struct ntg_box_padding
{
    size_t north, east, south, west;
};

typedef struct ntg_box ntg_box_t;

ntg_box_t* ntg_box_new(ntg_box_orientation_t orientation);
void ntg_box_destroy(ntg_box_t* box);

void ntg_box_add_child(ntg_box_t* box, ntg_object_t* object);
void ntg_box_remove_child(ntg_box_t* box, ntg_object_t* object);

struct ntg_box_padding ntg_box_get_padding(const ntg_box_t* box);
void ntg_box_set_padding(ntg_box_t* box, struct ntg_box_padding padding);

// size_t ntg_box_get_spacing(const ntg_box_t* box);
// void ntg_box_set_spacing(ntg_box_t* box, size_t spacing);

#endif // _NTG_BOX_H
