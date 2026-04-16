#include "core/scene/ntg_focus_manager.h"
#include "ntg.h"
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

void ntg_focus_manager_init(ntg_focus_manager* fm, ntg_scene* scene)
{
    assert(fm);
    assert(scene);

    (*fm) = (ntg_focus_manager) {0};

    fm->__scope_stack = malloc(sizeof(ntg_focus_scope_list));
    assert(fm->__scope_stack);

    int _status;
    ntg_focus_scope_list_init(fm->__scope_stack, &_status);
    assert(_status == GENC_SUCCESS);

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

    ntg_focus_manager_push_scope(fm, &scope);
}

void ntg_focus_manager_deinit(ntg_focus_manager* fm)
{
    assert(fm);

    int _status;
    ntg_focus_scope_list_deinit(fm->__scope_stack, &_status);
    assert(_status == GENC_SUCCESS);

    free(fm->__scope_stack);

    fm->__scope_stack = NULL;
    fm->_scene = NULL;
    fm->_focused = NULL;
}

void ntg_focus_manager_deinit_(void* _fm)
{
    assert(_fm);

    ntg_focus_manager_deinit(_fm);
}

/* -------------------------------------------------------------------------- */
/* GENERAL */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object)
{
    if(object) // FOCUS
    {
        const struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
        assert(scope);

        ntg_object* scope_root = scope->root;

        if(scope_root) // SSCOPE HAS ROOT
        {
            if(ntg_object_is_descendant_eq(scope_root, object)) // FOCUSABLE
            {
                fm->_focused = object;
                return true;
            }
            else // NOT FOCUSABLE
                return false;
        }
        else // NO ROOT SO FOCUSABLE
        {
            fm->_focused = object;
            return true;
        }
    }
    else // UNFOCUS
    {
        fm->_focused = NULL;
        return true;
    }
}

void ntg_focus_manager_push_scope(
        ntg_focus_manager* fm,
        const struct ntg_focus_scope* scope)
{
    assert(fm);
    assert(scope);

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;

    if(head)
    {
        if(head->data->scope.block_mode == NTG_FOCUS_SCOPE_BLOCK_TRUE)
            return;
    }

    if(scope->root) // make sure that scope root is desc of any layer root
    {
        size_t layer_count = ntg_scene_collect_layers_by_z(fm->_scene, NULL, 0);
        assert(layer_count > 0);

        ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
        assert(layers);

        ntg_scene_collect_layers_by_z(fm->_scene, layers, layer_count);

        size_t i;
        bool desc_of_any_layer = false;
        for(i = 0; i < layer_count; i++)
        {
            if(scope->root && ntg_object_is_descendant_eq(layers[i], scope->root))
                desc_of_any_layer = true;
        }

        free(layers);

        assert(desc_of_any_layer);
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
    assert(_status == GENC_SUCCESS);

    ntg_focus_manager_request_focus(fm, NULL);
}

void ntg_focus_manager_pop_scope(ntg_focus_manager* fm)
{
    assert(fm);

    // Keep the default scope inside the list
    if(fm->__scope_stack->size < 2)
        return;

    scope_stack_pop(fm);

    scope_stack_sync(fm);
}

const struct ntg_focus_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm)
{
    assert(fm);

    return (fm->__scope_stack->head ? &fm->__scope_stack->head->data->scope : NULL);
}

void ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed)
{
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
    assert(fm);

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
    assert(fm);

    const struct ntg_focus_scope* scope = ntg_focus_manager_get_active_scope(fm);
    if(!scope) return false;

    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);
    struct ntg_xy adj_pos = ntg_xy(0, 0);

    ntg_object* hit = ntg_scene_hit_test(fm->_scene, pos, &adj_pos);
    if(!hit)
        return false;

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
        if(scope->input_mode == NTG_FOCUS_SCOPE_INPUT_MODELESS)
            return ntg_object_feed_mouse(hit, mouse);
        else
            return false;

        if(scope->out_click_mode == NTG_FOCUS_SCOPE_OUT_CLICK_CLR)
        {
            ntg_focus_manager_request_focus(fm, NULL);
        }
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void scope_stack_pop(ntg_focus_manager* fm)
{
    int _status;
    ntg_focus_scope_list_popf(fm->__scope_stack, &_status);
    assert(_status == GENC_SUCCESS);

    struct ntg_focus_scope_list_node* head = fm->__scope_stack->head;
    assert(head);

    ntg_focus_manager_request_focus(fm, head->data->last_focused);

    head->data->last_focused = NULL;
}

static void scope_stack_sync(ntg_focus_manager* fm)
{
    struct ntg_focus_scope_list_node* head;

    while(true)
    {
        head = fm->__scope_stack->head;

        if(!head->data->valid)
            scope_stack_pop(fm);
        else
            break;
    }
}
