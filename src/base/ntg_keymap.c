#include "base/ntg_keymap.h"
#include "shared/_uthash.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_status.h"

/* ------------------------------------------------------------------------- */

struct _nt_keymap_entry
{
    struct nt_key_event key_event; // key
    nt_key_handler_fn handler;
    UT_hash_handle hh;
};

struct nt_keymap
{
    struct _nt_keymap_entry* _map;
};

nt_keymap_t* nt_keymap_new(nt_status_t* out_status)
{
    nt_keymap_t* new = (nt_keymap_t*)malloc(sizeof(nt_keymap_t));
    if(new == NULL)
    {
        _return(NULL, out_status, NTG_ERR_ALLOC_FAIL);
    }

    new->_map = NULL;

    _return(new, out_status, NT_SUCCESS);
}

void nt_keymap_destroy(nt_keymap_t* map)
{
    if(map == NULL) return;

    struct _nt_keymap_entry *curr, *tmp;

    HASH_ITER(hh, map->_map, curr, tmp) {
        HASH_DEL(map->_map, curr);
        free(curr);
    }

    free(map);
}

void nt_keymap_bind(nt_keymap_t* map, struct nt_key_event key_event,
        nt_key_handler_fn event_handler, nt_status_t* out_status)
{
    if((map == NULL) || (event_handler == NULL))
        _vreturn(out_status, NT_ERR_INVALID_ARG);

    struct _nt_keymap_entry* _found;
    HASH_FIND(hh, map->_map, &key_event, sizeof(map->_map->key_event), _found);

    if(_found != NULL)
        _vreturn(out_status, NT_ERR_BIND_ALREADY_EXISTS);

    struct _nt_keymap_entry* new = (struct _nt_keymap_entry*)malloc
        (sizeof(struct _nt_keymap_entry));
    if(new == NULL)
        _vreturn(out_status, NT_ERR_ALLOC_FAIL);

    memset(new, 0, sizeof(struct _nt_keymap_entry));

    new->key_event = key_event;
    new->handler = event_handler;

    HASH_ADD(hh, map->_map, key_event, sizeof(map->_map->key_event), new);

    _vreturn(out_status, NT_SUCCESS);
}

void nt_keymap_unbind(nt_keymap_t* map, struct nt_key_event key_event,
        nt_status_t* out_status)
{
    if(map == NULL)
        _vreturn(out_status, NT_ERR_INVALID_ARG);

    struct _nt_keymap_entry* _found;
    HASH_FIND(hh, map->_map, &key_event, sizeof(map->_map->key_event), _found);
    if(_found == NULL)
        _vreturn(out_status, NT_SUCCESS);

    HASH_DEL(map->_map, _found);
    free(_found);

    _vreturn(out_status, NT_SUCCESS);
}

nt_key_handler_fn nt_keymap_get(nt_keymap_t* map, struct nt_key_event key_event,
        nt_status_t* out_status)
{
    if(map == NULL)
    {
        _return(NULL, out_status, NT_ERR_INVALID_ARG);
    }

    struct _nt_keymap_entry* _found;
    HASH_FIND(hh, map->_map, &key_event, sizeof(map->_map->key_event), _found);
    if(_found == NULL)
    {
        _return(NULL, out_status, NT_SUCCESS);
    }
    else
    {
        _return(_found->handler, out_status, NT_SUCCESS);
    }

    _return(NULL, out_status, NT_SUCCESS);
}
