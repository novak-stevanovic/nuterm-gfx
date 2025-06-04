#ifndef _NTG_OBJECT_FWD_H_
#define _NTG_OBJECT_FWD_H_

typedef struct ntg_object ntg_object_t;
typedef struct ntg_object_content ntg_object_content_t;

/* Set constraints of object's children based on its constraints. */
typedef void (*ntg_constrain_fn)(ntg_object_t* object);

/* Set object's size based on constraints and children sizes. */
typedef void (*ntg_measure_fn)(ntg_object_t* object);

/* Arrange object's content. Its size has already been set. */
typedef void (*ntg_arrange_fn)(ntg_object_t* object);

#endif // _NTG_OBJECT_FWD_H_
