#include "ntg.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

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
struct ntg_border_style flt_rounded_border, def_rounded_border;
ntg_main_panel root;
ntg_label north;
ntg_box center, south, south_box;
ntg_label s_label, sb_label1, sb_label2, sb_label3;
ntg_color_block c_cb1, c_cb2;
ntg_stage stage;
ntg_scene scene;
ntg_loop loop;

ntg_button flt_button;

unsigned int sflt_counter = 0;

struct ntg_anchor_policy flt_ap;

ntg_label sflt_label;
struct ntg_anchor_policy sflt_ap;

struct ntg_focus_scope fs1, fs2;

void init_north();
void init_center();
void init_south();
void init_flt_button();
void init_sflt_label();
void init_root();
void init_bs(); // border styles
void init_ap(); // attach policies
void init_fs(); // focus scopes

const struct ntg_focus_scope_vtable FS1_VTABLE;

bool flt_button_mouse_fn(ntg_object* object, const struct ntg_object_mouse* event)
{
    if(event->from_keybind)
    {
        ntg_loop_break(&loop, true);
        return true;
    }
    else return false;
}

bool loop_on_event_fn(ntg_loop* loop, struct nt_event event)
{
    bool consumed = ntg_loop_dispatch_event(loop, event);
    if(consumed) return true;

    if(event.type == NT_EVENT_KEY)
    {
        struct nt_key_event key = *(struct nt_key_event*)event.data;
        if(nt_key_event_utf32_check_alt(key, 'q', false))
        {
            ntg_loop_break(loop, true);
            return true;
        }
    }

    return false;
}

bool fs1_dispatch_mouse_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    struct ntg_xy curr_scroll = (ntg_txt(&north))->_scroll;

    if(mouse.type == NT_MOUSE_SCROLL_DOWN)
        curr_scroll.y += 1;
    else if(mouse.type == NT_MOUSE_SCROLL_UP)
        curr_scroll.y = ((curr_scroll.y > 0) ? (curr_scroll.y - 1) : 0);

    ntg_text_set_scroll(ntg_txt(&north), curr_scroll);

    return true;
}

/*
bool fs1_handle_mouse_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    bool consumed = ntg_focus_scope_dispatch_mouse_bubble_fn(scope, mouse, clicked);
    if(consumed) return true;

    // ntg_object_remove_from_scene(clicked);

    return true;
}
*/

void sflt_on_mouse_fn(ntg_object* _label, const struct ntg_object_mouse* event)
{
    ntg_label* label = ntg_lbl(_label);

    if(event->mouse.type == NT_MOUSE_CLICK_LEFT)
        sflt_counter++;
    else if(event->mouse.type == NT_MOUSE_CLICK_RIGHT)
        sflt_counter--;

    char buff[50];
    sprintf(buff, "Broj klikova: %d", sflt_counter);

    ntg_label_set_text_unsafe(label, buff, 0, NULL);
}

int main(int argc, char *argv[])
{
    int _status;
    struct ntg_opts opts = ntg_opts_def();
    opts.alt_screen_mode = NTG_ALT_SCREEN_DISABLE;
    ntg_enable(&opts, "/home/novak/Desktop/ntg_log.txt", &_status);
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

    ntg_scene_init(&scene, NULL, &_status);
    ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_void, NULL, &_status);

    ntg_stage_init(&stage, &_status);
    ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_void, NULL, &_status);

    ntg_loop_init(&loop, &stage, NULL, 60, 4, loop_on_event_fn, &_status);
    ntg_cleanup_batch_add(batch, &loop, ntg_loop_deinit_void, NULL, &_status);

    // ATTACH ROOTS, SCENE, STAGE

    ntg_scene_set_root(&scene, ntg_obj(&root), &_status);
    ntg_stage_set_scene(&stage, &scene, &_status);
    ntg_object_anchor(ntg_obj(&root), ntg_obj(&flt_button), &flt_ap, &_status);
    ntg_object_anchor(ntg_obj(&flt_button), ntg_obj(&sflt_label), &sflt_ap, &_status);

    // ntg_focus_manager_push_scope(scene._fm, &fs2, &_status);
    ntg_focus_manager_push_scope(scene._fm, &fs1, &_status);

    ntg_loop_exit_status loop_status = ntg_loop_run(&loop, &_status);
    ntg_log_log("LOOP END | STATUS: %d", loop_status);

    ntg_cleanup_batch_finish(batch);

    ntg_disable();

    return 0;
}

void init_north()
{
    int _status;

    struct nt_gfx label_gfx = {
        .fg = nt_color_new_auto(255, 255, 255),
        .bg = nt_color_new_auto(143, 0, 255),
        .style = nt_style_new_uniform(NT_STYLE_VAL_BOLD)
    };
    struct ntg_label_opts north_label_opts = ntg_label_opts_def();
    north_label_opts.text_opts.gfx = label_gfx;
    north_label_opts.text_opts.indent = 2;
    north_label_opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;

    ntg_label_init(&north, &north_label_opts, &_status);
    ntg_cleanup_batch_add(batch, &north, ntg_label_deinit_void, NULL, &_status);
    ntg_label_set_text_unsafe(&north, lorem, 0, &_status);

    struct ntg_padding_opts north_pad_opts = ntg_padding_opts_def();
    north_pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    //ntg_object_set_padding_opts(ntg_obj(&north), &north_pad_opts);
}

void init_center()
{
    int _status;

    ntg_box_init(&center, NULL, &_status);
    ntg_cleanup_batch_add(batch, &center, ntg_box_deinit_void, NULL, &_status);

    ntg_color_block_init(&c_cb1, nt_color_new_auto(200, 0, 40), &_status);
    ntg_cleanup_batch_add(batch, &c_cb1, ntg_color_block_deinit_void, NULL, &_status);

    ntg_color_block_init(&c_cb2, nt_color_new_auto(40, 0, 200), &_status);
    ntg_cleanup_batch_add(batch, &c_cb2, ntg_color_block_deinit_void, NULL, &_status);

    struct ntg_layout_opts center_layout_opts = (ntg_obj(&center))->_layout_opts;
    center_layout_opts.min_cont_size.y = 15;
    ntg_object_set_layout_opts(ntg_obj(&center), &center_layout_opts);
    
    // CONNECT

    ntg_box_add_child(&center, ntg_obj(&c_cb1), &_status);
    ntg_box_add_child(&center, ntg_obj(&c_cb2), &_status);
}

void init_south()
{
    int _status;

    struct ntg_padding_opts pad_opts = ntg_padding_opts_def();
    pad_opts.pref_size = ntg_insets(1, 1, 1, 1);

    struct ntg_border_opts border_opts = ntg_border_opts_def();
    border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    border_opts.style = &def_rounded_border;
    border_opts.enable = NTG_OBJECT_DCR_ENABLE_ALWAYS;

    // SOUTH BOX

    struct ntg_box_opts south_box_opts = ntg_box_opts_def();
    south_box_opts.orient = NTG_ORIENT_V;
    south_box_opts.spacing = 1;

    ntg_box_init(&south_box, &south_box_opts, &_status);
    ntg_cleanup_batch_add(batch, &south_box, ntg_box_deinit_void, NULL, &_status);

    ntg_object_set_border_opts(ntg_obj(&south_box), &border_opts);

    // SOUTH BOX LABEL1

    struct ntg_label_opts sb_label1_opts = ntg_label_opts_def();
    sb_label1_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(250, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_init(&sb_label1, &sb_label1_opts, &_status);
    ntg_cleanup_batch_add(batch, &sb_label1, ntg_label_deinit_void, NULL, &_status);
    ntg_label_set_text_unsafe(&sb_label1, "Test1", 0, &_status);

    ntg_object_set_padding_opts(ntg_obj(&sb_label1), &pad_opts);

    // SOUTH BOX LABEL2

    struct ntg_label_opts sb_label2_opts = ntg_label_opts_def();
    sb_label2_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(150, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_init(&sb_label2, &sb_label2_opts, &_status);
    ntg_cleanup_batch_add(batch, &sb_label2, ntg_label_deinit_void, NULL, &_status);

    ntg_label_set_text_unsafe(&sb_label2, "Test2", 0, &_status);
    ntg_object_set_padding_opts(ntg_obj(&sb_label2), &pad_opts);

    // SOUTH BOX LABEL3

    struct ntg_label_opts sb_label3_opts = ntg_label_opts_def();
    sb_label3_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(100, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    sb_label3_opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;

    ntg_label_init(&sb_label3, &sb_label3_opts, &_status);
    ntg_cleanup_batch_add(batch, &sb_label3, ntg_label_deinit_void, NULL, &_status);

    ntg_label_set_text_unsafe(&sb_label3, lorem, 0, &_status);
    
    // CONNECT

    ntg_box_add_child(&south_box, ntg_obj(&sb_label1), &_status);
    ntg_box_add_child(&south_box, ntg_obj(&sb_label2), &_status);
    ntg_box_add_child(&south_box, ntg_obj(&sb_label3), &_status);

    // SOUTH LABEL

    struct ntg_label_opts s_label_opts = ntg_label_opts_def();
    s_label_opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(50, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_set_opts(&s_label, &s_label_opts);
    ntg_object_set_padding_opts(ntg_obj(&s_label), &pad_opts);

    ntg_label_init(&s_label, &s_label_opts, &_status);
    ntg_cleanup_batch_add(batch, &s_label, ntg_label_deinit_void, NULL, &_status);

    ntg_label_set_text_unsafe(&s_label, "ABCD", 0, &_status);

    ntg_object_set_border_opts(ntg_obj(&s_label), &border_opts);

    // SOUTH

    struct ntg_box_opts south_opts = ntg_box_opts_def();
    south_opts.bg = ntg_vcell_bg(nt_color_new_auto(255, 255, 0));

    ntg_box_init(&south, &south_opts, &_status);
    ntg_cleanup_batch_add(batch, &south, ntg_box_deinit_void, NULL, &_status);

    ntg_object_set_border_opts(ntg_obj(&south), &border_opts);

    ntg_box_add_child(&south, ntg_obj(&s_label), &_status);
    ntg_box_add_child(&south, ntg_obj(&south_box), &_status);
}

void init_flt_button()
{
    int _status;

    struct ntg_button_opts opts = ntg_button_opts_def();
    opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;
    opts.text_opts.bg_mode = NTG_TEXT_BG_FLT;
    opts.text_opts.focused_gfx = (struct nt_gfx) {
        .fg = nt_color_new_auto(255, 165, 0),
        .style = nt_style_new_uniform(NT_STYLE_VAL_BOLD),
        .bg = nt_color_new_auto(255, 255, 255)
    };

    ntg_button_init(&flt_button, &opts, flt_button_mouse_fn, &_status);
    ntg_cleanup_batch_add(batch, &flt_button, ntg_button_deinit_void, NULL, &_status);

    ntg_button_set_text_unsafe(&flt_button, "Floating button example", 0, &_status);

    struct ntg_layout_opts flt_button_layout_opts = (ntg_obj(&flt_button))->_layout_opts;
    flt_button_layout_opts.z_index = 1;
    ntg_object_set_layout_opts(ntg_obj(&flt_button), &flt_button_layout_opts);

    struct ntg_padding_opts pad_opts = ntg_padding_opts_def();
    pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    ntg_object_set_padding_opts(ntg_obj(&flt_button), &pad_opts);

    struct ntg_border_opts border_opts = ntg_border_opts_def();
    border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    border_opts.style = &flt_rounded_border;
    ntg_object_set_border_opts(ntg_obj(&flt_button), &border_opts);

    // ntg_object_set_user_min_size_cont(ntg_obj(&flt_button), ntg_xy(1000, 1000));
}

void init_sflt_label()
{
    int _status;

    struct ntg_label_opts opts = ntg_label_opts_def();
    ntg_log_log("ABCD LABEL | ROOT | SOUTH: %p %p %p", &s_label, &root, &south);

    opts.text_opts.wrap = NTG_TEXT_WRAP_WORD;
    opts.text_opts.line_mode = NTG_TEXT_LINE_JUSTIFY;
    opts.text_opts.line_mode = NTG_TEXT_LINE_ALIGN;
    opts.text_opts.sec_align = NTG_ALIGN_3;
    opts.text_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(255, 255, 255),
        .fg = nt_color_new_auto(0, 0, 0),
        .style = nt_style_new_uniform(NT_STYLE_VAL_ITALIC)
    };
    ntg_label_set_opts(&sflt_label, &opts);

    ntg_label_init(&sflt_label, &opts, &_status);

    ntg_obj(&sflt_label)->hooks.on_mouse_fn = sflt_on_mouse_fn;

    struct ntg_layout_opts sflt_label_layout_opts = (ntg_obj(&sflt_label))->_layout_opts;
    sflt_label_layout_opts.z_index = 2;
    ntg_object_set_layout_opts(ntg_obj(&sflt_label), &sflt_label_layout_opts);

    ntg_label_set_text_unsafe(&sflt_label, "Floating label example - Sidefloat", 0, &_status);

    ntg_cleanup_batch_add(batch, &sflt_label, ntg_label_deinit_void, NULL, &_status);

    ntg_object_set_clickable(ntg_obj(&sflt_label), NTG_OBJECT_CLICKABLE_BORDER);
}

struct ntg_object_vtable root_vtable;

void init_root()
{
    int _status;

    ntg_main_panel_init(&root, NULL, &_status);

    ntg_cleanup_batch_add(batch, &root, ntg_main_panel_deinit_void, NULL, &_status);
    ntg_main_panel_set(&root, ntg_obj(&north), NTG_MAIN_PANEL_NORTH, &_status);
    ntg_main_panel_set(&root, ntg_obj(&center), NTG_MAIN_PANEL_CENTER, &_status);
    ntg_main_panel_set(&root, ntg_obj(&south), NTG_MAIN_PANEL_SOUTH, &_status);
}

void init_bs()
{
    int _status;

    ntg_border_style_init_rounded(&flt_rounded_border, NT_GFX_DEFAULT, true, &_status);
    ntg_cleanup_batch_add(batch, &flt_rounded_border, ntg_border_style_deinit_, NULL, &_status);
    ntg_border_style_init_rounded(&def_rounded_border, NT_GFX_DEFAULT, false, &_status);
    ntg_cleanup_batch_add(batch, &def_rounded_border, ntg_border_style_deinit_, NULL, &_status);
}

void init_ap()
{
    int _status;

    struct ntg_float_policy_opts flt_opts = ntg_float_policy_opts_def();
    flt_opts.prim_align = NTG_ALIGN_2;
    flt_opts.sec_align = NTG_ALIGN_2;
    flt_opts.enable = NTG_FLOAT_POLICY_ENABLE_ALWAYS;

    ntg_anchor_policy_init_float(&flt_ap, &flt_opts, &_status);

    ntg_cleanup_batch_add(batch, &flt_ap, ntg_anchor_policy_deinit_, NULL, &_status);

    struct ntg_sidefloat_policy_opts sflt_opts = ntg_sidefloat_policy_opts_def();
    sflt_opts.align = NTG_ALIGN_2;
    sflt_opts.side = NTG_SIDE_W;
    sflt_opts.thresh = NTG_SIDEFLOAT_POLICY_THRESH_ALWAYS;

    ntg_anchor_policy_init_sidefloat(&sflt_ap, &sflt_opts, &_status);

    ntg_cleanup_batch_add(batch, &sflt_ap, ntg_anchor_policy_deinit_, NULL, &_status);
}

const struct ntg_focus_scope_vtable FS1_VTABLE = {
    .dispatch_key_fn = ntg_focus_scope_dispatch_key_bubble_fn,
    .dispatch_mouse_fn = fs1_dispatch_mouse_fn
};

void init_fs()
{
    struct ntg_focus_scope_opts opts = ntg_focus_scope_opts_def();
    opts.input_mode = NTG_FOCUS_SCOPE_INPUT_MODAL;
    opts.out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_CLR;

    ntg_focus_scope_init_override(&fs1, &FS1_VTABLE, ntg_obj(&root), NULL, &opts, NULL);

    // ntg_focus_scope_init(&fs2, NULL, NULL, NULL, NULL);
}
