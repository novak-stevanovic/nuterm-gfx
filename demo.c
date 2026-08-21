#include "nt.h"
#include "ntg.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

void set_breakpoint() {}

const char* lorem = 
"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi ullamcorper "
"a diam ut mollis. Sed sed diam eu erat consequat finibus pulvinar eu eros. "
"Donec accumsan scelerisque dolor. Aenean consequat ac massa et fringilla. "
"Aliquam sem leo, tincidunt convallis lorem sed, laoreet lacinia urna. "
"Mauris ut mi lectus. Quisque felis sapien, bibendum eu vehicula sed, "
"semper non ligula. Nam nec nibh felis. Suspendisse rutrum neque a orci "
"vestibulum euismod. Fusce egestas, ante sit amet vehicula maximus, neque "
"dui imperdiet turpis, sit amet tempus metus sapien a est. Cras rhoncus "
"odio libero, at tristique neque aliquam vitae. Praesent varius libero sed "
"malesuada rhoncus. Cras felis ligula, ultrices a mi at, ultrices convallis "
"leo. Pellentesque nibh risus, efficitur at massa eget, pretium mattis "
"neque.\n\n"

"Sed a dapibus lacus. Vestibulum id ex sem. Fusce eleifend iaculis mollis. "
"Phasellus hendrerit tempus ligula nec pulvinar. Sed vitae enim et eros "
"sagittis ultrices ut in orci. Sed sed tellus iaculis ex fermentum mattis. "
"Proin feugiat diam eget tortor sollicitudin, a aliquet justo porta. "
"Integer cursus venenatis sem quis elementum. In id nibh lacus. Nullam ut "
"tempus mauris, id pharetra lectus. Nullam porta arcu eget turpis faucibus, "
"nec eleifend quam semper.\n\n"

"Nulla ut feugiat sem. Fusce hendrerit fringilla ipsum. Etiam at quam nisi. "
"Donec magna arcu, posuere in odio nec, commodo eleifend neque. Praesent "
"eleifend sit amet arcu id posuere. Sed congue tellus sed tellus laoreet, "
"dignissim posuere libero pharetra. In in dui lacus. Pellentesque maximus "
"pulvinar faucibus. Interdum et malesuada fames ac ante ipsum primis in "
"faucibus. Aliquam a posuere diam. In iaculis varius mauris, non tincidunt "
"dui porta in. Suspendisse non purus erat.\n\n"

"Morbi sed magna eros. Maecenas rhoncus blandit nisi, in tincidunt diam "
"dignissim vitae. Integer in placerat lorem, a elementum ipsum. Sed posuere "
"at erat quis congue. Sed molestie ex diam, porttitor sodales dolor "
"ultricies quis. Aliquam erat volutpat. Duis ac consequat justo. In feugiat "
"vitae elit porttitor lacinia. In hac habitasse platea dictumst. Proin "
"vulputate magna quis feugiat ultrices. Morbi eget libero nec sem facilisis "
"vestibulum non et nibh. Fusce vestibulum mollis rhoncus. In hac habitasse "
"platea dictumst. Ut in consequat ipsum. Sed tempus, risus id tincidunt "
"volutpat, tortor lacus tempor ligula, vitae pretium ante metus ac odio.\n\n"

"Integer euismod malesuada quam ut vulputate. Vivamus aliquet volutpat nunc, "
"et pellentesque tortor laoreet in. Morbi blandit tincidunt purus non "
"maximus. Nunc nec justo massa. Nulla ut volutpat felis. In ac luctus augue. "
"Ut aliquet purus felis, a maximus felis volutpat nec. Maecenas condimentum "
"dolor ut euismod tempor. In hac habitasse platea dictumst. Proin eget "
"vestibulum diam.";

ntg_cleanup_batch* batch;
ntg_border_9x flt_rounded_border, def_rounded_border;
ntg_main_panel root;
ntg_label north;
ntg_box center, south, south_box;
ntg_label s_label, sb_label1, sb_label2, sb_label3;
ntg_clr_block c_cb1, c_cb2;
ntg_stage stage;
ntg_scene scene;

ntg_button flt_button;

unsigned int sflt_counter = 0;
ntg_event_binding sflt_mouse_binding = {0};

ntg_float flt_ap;

ntg_label sflt_label;
ntg_sidefloat sflt_ap;

struct ntg_fcs_scope fs1, fs2;

void init_north();
void init_center();
void init_south();
void init_flt_button();
void init_sflt_label();
void init_root();
void init_bs(); // border styles
void init_ap(); // attach policies
void init_fs(); // focus scopes


const struct ntg_fcs_scope_vtable FS1_VTABLE;

void task_loop_stop(void* _)
{
    ntg_loop_stop();
}

void task_change_text_1(void* _)
{
    ntg_button_set_text_unsafe(&flt_button, "1", NTG_TEXT_SET_RM_WS);
}

void task_change_text_2a(void* _)
{
    ntg_button_set_text_unsafe(&flt_button, "2a", NTG_TEXT_SET_RM_WS);
}

void task_change_text_2b(void* _)
{
    ntg_button_set_text_unsafe(&flt_button, "2b", NTG_TEXT_SET_RM_WS);
}

bool flt_button_mouse_fn(ntg_button* button)
{
    int _status;
    _status = ntg_loop_schedule(task_loop_stop, NULL, 5000);
    _status = ntg_loop_schedule(task_change_text_1, NULL, 1000);
    _status = ntg_loop_schedule(task_change_text_2a, NULL, 2000);
    _status = ntg_loop_schedule(task_change_text_2b, NULL, 2000);
    assert(!_status);

    return true;
}

bool loop_on_event_fn(const struct nt_event* event)
{
    if(!event) return false;

    bool consumed = ntg_loop_dispatch_event_fn_default(event);
    if(consumed) return true;

    if(event->type == NT_EVENT_KEY)
    {
        struct nt_key key;
        NT_EVENT_FILL_DATA((*event), &key);
        if(nt_key_utf32_match_alt(key, 'q', false))
        {
            ntg_loop_stop();
            return true;
        }
    }

    return false;
}

bool fs1_dispatch_key_fn(ntg_fcs_scope* scope, struct nt_key key)
{
    if(nt_key_esc_match(key, NT_ESC_KEY_ARR_UP))
    {
        ntg_text_scroll(ntg_txt(&north), ntg_dxy(0, -1));
        return true;
    }
    else if(nt_key_esc_match(key, NT_ESC_KEY_ARR_RIGHT))
    {
        ntg_text_scroll(ntg_txt(&north), ntg_dxy(1, 0));
        return true;
    }
    else if(nt_key_esc_match(key, NT_ESC_KEY_ARR_DOWN))
    {
        ntg_text_scroll(ntg_txt(&north), ntg_dxy(0, 1));
        return true;
    }
    else if(nt_key_esc_match(key, NT_ESC_KEY_ARR_LEFT))
    {
        ntg_text_scroll(ntg_txt(&north), ntg_dxy(-1, 0));
        return true;
    }
    else if(nt_key_utf32_match(key, 'w'))
    {
        struct ntg_label_opts north_opts;
        ntg_label_get_opts(&north, &north_opts);

        north_opts.text_opts.wrap = (north_opts.text_opts.wrap + 1) % 3;
        ntg_label_set_opts(&north, &north_opts);

        return true;
    }

    return false;
}

bool fs1_dispatch_mouse_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked)
{
    if(ntg_fcs_scope_dispatch_mouse_fn(scope, mouse, clicked))
        return true;

    struct ntg_xy curr_scroll = (ntg_txt(&north))->ro.scroll;

    if(mouse.type == NT_MOUSE_SCROLL_DOWN)
        curr_scroll.y += 1;
    else if(mouse.type == NT_MOUSE_SCROLL_UP)
        curr_scroll.y = ((curr_scroll.y > 0) ? (curr_scroll.y - 1) : 0);

    ntg_text_set_scroll(ntg_txt(&north), curr_scroll);

    return true;
}

/*
bool fs1_handle_mouse_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked)
{
    bool consumed = ntg_fcs_scope_dispatch_mouse_bubble_fn(scope, mouse, clicked);
    if(consumed) return true;

    // ntg_object_remove_from_scene(clicked);

    return true;
}
*/

void sflt_on_mouse_fn(void* subscriber, struct ntg_event event)
{
    if(event.type == NTG_EVENT_OBJECT_MOUSE)
    {
        ntg_label* label = subscriber;
        const struct ntg_event_object_mouse_dt* event_dt = event.data;
        if(!event_dt || !event_dt->mouse) return;

        if(event_dt->mouse->mouse.type == NT_MOUSE_CLICK_LEFT)
            sflt_counter++;
        else if(event_dt->mouse->mouse.type == NT_MOUSE_CLICK_RIGHT)
            sflt_counter--;

        char buff[50];
        sprintf(buff, "Broj klikova: %d", sflt_counter);

        ntg_label_set_text_unsafe(label, buff, 0);
    }
}

int main(int argc, char *argv[])
{
    int _status;
    struct ntg_opts opts = ntg_opts_default();
    // opts.alt_screen_mode = NTG_ALT_SCREEN_DISABLE;
    // opts.cursor_mode = NTG_CURSOR_SHOW;
    _status = ntg_enable(&opts, "ntg_log.txt");
    assert(!_status);

    batch = ntg_cleanup_batch_new();

    // INIT BORDER

    init_bs();
    init_ap();
    init_north();
    init_center();
    init_south();
    init_flt_button();
    init_sflt_label();
    init_root();
    init_fs();

    _status = ntg_scene_init(&scene, NULL, NTG_SCENE_MAX_IT_AUTO);
    _status = ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_void, NULL);

    _status = ntg_stage_init(&stage);
    _status = ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_void, NULL);

    _status = ntg_loop_init(
        NULL,
        loop_on_event_fn,
        &stage,
        NULL);
    assert(!_status);

    // ATTACH ROOTS, SCENE, STAGE

    _status = ntg_scene_add_root(&scene, ntg_obj(&root));
    _status = ntg_stage_set_scene(&stage, &scene);
    _status = ntg_object_anchor(ntg_obj(&root), ntg_obj(&flt_button));
    _status = ntg_object_anchor(ntg_obj(&flt_button), ntg_obj(&sflt_label));

    assert(!ntg_object_set_anchor_policy(ntg_obj(&flt_button), ntg_ap(&flt_ap)));
    assert(!ntg_object_set_anchor_policy(ntg_obj(&sflt_label), ntg_ap(&sflt_ap)));

    // ntg_fcs_manager_push_scope(scene.ro.fm, &fs2, &_status);
    _status = ntg_fcs_manager_stack_push(scene.ro.fm, &fs1);

    struct ntg_loop_start_opts loop_start_opts = ntg_loop_start_opts_default();
    loop_start_opts.mouse_mode = NTG_LOOP_MOUSE_ENABLE;
    _status = ntg_loop_start(&loop_start_opts);
    ntg_log_log("STATUS: %d", _status);
    assert(!_status);

    set_breakpoint();

    nt_cursor_move(0, 0);
    nt_write_str_unsafe("Loop end", NT_GFX_DEFAULT);

    nt_buffer_flush();

    set_breakpoint();

    char c = getchar();
    if(c == 'a')
    {
        _status = ntg_loop_start(&loop_start_opts);
        assert(!_status);
    }

    ntg_cleanup_batch_finish(batch);

    _status = ntg_loop_deinit();
    assert(!_status);

    ntg_disable();

    return 0;
}

void init_north()
{
    int _status;

    struct nt_gfx label_gfx = {
        .fg = nt_color_new_auto(255, 255, 255),
        .bg = nt_color_new_auto(143, 0, 255),
        .style = nt_style_new_uniform(NT_STYLE_BOLD)
    };
    struct ntg_label_opts north_label_opts = ntg_label_opts_default();
    north_label_opts.text_opts.gfx = label_gfx;
    north_label_opts.text_opts.indent = 2;
    north_label_opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;

    _status = ntg_label_init(&north, &north_label_opts);
    _status = ntg_cleanup_batch_add(batch, &north, ntg_label_deinit_void, NULL);
    _status = ntg_label_set_text_unsafe(&north, lorem, 0);
}

void init_center()
{
    int _status;

    _status = ntg_box_init(&center, NULL);
    _status = ntg_cleanup_batch_add(batch, &center, ntg_box_deinit_void, NULL);

    _status = ntg_clr_block_init(&c_cb1, nt_color_new_auto(200, 0, 40));
    _status = ntg_cleanup_batch_add(batch, &c_cb1, ntg_clr_block_deinit_void, NULL);

    _status = ntg_clr_block_init(&c_cb2, nt_color_new_auto(40, 0, 200));
    _status = ntg_cleanup_batch_add(batch, &c_cb2, ntg_clr_block_deinit_void, NULL);

    struct ntg_lay_opts center_layout_opts = (ntg_obj(&center))->ro.layout_opts;
    center_layout_opts.min_cont_size.y = 15;
    ntg_object_set_lay_opts(ntg_obj(&center), &center_layout_opts);
    
    // CONNECT

    _status = ntg_box_add_child(&center, ntg_obj(&c_cb1));
    _status = ntg_box_add_child(&center, ntg_obj(&c_cb2));
}

void init_south()
{
    int _status;

    struct ntg_pad_opts pad_opts = ntg_padding_opts_default();
    pad_opts.pref_size = ntg_insets(1, 1, 1, 1);
    pad_opts.enable = NTG_OBJECT_DCR_ENABLE_ALWAYS;

    struct ntg_bdr_opts border_opts = ntg_bdr_opts_default();
    border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    border_opts.style = &def_rounded_border.priv.base;
    border_opts.enable = NTG_OBJECT_DCR_ENABLE_ALWAYS;

    // SOUTH BOX

    struct ntg_box_opts south_box_opts = ntg_box_opts_default();
    south_box_opts.orient = NTG_ORIENT_V;
    south_box_opts.spacing = 1;

    _status = ntg_box_init(&south_box, &south_box_opts);
    _status = ntg_cleanup_batch_add(batch, &south_box, ntg_box_deinit_void, NULL);

    ntg_object_set_bdr_opts(ntg_obj(&south_box), &border_opts);

    // SOUTH BOX LABEL1

    struct ntg_label_opts sb_label1_opts = ntg_label_opts_default();
    sb_label1_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(250, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    _status = ntg_label_init(&sb_label1, &sb_label1_opts);
    _status = ntg_cleanup_batch_add(batch, &sb_label1, ntg_label_deinit_void, NULL);
    _status = ntg_label_set_text_unsafe(&sb_label1, "Test1", 0);

    ntg_object_set_pad_opts(ntg_obj(&sb_label1), &pad_opts);

    // SOUTH BOX LABEL2

    struct ntg_label_opts sb_label2_opts = ntg_label_opts_default();
    sb_label2_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(150, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    _status = ntg_label_init(&sb_label2, &sb_label2_opts);
    _status = ntg_cleanup_batch_add(batch, &sb_label2, ntg_label_deinit_void, NULL);

    _status = ntg_label_set_text_unsafe(&sb_label2, "Test2", 0);
    ntg_object_set_pad_opts(ntg_obj(&sb_label2), &pad_opts);

    // SOUTH BOX LABEL3

    struct ntg_label_opts sb_label3_opts = ntg_label_opts_default();
    sb_label3_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(100, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    sb_label3_opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;

    _status = ntg_label_init(&sb_label3, &sb_label3_opts);
    _status = ntg_cleanup_batch_add(batch, &sb_label3, ntg_label_deinit_void, NULL);

    _status = ntg_label_set_text_unsafe(&sb_label3, lorem, 0);
    ntg_object_set_pad_opts(ntg_obj(&sb_label3), &pad_opts);
    
    // CONNECT

    _status = ntg_box_add_child(&south_box, ntg_obj(&sb_label1));
    _status = ntg_box_add_child(&south_box, ntg_obj(&sb_label2));
    _status = ntg_box_add_child(&south_box, ntg_obj(&sb_label3));

    // SOUTH LABEL

    struct ntg_label_opts s_label_opts = ntg_label_opts_default();
    s_label_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(50, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_set_opts(&s_label, &s_label_opts);

    _status = ntg_label_init(&s_label, &s_label_opts);
    ntg_object_set_pad_opts(ntg_obj(&s_label), &pad_opts);

    _status = ntg_cleanup_batch_add(batch, &s_label, ntg_label_deinit_void, NULL);

    _status = ntg_label_set_text_unsafe(&s_label, "ABCD", 0);

    ntg_object_set_bdr_opts(ntg_obj(&s_label), &border_opts);

    // SOUTH

    struct ntg_box_opts south_opts = ntg_box_opts_default();
    south_opts.bg = ntg_vcell_new_full_bg(nt_color_new_auto(255, 255, 0));

    _status = ntg_box_init(&south, &south_opts);
    _status = ntg_cleanup_batch_add(batch, &south, ntg_box_deinit_void, NULL);

    ntg_object_set_bdr_opts(ntg_obj(&south), &border_opts);

    _status = ntg_box_add_child(&south, ntg_obj(&s_label));
    _status = ntg_box_add_child(&south, ntg_obj(&south_box));
}

void init_flt_button()
{
    int _status;

    struct ntg_button_opts opts = ntg_button_opts_default();
    opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;
    opts.text_opts.bg_mode = NTG_TEXT_BG_FLT;
    opts.text_opts.focused_gfx = (struct nt_gfx) {
        .fg = nt_color_new_auto(255, 165, 0),
        .style = nt_style_new_uniform(NT_STYLE_BOLD),
        .bg = nt_color_new_auto(255, 255, 255)
    };

    _status = ntg_button_init(&flt_button, &opts, flt_button_mouse_fn);
    _status = ntg_cleanup_batch_add(batch, &flt_button, ntg_button_deinit_void, NULL);

    _status = ntg_button_set_text_unsafe(&flt_button, "Floating button example", 0);

    struct ntg_lay_opts flt_button_layout_opts = (ntg_obj(&flt_button))->ro.layout_opts;
    flt_button_layout_opts.z_index = 1;
    ntg_object_set_lay_opts(ntg_obj(&flt_button), &flt_button_layout_opts);

    struct ntg_pad_opts pad_opts = ntg_padding_opts_default();
    pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    ntg_object_set_pad_opts(ntg_obj(&flt_button), &pad_opts);

    struct ntg_bdr_opts border_opts = ntg_bdr_opts_default();
    border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    border_opts.style = &flt_rounded_border.priv.base;
    ntg_object_set_bdr_opts(ntg_obj(&flt_button), &border_opts);

    // ntg_object_set_user_min_size_cont(ntg_obj(&flt_button), ntg_xy(1000, 1000));
}

void init_sflt_label()
{
    int _status;

    struct ntg_label_opts opts = ntg_label_opts_default();
    ntg_log_log("ABCD LABEL | ROOT | SOUTH: %p %p %p", &s_label, &root, &south);

    opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;
    opts.text_opts.line_mode = NTG_TEXT_LINE_JUSTIFY;
    opts.text_opts.line_mode = NTG_TEXT_LINE_ALIGN;
    opts.text_opts.sec_align = NTG_ALIGN_3;
    opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(255, 255, 255),
        .fg = nt_color_new_auto(0, 0, 0),
        .style = nt_style_new_uniform(NT_STYLE_ITALIC)
    };
    ntg_label_set_opts(&sflt_label, &opts);

    _status = ntg_label_init(&sflt_label, &opts);

    _status = ntg_event_bind(
            &ntg_obj(&sflt_label)->ro.event_dlgt,
            &sflt_label,
            sflt_on_mouse_fn,
            &sflt_mouse_binding);

    struct ntg_lay_opts sflt_label_layout_opts = (ntg_obj(&sflt_label))->ro.layout_opts;
    sflt_label_layout_opts.z_index = 2;
    ntg_object_set_lay_opts(ntg_obj(&sflt_label), &sflt_label_layout_opts);

    _status = ntg_label_set_text_unsafe(&sflt_label, "Floating label example - Sidefloat", 0);

    _status = ntg_cleanup_batch_add(batch, &sflt_label, ntg_label_deinit_void, NULL);

    ntg_object_set_clickable(ntg_obj(&sflt_label), NTG_OBJECT_CLICKABLE_BDR);
}

void init_root()
{
    int _status;

    _status = ntg_main_panel_init(&root, NULL);

    _status = ntg_cleanup_batch_add(batch, &root, ntg_main_panel_deinit_void, NULL);
    _status = ntg_main_panel_set(&root, ntg_obj(&north), NTG_MAIN_PANEL_NORTH);
    _status = ntg_main_panel_set(&root, ntg_obj(&center), NTG_MAIN_PANEL_CENTER);
    _status = ntg_main_panel_set(&root, ntg_obj(&south), NTG_MAIN_PANEL_SOUTH);
}

void init_bs()
{
    int _status;

    _status = ntg_border_9x_init_rounded(
            &flt_rounded_border,
            NT_GFX_DEFAULT,
            true);
    _status = ntg_cleanup_batch_add(
            batch,
            &flt_rounded_border,
            ntg_border_9x_deinit_void,
            NULL);

    _status = ntg_border_9x_init_rounded(
            &def_rounded_border,
            NT_GFX_DEFAULT,
            false);
    _status = ntg_cleanup_batch_add(
            batch,
            &def_rounded_border,
            ntg_border_9x_deinit_void,
            NULL);
}

void init_ap()
{
    int _status;

    struct ntg_float_opts flt_opts = ntg_float_opts_default();
    flt_opts.prim_align = NTG_ALIGN_2;
    flt_opts.sec_align = NTG_ALIGN_2;
    flt_opts.enable = NTG_FLOAT_ENABLE_ALWAYS;

    _status = ntg_float_init(&flt_ap, &flt_opts);

    _status = ntg_cleanup_batch_add(
            batch,
            &flt_ap,
            ntg_float_deinit_void,
            NULL);

    struct ntg_sidefloat_opts sflt_opts = ntg_sidefloat_opts_default();
    sflt_opts.align = NTG_ALIGN_2;
    sflt_opts.side = NTG_SIDE_W;
    sflt_opts.thresh = NTG_SIDEFLOAT_THRESH_ALWAYS;

    _status = ntg_sidefloat_init(&sflt_ap, &sflt_opts);

    _status = ntg_cleanup_batch_add(
            batch,
            &sflt_ap,
            ntg_sidefloat_deinit_void,
            NULL);
}

const struct ntg_fcs_scope_vtable FS1_VTABLE = {
    .dispatch_key_fn = fs1_dispatch_key_fn,
    .dispatch_mouse_fn = fs1_dispatch_mouse_fn
};

void init_fs()
{
    struct ntg_fcs_scope_opts opts = ntg_fcs_scope_opts_default();
    opts.input_mode = NTG_FCS_SCOPE_INPUT_MODAL;
    opts.out_click_mode = NTG_FCS_SCOPE_OUT_CLICK_CLR;

    ntg_fcs_scope_init_inherit(&fs1, &FS1_VTABLE, ntg_obj(NULL), NULL, &opts);

    // ntg_fcs_scope_init(&fs2, NULL, NULL, NULL, NULL);
}
