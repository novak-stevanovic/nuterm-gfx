#ifndef _NTG_OBJECT_FWD_H_
#define _NTG_OBJECT_FWD_H_

typedef struct ntg_object ntg_object_t;
typedef struct ntg_object_drawing ntg_object_drawing_t;

/* Set constraints of object's children based on its adjusted constraints. */
typedef void (*ntg_constrain_fn)(ntg_object_t* object);

/* Set object's size based on adjusted constraints and children sizes. */
typedef void (*ntg_measure_fn)(ntg_object_t* object);

/* Arrange object's content based on conent size and object's internal state. */
typedef void (*ntg_arrange_fn)(ntg_object_t* object);

typedef struct ntg_pane ntg_pane_t;

typedef struct ntg_color_block ntg_color_block_t;

#endif // _NTG_OBJECT_FWD_H_

