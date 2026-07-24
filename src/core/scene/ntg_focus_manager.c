#include "core/scene/ntg_focus_manager.h"
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_focus_scope_data
{
    struct ntg_focus_scope scope;
    ntg_object* last_focused;
    bool valid;
};

GENC_SIMPLE_LIST_GENERATE(ntg_focus_scope_list, struct ntg_focus_scope_data);

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void scope_stack_pop(ntg_focus_manager* fm);
static void scope_stack_sync(ntg_focus_manager* fm);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void _ntg_focus_manager_init(
        ntg_focus_manager* fm,
        ntg_scene* scene,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!fm || !scene)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    int _status;

    (*fm) = (ntg_focus_manager) {0};

    fm->__scope_stack = malloc(sizeof(ntg_focus_scope_list));
    if(!fm->__scope_stack)
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

    ntg_focus_scope_list_init(fm->__scope_stack, NULL);

    fm->_scene = scene;
    fm->_focused = NULL;

    ntg_focus_scope scope;
    ntg_focus_scope_init(
            &scope,
            NULL,
            init_scope_keybinds,
            NULL,
            &_status);

    switch(_status)
    {
        case 0: break;

        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    ntg_focus_manager_push_scope(fm, &scope, &_status);
    if(_status != 0)
    {
        _ntg_focus_manager_deinit(fm);

        switch(_status)
        {
            case NTG_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }
}

void _ntg_focus_manager_deinit(ntg_focus_manager* fm)
{
    if(!fm) return;

    if(fm->__scope_stack)
    {
        ntg_focus_scope_list_deinit(fm->__scope_stack, NULL);
        free(fm->__scope_stack);
    }

    fm->__scope_stack = NULL;
    fm->_scene = NULL;
    fm->_focused = NULL;
}

void ntg_focus_manager_deinit_(void* _fm)
{
    if(!_fm) return;

    _ntg_focus_manager_deinit(_fm);
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */

bool ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object)
{
    if(!fm) return false;

    bool focusable = false;
    ntg_object* old_focused = fm->_focused;

    if(object) 
    {
        const struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
        
        if(!scope) return false;

        ntg_object* scope_root = scope->_root;

        if(scope_root) 
        {
            if(ntg_object_is_descendant_eq(scope_root, object)) 
                focusable = true;
            else 
                focusable = false;
        }
        else 
            focusable = true;
    }
    else 
        focusable = true;
        
    if(focusable)
    {
        fm->_focused = object;

        if(fm->hooks.on_focused_chng_fn)
            fm->hooks.on_focused_chng_fn(fm, old_focused, object);
            
        if(old_focused)
            _ntg_object_unfocus(old_focused, object);

        if(object)
            _ntg_object_unfocus(object, old_focused);

        return true;
    }

    return false;
}

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

void ntg_focus_manager_push_scope(
        ntg_focus_manager* fm,
        const ntg_focus_scope* scope,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!fm || !scope)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    int _status;

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;

    if(head)
    {
        if(head->data->scope._opts.block_mode == NTG_FOCUS_SCOPE_BLOCK_TRUE)
            return;
    }

    if(scope->_root) 
    {
        size_t layer_count = ntg_scene_collect_layers_by_z(fm->_scene, NULL, 0);
        if(layer_count == 0)
            ntg_vreturn(out_status, NTG_ERR_SCENE_EMPTY);

        ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
        if(!layers)
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        ntg_scene_collect_layers_by_z(fm->_scene, layers, layer_count);

        size_t i;
        bool desc_of_any_layer = false;
        for(i = 0; i < layer_count; i++)
        {
            if(scope->_root && ntg_object_is_descendant_eq(layers[i], scope->_root))
                desc_of_any_layer = true;
        }

        free(layers);

        if(!desc_of_any_layer)
            ntg_vreturn(out_status, NTG_ERR_SCOPE_NOT_IN_SCENE);
    }

    if(head)
    {
        head->data->last_focused = fm->_focused;
    }

    struct ntg_focus_scope_data data = {0};
    data.last_focused = NULL;
    data.valid = true;
    ntg_focus_scope_init_move(&data.scope, scope, &_status);
    switch(_status)
    {
        case 0: break;

        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    ntg_focus_scope_list_pushf(fm->__scope_stack, data, &_status);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

    _ntg_focus_scope_attach(&data.scope);

    ntg_focus_manager_request_focus(fm, NULL);

    if(fm->hooks.on_scope_push_fn)
        fm->hooks.on_scope_push_fn(fm, scope);
}

void ntg_focus_manager_pop_scope(ntg_focus_manager* fm)
{
    if(!fm) return;

    
    if(fm->__scope_stack->size < 2)
        return;

    scope_stack_pop(fm);

    scope_stack_sync(fm);
}

struct ntg_focus_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm)
{
    if(!fm) return NULL;

    return (fm->__scope_stack->head ? &fm->__scope_stack->head->data->scope : NULL);
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INVALIDATE */
/* ------------------------------------------------------ */

void _ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed)
{
    if(!fm) return;

    struct ntg_focus_scope_list_node* it_node = fm->__scope_stack->head;
    struct ntg_focus_scope_data* it_data;

    while(it_node)
    {
        it_data = it_node->data;

        if(it_data->scope._root && ntg_object_is_descendant_eq(it_data->scope._root, removed))
            it_data->valid = false;

        it_node = it_node->next;
    }

    scope_stack_sync(fm);
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_focus_manager_feed_key(ntg_focus_manager* fm, struct nt_key_event key)
{
    if(!fm) return false;

    ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
    if(!scope) return false;

    return ntg_focus_scope_feed_key(scope, key);
}

bool ntg_focus_manager_feed_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse)
{
    if(!fm) return false;

    struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
    if(!scope) return false;

    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);
    struct ntg_xy adj_pos = ntg_xy(0, 0);

    

    int _status;

    ntg_object* hit = ntg_scene_hit_test(fm->_scene, pos, &adj_pos, &_status);
    if(_status != 0)
        return false;

    if(!hit)
    {
        if(scope->_opts.out_click_mode == NTG_FOCUS_SCOPE_OUT_CLICK_CLR)
            ntg_focus_manager_request_focus(fm, NULL);

        return false;
    }

    
    if((!scope->_root) || ntg_object_is_descendant_eq(scope->_root, hit))
    {
        return ntg_focus_scope_feed_mouse(scope, mouse, hit);
    }
    else 
    {
        if(scope->_opts.out_click_mode == NTG_FOCUS_SCOPE_OUT_CLICK_CLR)
            ntg_focus_manager_request_focus(fm, NULL);

        if(scope->_opts.input_mode == NTG_FOCUS_SCOPE_INPUT_MODELESS)
            return ntg_object_feed_mouse(hit, mouse, NTG_OBJECT_MOUSE_TRUE);
        else
            return false;
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void scope_stack_pop(ntg_focus_manager* fm)
{
    struct ntg_focus_scope_list_node* old_head = fm->__scope_stack->head;
    if(!old_head)
        return;

    struct ntg_focus_scope popped_scope = old_head->data->scope;

    ntg_focus_scope_list_popf(fm->__scope_stack, NULL);

    
    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;
    if(!head)
        return;

    ntg_focus_manager_request_focus(fm, head->data->last_focused);

    head->data->last_focused = NULL;

    if(fm->hooks.on_scope_pop_fn)
        fm->hooks.on_scope_pop_fn(fm, &popped_scope);
}

static void scope_stack_sync(ntg_focus_manager* fm)
{
    if(!fm || !fm->__scope_stack)
        return;

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;
    while(head && !head->data->valid && (fm->__scope_stack->size > 1))
    {
        scope_stack_pop(fm);
        head = fm->__scope_stack->head;
    }

    
    if(head && !head->data->valid)
        head->data->valid = true;
}
