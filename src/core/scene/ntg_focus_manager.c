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
    ntg_focus_scope* scope;
};

GENC_FWD_LIST_INLINE(ntg_focus_scope_list, struct ntg_focus_scope_data)

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

int _ntg_focus_manager_init(
        ntg_focus_manager* fm,
        ntg_scene* scene,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds)
{
    if(!fm || !scene)
        return NTG_ERR_INV_ARG;

    int _status;

    (*fm) = (ntg_focus_manager) {0};

    fm->__scope_stack = malloc(sizeof(ntg_focus_scope_list));
    if(!fm->__scope_stack)
        return NTG_ERR_ALLOC_FAIL;

    *fm->__scope_stack = (ntg_focus_scope_list) {0};

    fm->_scene = scene;
    fm->_focused = NULL;

    ntg_focus_scope scope;
    _status = ntg_focus_scope_init(
            &scope,
            NULL,
            init_scope_keybinds,
            NULL);

    if(_status != 0)
        return _status;

    _status = ntg_focus_manager_stack_push(fm, &scope, NULL);
    if(_status != 0)
    {
        _ntg_focus_manager_deinit(fm);
        return _status;
    }

    return 0;
}

void _ntg_focus_manager_deinit(ntg_focus_manager* fm)
{
    if(!fm) return;

    if(fm->__scope_stack)
    {
        while(fm->__scope_stack->size > 1)
            ntg_focus_manager_stack_pop(fm);

        ntg_focus_scope* head = ntg_focus_manager_stack_get_active(fm);
        if(head)
        {
            ntg_focus_scope_deinit(head);
            free(head);
        }

        (void)ntg_focus_scope_list_deinit(fm->__scope_stack);
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
        const struct ntg_focus_scope* scope = ntg_focus_manager_stack_get_active(fm);
        
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
            _ntg_object_focus(object, old_focused);

        return true;
    }

    return false;
}

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

int ntg_focus_manager_stack_push(
        ntg_focus_manager* fm,
        const ntg_focus_scope* scope,
        ntg_focus_scope** out_scope)
{
    if(!fm || !scope)
        return NTG_ERR_INV_ARG;

    ntg_set_out(out_scope, NULL);

    int _status;

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;

    if(head)
    {
        if(head->data.scope->_opts.block_mode == NTG_FOCUS_SCOPE_BLOCK_TRUE)
            return 0;
    }

    if(scope->_root) 
    {
        size_t layer_count = ntg_scene_collect_layers_by_z(fm->_scene, NULL, 0);
        if(layer_count == 0)
            return NTG_ERR_SCENE_EMPTY;

        ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
        if(!layers)
            return NTG_ERR_ALLOC_FAIL;

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
            return NTG_ERR_SCOPE_NOT_IN_SCENE;
    }

    if(head)
    {
        _ntg_focus_scope_set_last_focused(head->data.scope, fm->_focused);
    }

    ntg_focus_scope* new_scope = malloc(sizeof(ntg_focus_scope));
    if(!new_scope)
        return NTG_ERR_ALLOC_FAIL;

    struct ntg_focus_scope_data data = {0};
    _status = ntg_focus_scope_init_move(new_scope, scope);
    if(_status != 0)
    {
        free(new_scope);
        return _status;
    }
    data.scope = new_scope;

    _status = ntg_focus_scope_list_pushf(fm->__scope_stack, data);
    if(_status != 0)
    {
        free(new_scope);
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;

            default:
                return NTG_ERR_UNEXPECTED;
        }
    }
    _ntg_focus_scope_attach(new_scope, fm);

    ntg_focus_manager_request_focus(fm, NULL);

    if(fm->hooks.on_scope_push_fn)
        fm->hooks.on_scope_push_fn(fm, new_scope);

    ntg_set_out(out_scope, new_scope);
    return 0;
}

void ntg_focus_manager_stack_pop(ntg_focus_manager* fm)
{
    if(!fm) return;
    
    if(fm->__scope_stack->size < 2)
        return;

    scope_stack_pop(fm);

    scope_stack_sync(fm);
}

ntg_focus_scope* ntg_focus_manager_stack_get_active(ntg_focus_manager* fm)
{
    if(!fm) return NULL;

    return (fm->__scope_stack->head ?
            fm->__scope_stack->head->data.scope : NULL);
}

size_t ntg_focus_manager_stack_get_size(const ntg_focus_manager* fm)
{
    if(!fm) return 0;

    return (fm->__scope_stack ? fm->__scope_stack->size : 0);
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

void _ntg_focus_manager_on_scene_object_rm(ntg_focus_manager* fm, ntg_object* removed)
{
    if(!fm) return;

    if(removed && (fm->_focused == removed))
        ntg_focus_manager_request_focus(fm, NULL);

    struct ntg_focus_scope_list_node* it_node = fm->__scope_stack->head;
    struct ntg_focus_scope_data* it_data;

    while(it_node)
    {
        it_data = &it_node->data;

        if(it_data->scope->_root && ntg_object_is_descendant_eq(it_data->scope->_root, removed))
            _ntg_focus_scope_invalidate(it_data->scope);

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

    ntg_focus_scope* scope = ntg_focus_manager_stack_get_active(fm);
    if(!scope) return false;

    return ntg_focus_scope_feed_key(scope, key);
}

bool ntg_focus_manager_feed_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse)
{
    if(!fm) return false;

    struct ntg_focus_scope* scope = ntg_focus_manager_stack_get_active(fm);
    if(!scope) return false;

    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);
    struct ntg_xy adj_pos = ntg_xy(0, 0);

    int _status;
    ntg_object* hit = NULL;
    _status = ntg_scene_hit_test(fm->_scene, pos, &adj_pos, NULL, &hit);
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
        mouse.x = adj_pos.x;
        mouse.y = adj_pos.y;

        return ntg_focus_scope_feed_mouse(scope, mouse, hit);
    }
    else 
    {
        if(scope->_opts.out_click_mode == NTG_FOCUS_SCOPE_OUT_CLICK_CLR)
            ntg_focus_manager_request_focus(fm, NULL);

        if(scope->_opts.input_mode == NTG_FOCUS_SCOPE_INPUT_MODELESS)
        {
            mouse.x = adj_pos.x;
            mouse.y = adj_pos.y;

            struct ntg_object_mouse event = {
                .mouse = mouse,
                .target = hit,
                .from_keybind = false
            };
            
            if(hit->_clickable)
                return ntg_object_feed_mouse(hit, &event);
            else
                return false;
        }
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

    ntg_focus_scope* popped_scope = old_head->data.scope;

    (void)ntg_focus_scope_list_popf(fm->__scope_stack);
    
    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;
    if(!head)
        return;

    ntg_focus_manager_request_focus(fm, head->data.scope->_last_focused);
    _ntg_focus_scope_set_last_focused(head->data.scope, NULL);

    if(fm->hooks.on_scope_pop_fn)
        fm->hooks.on_scope_pop_fn(fm, popped_scope);

    ntg_focus_scope_deinit(popped_scope);
    free(popped_scope);
}

static void scope_stack_sync(ntg_focus_manager* fm)
{
    if(!fm || !fm->__scope_stack)
        return;

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;
    while(head && !head->data.scope->__valid && (fm->__scope_stack->size > 1))
    {
        scope_stack_pop(fm);
        head = fm->__scope_stack->head;
    }

    if(head && !head->data.scope->__valid)
        head->data.scope->__valid = true;
}
