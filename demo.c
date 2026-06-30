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

ntg_label flt_label;

unsigned int sflt_counter = 0;

struct ntg_anchor_policy flt_ap;

ntg_label sflt_label;
struct ntg_anchor_policy sflt_ap;

struct ntg_focus_scope fs1, fs2;

void init_north();
void init_center();
void init_south();
void init_flt_label();
void init_sflt_label();
void init_root();
void init_bs(); // border styles
void init_ap(); // attach policies
void init_fs(); // focus scopes

bool loop_on_event_fn(ntg_loop* loop, struct nt_event event)
{
    bool consumed = ntg_loop_dispatch_event(loop, event);
    if(consumed) return true;

    if(event.type == NT_EVENT_KEY)
    {
        struct nt_key_event key = *(struct nt_key_event*)event.data;
        if(nt_key_event_utf32_check(key, 'q', false))
        {
            ntg_loop_break(loop, true);
            return true;
        }
    }

    return false;
}

bool fs1_on_key_fn(
        void* _,
        const struct ntg_focus_key_ctx* ctx,
        struct nt_key_event key)
{
    if(nt_key_event_utf32_check(key, '9', false))
    {
        ntg_loop_break(&loop, true);
        return true;
    }
    return false;
}

bool fs1_on_mouse_fn(
        void* data,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse)
{
    bool consumed = ntg_focus_scope_dispatch_mouse_stc(data, ctx, mouse);
    if(consumed) return true;

    ntg_object_remove_from_scene(ctx->clicked);

    return true;
}

bool sflt_on_mouse_fn(ntg_object* _label, struct nt_mouse_event mouse)
{
    ntg_label* label = ntg_lbl(_label);

    if(mouse.type == NT_MOUSE_EVENT_CLICK_LEFT)
        sflt_counter++;
    else if(mouse.type == NT_MOUSE_EVENT_CLICK_RIGHT)
        sflt_counter--;

    char buff[50];
    sprintf(buff, "Broj klikova: %d", sflt_counter);

    ntg_label_set_text(label, buff, NULL);

    return true;
}

void gui_fn1(void* _)
{
    batch = ntg_cleanup_batch_new();

    // INIT BORDER

    init_bs();
    init_ap();
    init_north();
    init_center();
    init_south();
    init_flt_label();
    init_sflt_label();
    init_root();
    init_fs();

    int _status;

    ntg_scene_init(&scene, &_status);
    ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_, NULL, &_status);

    ntg_stage_init(&stage, &_status);
    ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_, NULL, &_status);

    ntg_loop_init(&loop, &stage, NULL, 60, 4, loop_on_event_fn, &_status);
    ntg_cleanup_batch_add(batch, &loop, ntg_loop_deinit_, NULL, &_status);

    // ATTACH ROOTS, SCENE, STAGE

    ntg_scene_set_root(&scene, ntg_obj(&root), &_status);
    ntg_stage_set_scene(&stage, &scene, &_status);
    ntg_object_anchor(ntg_obj(&root), ntg_obj(&flt_label), &flt_ap, &_status);
    ntg_object_anchor(ntg_obj(&flt_label), ntg_obj(&sflt_label), &sflt_ap, &_status);

    ntg_focus_manager_push_scope(scene._fm, &fs2, &_status);
    ntg_focus_manager_push_scope(scene._fm, &fs1, &_status);

    ntg_loop_exit_status loop_status = ntg_loop_run(&loop, &_status);
    ntg_log_log("LOOP END | STATUS: %d", loop_status);

    ntg_cleanup_batch_finish(batch);
}

int main(int argc, char *argv[])
{
    ntg_init();

    ntg_launch(gui_fn1, NULL);

    ntg_wait();

    ntg_deinit();

    // printf("%lu", sizeof(struct ntg_border_style_custom_dt));

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
    north_label_opts.gfx = label_gfx;
    north_label_opts.indent = 2;
    north_label_opts.wrap = NTG_LABEL_WRAP_WORD;

    ntg_label_init(&north, &north_label_opts, &_status);
    ntg_cleanup_batch_add(batch, &north, ntg_label_deinit_, NULL, &_status);
    ntg_label_set_text(&north, lorem, &_status);

    struct ntg_padding_opts north_pad_opts = ntg_padding_opts_def();
    north_pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    //ntg_object_set_padding_opts(ntg_obj(&north), &north_pad_opts);
}

void init_center()
{
    int _status;

    ntg_box_init(&center, NULL, &_status);
    ntg_cleanup_batch_add(batch, &center, ntg_box_deinit_, NULL, &_status);

    ntg_color_block_init(&c_cb1, nt_color_new_auto(200, 0, 40), &_status);
    ntg_cleanup_batch_add(batch, &c_cb1, ntg_color_block_deinit_, NULL, &_status);

    ntg_color_block_init(&c_cb2, nt_color_new_auto(40, 0, 200), &_status);
    ntg_cleanup_batch_add(batch, &c_cb2, ntg_color_block_deinit_, NULL, &_status);

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
    ntg_cleanup_batch_add(batch, &south_box, ntg_box_deinit_, NULL, &_status);

    ntg_object_set_border_opts(ntg_obj(&south_box), &border_opts);

    // SOUTH BOX LABEL1

    struct ntg_label_opts sb_label1_opts = ntg_label_opts_def();
    sb_label1_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(250, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_init(&sb_label1, &sb_label1_opts, &_status);
    ntg_cleanup_batch_add(batch, &sb_label1, ntg_label_deinit_, NULL, &_status);
    ntg_label_set_text(&sb_label1, "Test1", &_status);

    ntg_object_set_padding_opts(ntg_obj(&sb_label1), &pad_opts);

    // SOUTH BOX LABEL2

    struct ntg_label_opts sb_label2_opts = ntg_label_opts_def();
    sb_label2_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(150, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_init(&sb_label2, &sb_label2_opts, &_status);
    ntg_cleanup_batch_add(batch, &sb_label2, ntg_label_deinit_, NULL, &_status);

    ntg_label_set_text(&sb_label2, "Test2", &_status);
    ntg_object_set_padding_opts(ntg_obj(&sb_label2), &pad_opts);

    // SOUTH BOX LABEL3

    struct ntg_label_opts sb_label3_opts = ntg_label_opts_def();
    sb_label3_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(100, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    sb_label3_opts.wrap = NTG_LABEL_WRAP_WORD;

    ntg_label_init(&sb_label3, &sb_label3_opts, &_status);
    ntg_cleanup_batch_add(batch, &sb_label3, ntg_label_deinit_, NULL, &_status);

    ntg_label_set_text(&sb_label3, lorem, &_status);
    
    // CONNECT

    ntg_box_add_child(&south_box, ntg_obj(&sb_label1), &_status);
    ntg_box_add_child(&south_box, ntg_obj(&sb_label2), &_status);
    ntg_box_add_child(&south_box, ntg_obj(&sb_label3), &_status);

    // SOUTH

    struct ntg_label_opts s_label_opts = ntg_label_opts_def();
    s_label_opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(50, 0, 0),
        .fg = nt_color_new_auto(255, 255, 255),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_set_opts(&s_label, &s_label_opts);
    ntg_object_set_padding_opts(ntg_obj(&s_label), &pad_opts);

    ntg_label_init(&s_label, &s_label_opts, &_status);
    ntg_cleanup_batch_add(batch, &s_label, ntg_label_deinit_, NULL, &_status);

    ntg_label_set_text(&s_label, "ABCD", &_status);

    ntg_object_set_border_opts(ntg_obj(&s_label), &border_opts);

    ntg_box_init(&south, NULL, &_status);
    ntg_cleanup_batch_add(batch, &south, ntg_box_deinit_, NULL, &_status);

    ntg_object_set_border_opts(ntg_obj(&south), &border_opts);

    ntg_box_add_child(&south, ntg_obj(&s_label), &_status);
    ntg_box_add_child(&south, ntg_obj(&south_box), &_status);
}

void init_flt_label()
{
    int _status;

    struct ntg_label_opts label_opts = ntg_label_opts_def();
    label_opts.wrap = NTG_LABEL_WRAP_WORD;
    label_opts.bg_mode = NTG_LABEL_BG_FLT;

    ntg_label_init(&flt_label, &label_opts, &_status);
    ntg_cleanup_batch_add(batch, &flt_label, ntg_label_deinit_, NULL, &_status);

    ntg_label_set_text(&flt_label, "Floating label example", &_status);

    struct ntg_layout_opts flt_label_layout_opts = (ntg_obj(&flt_label))->_layout_opts;
    flt_label_layout_opts.z_index = 1;
    ntg_object_set_layout_opts(ntg_obj(&flt_label), &flt_label_layout_opts);

    struct ntg_padding_opts pad_opts = ntg_padding_opts_def();
    pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    ntg_object_set_padding_opts(ntg_obj(&flt_label), &pad_opts);

    struct ntg_border_opts border_opts = ntg_border_opts_def();
    border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    border_opts.style = &flt_rounded_border;
    ntg_object_set_border_opts(ntg_obj(&flt_label), &border_opts);

    // ntg_object_set_user_min_size_cont(ntg_obj(&flt_label), ntg_xy(1000, 1000));
}

void init_sflt_label()
{
    int _status;

    struct ntg_label_opts opts = ntg_label_opts_def();
    ntg_log_log("ABCD LABEL | ROOT | SOUTH: %p %p %p", &s_label, &root, &south);

    opts.wrap = NTG_LABEL_WRAP_WORD;
    opts.text_mode = NTG_LABEL_TEXT_JUSTIFY;
    opts.sec_align = NTG_ALIGN_1;
    opts.gfx = (struct nt_gfx) {
        .bg = nt_color_new_auto(255, 255, 255),
        .fg = nt_color_new_auto(0, 0, 0),
        .style = nt_style_new_uniform(NT_STYLE_VAL_ITALIC)
    };
    ntg_label_set_opts(&sflt_label, &opts);

    ntg_obj(&sflt_label)->hooks.on_mouse_fn = sflt_on_mouse_fn;

    ntg_label_init(&sflt_label, &opts, &_status);

    struct ntg_layout_opts sflt_label_layout_opts = (ntg_obj(&sflt_label))->_layout_opts;
    sflt_label_layout_opts.z_index = 2;
    ntg_object_set_layout_opts(ntg_obj(&sflt_label), &sflt_label_layout_opts);

    ntg_label_set_text(&sflt_label, "Floating label example - Sidefloat", &_status);

    ntg_cleanup_batch_add(batch, &sflt_label, ntg_label_deinit_, NULL, &_status);
}

void init_root()
{
    int _status;

    ntg_main_panel_init(&root, NULL, &_status);

    ntg_cleanup_batch_add(batch, &root, ntg_main_panel_deinit_, NULL, &_status);
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

void init_fs()
{
    fs1 = (struct ntg_focus_scope) {
        .root = ntg_obj(&south),
        .input_mode = NTG_FOCUS_SCOPE_INPUT_MODAL,
        .out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
        .block_mode = NTG_FOCUS_SCOPE_BLOCK_FALSE,
        .on_key_fn = fs1_on_key_fn,
        .on_mouse_fn = fs1_on_mouse_fn,
        .data = NULL
    };

    fs2 = (struct ntg_focus_scope) {
        .root = NULL,
        .input_mode = NTG_FOCUS_SCOPE_INPUT_MODELESS,
        .out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
        .block_mode = NTG_FOCUS_SCOPE_BLOCK_FALSE,
        .on_key_fn = ntg_focus_scope_dispatch_key,
        .on_mouse_fn = ntg_focus_scope_dispatch_mouse_stc,
        .data = NULL
    };
}
