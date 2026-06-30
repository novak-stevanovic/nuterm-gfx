#include "core/scene/ntg_focus_manager.h"
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

struct ntg_focus_scope_data
{
    struct ntg_focus_scope scope;
    ntg_object* last_focused;
    bool valid;
};

GENC_SIMPLE_LIST_GENERATE(ntg_focus_scope_list, struct ntg_focus_scope_data);

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void scope_stack_pop(ntg_focus_manager* fm);
static void scope_stack_sync(ntg_focus_manager* fm);

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void ntg_focus_manager_init(ntg_focus_manager* fm, ntg_scene* scene, int* out_status)
{
    ntg_init_status(out_status);

    if(!fm || !scene)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    (*fm) = (ntg_focus_manager) {0};

    fm->__scope_stack = malloc(sizeof(ntg_focus_scope_list));
    if(!fm->__scope_stack)
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

    ntg_focus_scope_list_init(fm->__scope_stack, NULL);

    fm->_scene = scene;
    fm->_focused = NULL;

    struct ntg_focus_scope scope = {
        .root = NULL,
        .on_key_fn = NULL,
        .input_mode = NTG_FOCUS_SCOPE_INPUT_MODELESS,
        .out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
        .block_mode = NTG_FOCUS_SCOPE_BLOCK_FALSE,
        .data = NULL
    };

    int _status;
    ntg_focus_manager_push_scope(fm, &scope, &_status);
    if(_status != NTG_SUCCESS)
    {
        ntg_focus_manager_deinit(fm);

        switch(_status)
        {
            case NTG_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }
}

void ntg_focus_manager_deinit(ntg_focus_manager* fm)
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

    ntg_focus_manager_deinit(_fm);
}

/* -------------------------------------------------------------------------- */
/* GENERAL */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object)
{
    if(!fm) return false;

    bool focusable = false;
    ntg_object* old_focused = fm->_focused;

    if(object) // FOCUS
    {
        const struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
        // assert(scope);
        if(!scope) return false;

        ntg_object* scope_root = scope->root;

        if(scope_root) // SSCOPE HAS ROOT
        {
            if(ntg_object_is_descendant_eq(scope_root, object)) // FOCUSABLE
                focusable = true;
            else // NOT FOCUSABLE
                focusable = false;
        }
        else // NO ROOT SO FOCUSABLE
            focusable = true;
    }
    else // UNFOCUS
        focusable = true;
        
    if(focusable)
    {
        fm->_focused = object;

        if(fm->hooks.on_focused_chng_fn)
            fm->hooks.on_focused_chng_fn(fm, old_focused, object);
            
        if(old_focused && old_focused->hooks.on_unfocus_fn)
            old_focused->hooks.on_unfocus_fn(old_focused, object);

        if(object && object->hooks.on_focus_fn)
            object->hooks.on_focus_fn(object, old_focused);

        return true;
    }

    return false;
}

void ntg_focus_manager_push_scope(
        ntg_focus_manager* fm,
        const struct ntg_focus_scope* scope,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!fm || !scope)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;

    if(head)
    {
        if(head->data->scope.block_mode == NTG_FOCUS_SCOPE_BLOCK_TRUE)
            return;
    }

    if(scope->root) // make sure that scope root is desc of any layer root
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
            if(scope->root && ntg_object_is_descendant_eq(layers[i], scope->root))
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

    struct ntg_focus_scope_data data = {
        .scope = (*scope),
        .last_focused = NULL,
        .valid = true
    };

    int _status;
    ntg_focus_scope_list_pushf(fm->__scope_stack, data, &_status);
    if(_status != GENC_SUCCESS)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

    ntg_focus_manager_request_focus(fm, NULL);

    if(fm->hooks.on_scope_push_fn)
        fm->hooks.on_scope_push_fn(fm, scope);
}

void ntg_focus_manager_pop_scope(ntg_focus_manager* fm)
{
    if(!fm) return;

    // Keep the default scope inside the list
    if(fm->__scope_stack->size < 2)
        return;

    scope_stack_pop(fm);

    scope_stack_sync(fm);
}

const struct ntg_focus_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm)
{
    if(!fm) return NULL;

    return (fm->__scope_stack->head ? &fm->__scope_stack->head->data->scope : NULL);
}

void ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed)
{
    if(!fm) return;

    struct ntg_focus_scope_list_node* it_node = fm->__scope_stack->head;
    struct ntg_focus_scope_data* it_data;

    while(it_node)
    {
        it_data = it_node->data;

        if(it_data->scope.root && ntg_object_is_descendant_eq(it_data->scope.root, removed))
            it_data->valid = false;

        it_node = it_node->next;
    }

    scope_stack_sync(fm);
}

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_feed_key(ntg_focus_manager* fm, struct nt_key_event key)
{
    if(!fm) return false;

    const struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
    if(!scope) return false;

    struct ntg_focus_key_ctx ctx = {
        .fm = fm,
        .scope_root = scope->root
    };

    if(scope->on_key_fn)
        return scope->on_key_fn(scope->data, &ctx, key);
    else
        return false;
}

bool ntg_focus_manager_feed_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse)
{
    if(!fm) return false;

    const struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
    if(!scope) return false;

    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);
    struct ntg_xy adj_pos = ntg_xy(0, 0);

    int _status;

    ntg_object* hit = ntg_scene_hit_test(fm->_scene, pos, &adj_pos, &_status);
    if(_status != NTG_SUCCESS)
        return false;

    if(!hit)
    {
        if(scope->out_click_mode == NTG_FOCUS_SCOPE_OUT_CLICK_CLR)
            ntg_focus_manager_request_focus(fm, NULL);

        return false;
    }

    ntg_log_log("ADJ: %d %d", adj_pos.x, adj_pos.y);

    if((!scope->root) || ntg_object_is_descendant_eq(scope->root, hit)) // INSIDE SCOPE
    {
        struct ntg_focus_mouse_ctx ctx = {
            .fm = fm,
            .scope_root = scope->root,
            .clicked = hit,
            .adj_pos = adj_pos
        };

        if(scope->on_mouse_fn)
            return scope->on_mouse_fn(scope->data, &ctx, mouse);
        else
            return false;
    }
    else // OUTSIDE SCOPE
    {
        if(scope->out_click_mode == NTG_FOCUS_SCOPE_OUT_CLICK_CLR)
            ntg_focus_manager_request_focus(fm, NULL);

        if(scope->input_mode == NTG_FOCUS_SCOPE_INPUT_MODELESS)
            return ntg_object_feed_mouse(hit, mouse);
        else
            return false;
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void scope_stack_pop(ntg_focus_manager* fm)
{
    struct ntg_focus_scope_list_node* old_head = fm->__scope_stack->head;
    if(!old_head)
        return;

    struct ntg_focus_scope popped_scope = old_head->data->scope;

    ntg_focus_scope_list_popf(fm->__scope_stack, NULL);

    // Head is assumed to exist, one node is pushed on init...
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

    /* The default scope must always remain usable. */
    if(head && !head->data->valid)
        head->data->valid = true;
}
