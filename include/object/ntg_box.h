#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_object.h"

typedef enum ntg_box_orientation
{ 
    NTG_BOX_ORIENTATION_HORIZONTAL,
    NTG_BOX_ORIENTATION_VERTICAL 
} ntg_box_orientation;

typedef enum ntg_box_alignment
{
    /* left, when used in reference to x axis alignment */
    /* top, when used in reference to y axis alignment */
    NTG_BOX_ALIGNMENT_0,

    /* center */
    NTG_BOX_ALIGNMENT_1,

    /* right, when used in reference to x axis alignment */
    /* bottom, when used in reference to y axis alignment */
    NTG_BOX_ALIGNMENT_2

} ntg_box_alignment;

struct ntg_box_padding
{
    size_t north, east, south, west;
};

static inline struct ntg_box_padding ntg_box_padding(size_t north, size_t east,
        size_t south, size_t west)
{
    return (struct ntg_box_padding) {
        .north = north, .east = east,
        .south = south, .west = west
    };
}

static const struct ntg_box_padding NTG_BOX_PADDING_UNSET = {0};

typedef struct ntg_box ntg_box;

ntg_box* ntg_box_new(ntg_box_orientation orientation,
        ntg_box_alignment primary_alignment,
        ntg_box_alignment secondary_alignment);

void ntg_box_destroy(ntg_box* box);

void ntg_box_add_child(ntg_box* box, ntg_object* object);
void ntg_box_remove_child(ntg_box* box, ntg_object* object);

struct ntg_box_padding ntg_box_get_padding(const ntg_box* box);
void ntg_box_set_padding(ntg_box* box, struct ntg_box_padding padding);

ntg_box_orientation ntg_box_get_orientation(ntg_box* box);
void ntg_box_set_orientation(ntg_box* box, ntg_box_orientation orientation);

ntg_box_alignment ntg_box_get_primary_alignment(ntg_box* box);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_box_alignment alignment);

ntg_box_alignment ntg_box_get_secondary_alignment(ntg_box* box);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_box_alignment alignment);

// size_t ntg_box_get_spacing(const ntg_box_t* box);
// void ntg_box_set_spacing(ntg_box_t* box, size_t spacing);

#endif // _NTG_BOX_H
