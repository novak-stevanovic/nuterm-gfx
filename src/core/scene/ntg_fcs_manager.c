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
    struct ntg_fcs_scope scope;
    ntg_widget* last_focused;
    bool valid;
};

GENC_FWD_LIST_INLINE(ntg_fcs_scope_list, struct ntg_fcs_scope_data)

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void scope_stack_pop(ntg_fcs_manager* fm);
static void scope_stack_sync(ntg_fcs_manager* fm);
static void fcs_manager_deinit_fn(ntg_object* _fm);

static const struct ntg_object_vtable VTABLE = {
    .deinit_fn = fcs_manager_deinit_fn
};

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

bool ntg_fcs_manager_request_focus(ntg_fcs_manager* fm, ntg_widget* widget)
{
    if(!fm) return false;

    bool focusable = false;
    ntg_widget* old_focused = fm->ro.focused;

    if(widget) 
    {
        const struct ntg_fcs_scope* scope = ntg_fcs_manager_stack_get_active(fm);
        
        if(!scope) return false;

        ntg_widget* scope_root = scope->root;

        if(scope_root) 
        {
            if(ntg_widget_is_in_tree(scope_root, widget)) 
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
        fm->ro.focused = widget;

        struct ntg_event_fcs_manager_fcschg_dt event_dt = {
            .old_focused = old_focused,
            .new_focused = widget
        };
        ntg_object_event_raise(ntg_obj(fm), NTG_EVENT_FCS_MANAGER_FCSCHG, &event_dt);
            
        if(old_focused)
            ntg__widget_unfocus(old_focused);

        if(widget)
            ntg__widget_focus(widget);

        return true;
    }

    return false;
}

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

int ntg_fcs_manager_stack_push(ntg_fcs_manager* fm, const struct ntg_fcs_scope* scope_copy)
{
    if(!fm || !scope_copy)
        return NTG_ERR_INV_ARG;

    int _status;

    struct ntg_fcs_scope_list_node* head = fm->priv.scope_stack->head;

    if(head)
    {
        if(head->data.scope.block_mode == NTG_FCS_SCOPE_BLOCK_TRUE)
            return 0;
    }

    if(scope_copy->root) 
    {
        size_t layer_count = ntg_scene_collect_layers_by_z(fm->ro.scene, NULL, 0);
        if(layer_count == 0) return NTG_ERR_BAD_FCS_SCOPE;

        ntg_widget** layers = malloc(layer_count * sizeof(ntg_widget*));
        if(!layers) return NTG_ERR_ALLOC_FAIL;

        ntg_scene_collect_layers_by_z(fm->ro.scene, layers, layer_count);

        size_t i;
        bool desc_of_any_layer = false;
        for(i = 0; i < layer_count; i++)
        {
            if(scope_copy->root && ntg_widget_is_in_tree(layers[i], scope_copy->root))
                desc_of_any_layer = true;
        }

        free(layers);

        if(!desc_of_any_layer)
            return NTG_ERR_BAD_FCS_SCOPE;
    }

    if(head)
    {
        head->data.last_focused = fm->ro.focused;
    }

    struct ntg_fcs_scope_data data = {
        .scope = (*scope_copy),
        .last_focused = NULL,
        .valid = true
    };

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

    ntg_fcs_manager_request_focus(fm, NULL);

    struct ntg_event_fcs_manager_scpsh_dt event_dt = { .scope = scope_copy };
    ntg_object_event_raise(ntg_obj(fm), NTG_EVENT_FCS_MANAGER_SCPSH, &event_dt);

    return 0;
}

int ntg_fcs_manager_stack_pop(ntg_fcs_manager* fm)
{
    if(!fm) return NTG_ERR_INV_ARG;
    
    if(fm->priv.scope_stack->size < 2)
        return 0;

    scope_stack_pop(fm);

    scope_stack_sync(fm);

    return 0;
}

const struct ntg_fcs_scope* ntg_fcs_manager_stack_get_active(ntg_fcs_manager* fm)
{
    if(!fm) return NULL;

    return (fm->priv.scope_stack->head ?
            &(fm->priv.scope_stack->head->data.scope) :
            NULL);
}

size_t ntg_fcs_manager_stack_get_size(const ntg_fcs_manager* fm)
{
    if(!fm) return 0;

    return (fm->priv.scope_stack ? fm->priv.scope_stack->size : 0);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_fcs_manager_feed_key(ntg_fcs_manager* fm, nt_key key)
{
    if(!fm) return false;
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    /* Retrieve scope data, scope */

    const struct ntg_fcs_scope_data* scope_data = 
            (fm->priv.scope_stack->head ?
            &(fm->priv.scope_stack->head->data) :
            NULL);
    if(!scope_data) return false;
    const struct ntg_fcs_scope scope = scope_data->scope;

    /* Init ctx */

    struct ntg_fcs_scope_ctx ctx = {
        .data = scope.data,
        .fm = fm,
        .last_focused = scope_data->last_focused,
        .root = scope.root
    };

    /* Feed to scope */

    if(!ntg__fcs_scope_handle_keybind(&scope, &ctx, key))
    {
        if(scope.dispatch_key_fn)
            return scope.dispatch_key_fn(&ctx, key);
        else
            return ntg_fcs_scope_key_dispatch_fn(&ctx, key);
    }

    return false;
}

/* ------------------------------------------------------ */

bool ntg_fcs_manager_feed_mouse(ntg_fcs_manager* fm, nt_mouse mouse)
{
    if(!fm) return false;
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;

    /* Retrieve scope data, scope */

    const struct ntg_fcs_scope_data* scope_data = 
            (fm->priv.scope_stack->head ?
            &(fm->priv.scope_stack->head->data) :
            NULL);
    if(!scope_data) return false;
    const struct ntg_fcs_scope scope = scope_data->scope;

    /* Init ctx */

    struct ntg_fcs_scope_ctx ctx = {
        .data = scope.data,
        .fm = fm,
        .last_focused = scope_data->last_focused,
        .root = scope.root
    };

    ntg_xy pos = ntg_xy_new(mouse.x, mouse.y);

    int _status;
    struct ntg_scene_hit_res res;
    _status = ntg_scene_hit_test(fm->ro.scene, pos, &res);
    if(_status != 0) /* Click failed, tell the callers not to consume */
        return true;

    ntg_widget* hit = res.res.widget;

    if(!hit)
    {
        if(scope.out_click_mode == NTG_FCS_SCOPE_OUT_CLICK_CLR)
            ntg_fcs_manager_request_focus(fm, NULL);

        return false;
    }
    
    mouse.x = res.res.local_pos.ro.x;
    mouse.y = res.res.local_pos.ro.y;

    if((!scope.root) || ntg_widget_is_in_tree(scope.root, hit))
    {
        ntg__fcs_scope_handle_mouse_focus(&scope, &ctx, mouse, hit);
        if(scope.dispatch_mouse_fn)
            return scope.dispatch_mouse_fn(&ctx, mouse, hit);
        else
            return ntg_fcs_scope_mouse_dispatch_fn(&ctx, mouse, hit);
    }
    else 
    {
        if(scope.out_click_mode == NTG_FCS_SCOPE_OUT_CLICK_CLR)
            ntg_fcs_manager_request_focus(fm, NULL);

        if(scope.input_mode == NTG_FCS_SCOPE_INPUT_MODELESS)
        {
            struct ntg_widget_mouse event = {
                .mouse = mouse,
                .target = hit,
                .from_keybind = false
            };
            
            if(hit->ro.clickable)
                return ntg_widget_feed_mouse(hit, &event);
            else
                return false;
        }
        else
            return false;
    }

    return false;
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
        struct ntg_fcs_scope_keys init_scope_keys)
{
    if(!fm || !scene) return NTG_ERR_INV_ARG;

    int status = ntg_object_init_inherit(
            ntg_obj(fm),
            &VTABLE,
            &NTG_TYPE_FCS_MANAGER);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(fm);

    fm->priv.scope_stack = malloc(sizeof(struct ntg_fcs_scope_list));
    if(!fm->priv.scope_stack)
    {
        ntg_object_zero(fm);
        ntg_object_deinit(ntg_obj(fm));
        return NTG_ERR_ALLOC_FAIL;
    }

    *fm->priv.scope_stack = (struct ntg_fcs_scope_list) {0};

    fm->ro.scene = scene;
    fm->ro.focused = NULL;

    struct ntg_fcs_scope scope = {
        .data = NULL,
        .root = NULL,
        .keys = init_scope_keys
    };

    status = ntg_fcs_manager_stack_push(fm, &scope);
    switch(status)
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

        ntg_fcs_scope_list_deinit(fm->priv.scope_stack);

        free(fm->priv.scope_stack);
    }

    ntg_object_zero(fm);
    ntg_object_deinit(ntg_obj(fm));
}


/* ------------------------------------------------------ */
/* INVALIDATE */
/* ------------------------------------------------------ */

void ntg__fcs_manager_on_scene_widget_rm(ntg_fcs_manager* fm, ntg_widget* removed)
{
    if(!fm) return;

    if(removed && (fm->ro.focused == removed))
        ntg_fcs_manager_request_focus(fm, NULL);

    struct ntg_fcs_scope_list_node* it_node = fm->priv.scope_stack->head;
    struct ntg_fcs_scope_data* it_data;

    while(it_node)
    {
        it_data = &it_node->data;

        if(it_data->scope.root && ntg_widget_is_in_tree(it_data->scope.root, removed))
            it_data->valid = false;

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

static void fcs_manager_deinit_fn(ntg_object* _fm)
{
    ntg__fcs_manager_deinit((ntg_fcs_manager*)_fm);
}

static void scope_stack_pop(ntg_fcs_manager* fm)
{
    struct ntg_fcs_scope_list_node* old_head = fm->priv.scope_stack->head;
    if(!old_head) return; /* Can't pop last scope */

    struct ntg_fcs_scope popped_scope = old_head->data.scope;

    ntg_fcs_scope_list_popf(fm->priv.scope_stack);
    
    struct ntg_fcs_scope_list_node* head = fm->priv.scope_stack->head;
    if(!head)
        return;

    ntg_fcs_manager_request_focus(fm, head->data.last_focused);
    head->data.last_focused = NULL;

    struct ntg_event_fcs_manager_scpop_dt event_dt = { .scope = &popped_scope };
    ntg_object_event_raise(ntg_obj(fm), NTG_EVENT_FCS_MANAGER_SCPOP, &event_dt);
}

static void scope_stack_sync(ntg_fcs_manager* fm)
{
    if(!fm || !fm->priv.scope_stack) return;

    struct ntg_fcs_scope_list_node* head = fm->priv.scope_stack->head;
    while(head && !head->data.valid && (fm->priv.scope_stack->size > 1))
    {
        scope_stack_pop(fm);
        head = fm->priv.scope_stack->head;
    }

    if(head && !head->data.valid)
        head->data.valid = true;
}
