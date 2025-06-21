#ifndef _NTG_KEYMAP_H_
#define _NTG_KEYMAP_H_

#include "nt_event.h"

typedef struct nt_keymap nt_keymap_t;

/* Creates the nt_keymap, initializes internal hashmap.
 *
 * STATUS CODES:
 * 1. NT_SUCCESS,
 * 2. NT_ERR_ALLOC_FAIL - dynamic allocation for the internal struct nt_keymap
 * failed. */
nt_keymap_t* nt_keymap_new(nt_status_t* out_status);

/* Destroys the nt_keymap. Frees the dynamially allocated memory. */
void nt_keymap_destroy(nt_keymap_t* map);

typedef void (*nt_key_handler_fn)(struct nt_key_event key_event, void* data);

/* Binds a specific `event_handler` to given `key_event`. This will create an
 * entry inside the internal hashmap. Function assumes `key_event` is valid.
 *
 * STATUS CODES:
 * 1. NT_SUCCESS, 
 * 2. NT_ERR_INVALID_ARG - `map` or `event_handler` is NULL,
 * 3. NT_ERR_BIND_ALREADY_EXISTS - `key_event` is already a key inside the
 * hashmap,
 * 4. NT_ERR_ALLOC_FAIL - dynamic allocation failed. */
void nt_keymap_bind(nt_keymap_t* map, struct nt_key_event key_event,
        nt_key_handler_fn event_handler, nt_status_t* out_status);

/* Unbinds a specific `event_handler` from the given `key_event`. This will
 * remove the entry inside the internal hashmap. If `key_event` is not a key
 * inside the hashmap, NT_SUCCESS is returned.
 *
 * STATUS CODES:
 * 1. NT_SUCCESS, 
 * 2. NT_ERR_INVALID_ARG - `map` is NULL. */
void nt_keymap_unbind(nt_keymap_t* map, struct nt_key_event key_event,
        nt_status_t* out_status);

/* Retrieves nt_key_handler_t tied to given `key_event`. If `key_event` is not
 * an entry inside the `map`, NULL is returned.
 *
 * 1. NT_SUCCESS, 
 * 2. NT_ERR_INVALID_ARG - `map` is NULL. */
nt_key_handler_fn nt_keymap_get(nt_keymap_t* map, struct nt_key_event key_event,
        nt_status_t* out_status);

#endif // _NTG_KEYMAP_H_
