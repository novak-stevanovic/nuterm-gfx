#include <assert.h>

#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_drawable_vec.h"

static bool __ntg_process_key_fn_default(
        ntg_drawable* drawable,
        struct nt_key_event key_event,
        struct ntg_process_key_context context)
{
    return false;
}

static void __ntg_on_focus_fn_default(
        ntg_drawable* drawable,
        struct ntg_focus_context context)
{}

void __ntg_drawable_init__(ntg_drawable* drawable, /* non-NULL */
        void* user, /* non-NULL */
        ntg_measure_fn measure_fn, /* non-NULL */
        ntg_constrain_fn constrain_fn, /* non-NULL */
        ntg_arrange_fn arrange_fn, /* non-NULL */
        ntg_draw_fn draw_fn, /* non-NULL */
        ntg_get_children_fn_ get_children_fn_, /* non-NULL */
        ntg_get_children_fn get_children_fn, /* non-NULL */
        ntg_get_parent_fn_ get_parent_fn_, /* non-NULL */
        ntg_get_parent_fn get_parent_fn, /* non-NULL */
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn)
{
    assert(drawable != NULL);
    assert(user != NULL);
    
    assert(measure_fn != NULL);
    assert(constrain_fn != NULL);
    assert(arrange_fn != NULL);
    assert(draw_fn != NULL);
    assert(get_children_fn_ != NULL);
    assert(get_children_fn != NULL);
    assert(get_parent_fn_ != NULL);
    assert(get_parent_fn != NULL);

    drawable->__user = user;
    drawable->_measure_fn = measure_fn;
    drawable->_constrain_fn = constrain_fn;
    drawable->_arrange_fn = arrange_fn;
    drawable->_draw_fn = draw_fn;
    drawable->_get_children_fn_ = get_children_fn_;
    drawable->_get_children_fn = get_children_fn;
    drawable->_get_parent_fn_ = get_parent_fn_;
    drawable->_get_parent_fn = get_parent_fn;
    drawable->_process_key_fn = (process_key_fn != NULL) ?
        process_key_fn :
        __ntg_process_key_fn_default;
    drawable->_on_focus_fn = (on_focus_fn != NULL) ?
        on_focus_fn :
        __ntg_on_focus_fn_default;
}

void __ntg_drawable_deinit__(ntg_drawable* drawable)
{
    assert(drawable != NULL);

    (*drawable) = (ntg_drawable) {0};
}

const void* ntg_drawable_user(const ntg_drawable* drawable)
{
    assert(drawable != NULL);

    return drawable->__user;
}

void* ntg_drawable_user_(ntg_drawable* drawable)
{
    assert(drawable != NULL);

    return drawable->__user;
}

bool ntg_drawable_is_ancestor(const ntg_drawable* drawable, const ntg_drawable* ancestor)
{
    assert(drawable != NULL);
    assert(ancestor != NULL);

    const ntg_drawable* it_drawable = drawable->_get_parent_fn(drawable);

    while(it_drawable != NULL)
    {
        if(it_drawable == ancestor) return true;

        it_drawable = (const ntg_drawable*)(it_drawable->_get_parent_fn(drawable));
    }

    return false;
}

bool ntg_drawable_is_descendant(const ntg_drawable* drawable, const ntg_drawable* descendant)
{
    assert(drawable != NULL);
    assert(descendant != NULL);

    return ntg_drawable_is_ancestor(descendant, drawable);
}

void ntg_drawable_tree_perform(ntg_drawable* drawable,
        ntg_drawable_perform_mode mode,
        void (*perform_fn)(ntg_drawable* drawable, void* data),
        void* data)
{
    assert(drawable != NULL);
    assert(perform_fn != NULL);

    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t count = ntg_drawable_vec_view_count(&children);

    size_t i;
    if(mode == NTG_DRAWABLE_PERFORM_TOP_DOWN)
    {
        perform_fn(drawable, data);

        for(i = 0; i < count; i++)
        {
            ntg_drawable_tree_perform(
                    ntg_drawable_vec_view_at(&children, i),
                    mode, perform_fn, data);
        }
    }
    else
    {
        for(i = 0; i < count; i++)
        {
            ntg_drawable_tree_perform(
                    ntg_drawable_vec_view_at(&children, i),
                    mode, perform_fn, data);
        }

        perform_fn(drawable, data);
    }
}
