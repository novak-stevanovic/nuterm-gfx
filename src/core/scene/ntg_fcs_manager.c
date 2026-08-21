#include "core/scene/ntg_fcs_manager.h"
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_fcs_scope_data
{
    ntg_fcs_scope* scope;
};

GENC_FWD_LIST_INLINE(ntg_fcs_scope_list, struct ntg_fcs_scope_data)

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void scope_stack_pop(ntg_fcs_manager* fm);
static void scope_stack_sync(ntg_fcs_manager* fm);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */

bool ntg_fcs_manager_request_focus(ntg_fcs_manager* fm, ntg_object* object)
{
    if(!fm) return false;

    bool focusable = false;
    ntg_object* old_focused = fm->ro.focused;

    if(object) 
    {
        const struct ntg_fcs_scope* scope = ntg_fcs_manager_stack_get_active(fm);
        
        if(!scope) return false;

        ntg_object* scope_root = scope->ro.root;

        if(scope_root) 
        {
            if(ntg_object_is_in_tree(scope_root, object)) 
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
        fm->ro.focused = object;

        struct ntg_event_fcs_manager_fcschg_dt event_dt = {
            .old_focused = old_focused,
            .new_focused = object
        };
        ntg_event_raise(
                &fm->ro.event_dlgt,
                ntg_event_new(
                    NTG_EVENT_FCS_MANAGER_FCSCHG, fm, &event_dt));
            
        if(old_focused)
            ntg__object_unfocus(old_focused);

        if(object)
            ntg__object_focus(object);

        return true;
    }

    return false;
}

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

int ntg_fcs_manager_stack_push(ntg_fcs_manager* fm, ntg_fcs_scope* scope)
{
    if(!fm || !scope)
        return NTG_ERR_INV_ARG;

    int _status;

    struct ntg_fcs_scope_list_node* head = fm->priv.scope_stack->head;

    if(head)
    {
        if(head->data.scope->ro.opts.block_mode == NTG_FCS_SCOPE_BLOCK_TRUE)
            return 0;
    }

    if(scope->ro.root) 
    {
        size_t layer_count = ntg_scene_collect_layers_by_z(fm->ro.scene, NULL, 0);
        if(layer_count == 0)
            return NTG_ERR_SCENE_EMPTY;

        ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
        if(!layers)
            return NTG_ERR_ALLOC_FAIL;

        ntg_scene_collect_layers_by_z(fm->ro.scene, layers, layer_count);

        size_t i;
        bool desc_of_any_layer = false;
        for(i = 0; i < layer_count; i++)
        {
            if(scope->ro.root && ntg_object_is_in_tree(layers[i], scope->ro.root))
                desc_of_any_layer = true;
        }

        free(layers);

        if(!desc_of_any_layer)
            return NTG_ERR_SCOPE_NOT_IN_SCENE;
    }

    if(head)
    {
        ntg__fcs_scope_set_last_focused(head->data.scope, fm->ro.focused);
    }

    struct ntg_fcs_scope_data data = { .scope = scope };

    _status = ntg_fcs_scope_list_pushf(fm->priv.scope_stack, data);
    switch(_status)
    {
        case 0:
            break;
        case GENC_ERR_ALLOC_FAIL:
            return NTG_ERR_ALLOC_FAIL;

        default:
            return NTG_ERR_UNEXPECTED;
    }

    ntg__fcs_scope_attach(scope, fm);

    ntg_fcs_manager_request_focus(fm, NULL);

    struct ntg_event_fcs_manager_scpsh_dt event_dt = { .scope = scope };
    ntg_event_raise(
            &fm->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_FCS_MANAGER_SCPSH, fm, &event_dt));

    return 0;
}

/* ------------------------------------------------------ */

int ntg_fcs_manager_stack_pop(ntg_fcs_manager* fm)
{
    if(!fm) return NTG_ERR_INV_ARG;
    
    if(fm->priv.scope_stack->size < 2)
        return 0;

    scope_stack_pop(fm);

    scope_stack_sync(fm);

    return 0;
}

/* ------------------------------------------------------ */

ntg_fcs_scope* ntg_fcs_manager_stack_get_active(ntg_fcs_manager* fm)
{
    if(!fm) return NULL;

    return (fm->priv.scope_stack->head ?
            fm->priv.scope_stack->head->data.scope : NULL);
}

/* ------------------------------------------------------ */

size_t ntg_fcs_manager_stack_get_size(const ntg_fcs_manager* fm)
{
    if(!fm) return 0;

    return (fm->priv.scope_stack ? fm->priv.scope_stack->size : 0);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_fcs_manager_feed_key(ntg_fcs_manager* fm, struct nt_key key)
{
    if(!fm) return false;

    ntg_fcs_scope* scope = ntg_fcs_manager_stack_get_active(fm);
    if(!scope) return false;

    return ntg_fcs_scope_feed_key(scope, key);
}

/* ------------------------------------------------------ */

bool ntg_fcs_manager_feed_mouse(ntg_fcs_manager* fm, struct nt_mouse mouse)
{
    if(!fm) return false;

    struct ntg_fcs_scope* scope = ntg_fcs_manager_stack_get_active(fm);
    if(!scope) return false;

    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);

    int _status;
    struct ntg_scene_hit_res res;
    _status = ntg_scene_hit_test(fm->ro.scene, pos, &res);
    if(_status != 0) /* Click failed, tell the callers not to consume */
        return true;

    ntg_object* hit = res.res.object;

    if(!hit)
    {
        if(scope->ro.opts.out_click_mode == NTG_FCS_SCOPE_OUT_CLICK_CLR)
            ntg_fcs_manager_request_focus(fm, NULL);

        return false;
    }
    
    mouse.x = res.res.local_pos.x;
    mouse.y = res.res.local_pos.y;

    if((!scope->ro.root) || ntg_object_is_in_tree(scope->ro.root, hit))
    {
        return ntg_fcs_scope_feed_mouse(scope, mouse, hit);
    }
    else 
    {
        if(scope->ro.opts.out_click_mode == NTG_FCS_SCOPE_OUT_CLICK_CLR)
            ntg_fcs_manager_request_focus(fm, NULL);

        if(scope->ro.opts.input_mode == NTG_FCS_SCOPE_INPUT_MODELESS)
        {
            struct ntg_object_mouse event = {
                .mouse = mouse,
                .target = hit,
                .from_keybind = false
            };
            
            if(hit->ro.clickable)
                return ntg_object_feed_mouse(hit, &event);
            else
                return false;
        }
        else
            return false;
    }
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg__fcs_manager_init(
        ntg_fcs_manager* fm,
        ntg_scene* scene,
        const struct ntg_fcs_scope_keys* init_scope_keybinds)
{
    if(!fm || !scene)
        return NTG_ERR_INV_ARG;

    int _status;

    (*fm) = (ntg_fcs_manager) {0};

    fm->priv.scope_stack = malloc(sizeof(ntg_fcs_scope_list));
    if(!fm->priv.scope_stack)
        return NTG_ERR_ALLOC_FAIL;

    *fm->priv.scope_stack = (ntg_fcs_scope_list) {0};

    fm->ro.scene = scene;
    fm->ro.focused = NULL;

    ntg_fcs_scope* scope = malloc(sizeof(ntg_fcs_scope));
    if(!scope)
    {
        ntg__fcs_manager_deinit(fm);
        return NTG_ERR_ALLOC_FAIL;
    }

    _status = ntg_fcs_scope_init(scope, NULL, init_scope_keybinds, NULL);

    if(_status != 0)
        return _status;

    _status = ntg_fcs_manager_stack_push(fm, scope);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg__fcs_manager_deinit(fm);
            return NTG_ERR_ALLOC_FAIL;
        default:
            ntg__fcs_manager_deinit(fm);
            return NTG_ERR_UNEXPECTED;
    }

    ntg_event_delegate_init(&fm->ro.event_dlgt);

    return 0;
}

/* ------------------------------------------------------ */

void ntg__fcs_manager_deinit(ntg_fcs_manager* fm)
{
    if(!fm) return;

    if(fm->priv.scope_stack)
    {
        while(fm->priv.scope_stack->size > 1)
            ntg_fcs_manager_stack_pop(fm);

        ntg_fcs_scope* head = ntg_fcs_manager_stack_get_active(fm);
        if(head)
        {
            ntg_fcs_scope_deinit(head);
            free(head);
        }

        ntg_fcs_scope_list_deinit(fm->priv.scope_stack);

        free(fm->priv.scope_stack);
    }

    fm->priv.scope_stack = NULL;
    fm->ro.scene = NULL;
    fm->ro.focused = NULL;

    ntg_event_delegate_deinit(&fm->ro.event_dlgt);

}

void ntg_fcs_manager_deinit_void(void* _fm)
{
    if(!_fm) return;

    ntg__fcs_manager_deinit(_fm);
}

/* ------------------------------------------------------ */
/* INVALIDATE */
/* ------------------------------------------------------ */

void ntg__fcs_manager_on_scene_object_rm(ntg_fcs_manager* fm, ntg_object* removed)
{
    if(!fm) return;

    if(removed && (fm->ro.focused == removed))
        ntg_fcs_manager_request_focus(fm, NULL);

    struct ntg_fcs_scope_list_node* it_node = fm->priv.scope_stack->head;
    struct ntg_fcs_scope_data* it_data;

    while(it_node)
    {
        it_data = &it_node->data;

        if(it_data->scope->ro.root && ntg_object_is_in_tree(it_data->scope->ro.root, removed))
            ntg__fcs_scope_invalidate(it_data->scope);

        it_node = it_node->next;
    }

    scope_stack_sync(fm);

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void scope_stack_pop(ntg_fcs_manager* fm)
{
    struct ntg_fcs_scope_list_node* old_head = fm->priv.scope_stack->head;
    if(!old_head)
        return;

    ntg_fcs_scope* popped_scope = old_head->data.scope;

    ntg_fcs_scope_list_popf(fm->priv.scope_stack);
    
    struct ntg_fcs_scope_list_node* head = fm->priv.scope_stack->head;
    if(!head)
        return;

    ntg_fcs_manager_request_focus(fm, head->data.scope->ro.last_focused);
    ntg__fcs_scope_set_last_focused(head->data.scope, NULL);

    struct ntg_event_fcs_manager_scpop_dt event_dt = { .scope = popped_scope };
    ntg_event_raise(
            &fm->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_FCS_MANAGER_SCPOP, fm, &event_dt));

    ntg_fcs_scope_deinit(popped_scope);
    // TODO
    // free(popped_scope);
}

static void scope_stack_sync(ntg_fcs_manager* fm)
{
    if(!fm || !fm->priv.scope_stack)
        return;

    struct ntg_fcs_scope_list_node* head = fm->priv.scope_stack->head;
    while(head && !head->data.scope->priv.valid && (fm->priv.scope_stack->size > 1))
    {
        scope_stack_pop(fm);
        head = fm->priv.scope_stack->head;
    }

    if(head && !head->data.scope->priv.valid)
        head->data.scope->priv.valid = true;
}
