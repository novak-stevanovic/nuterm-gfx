#include "nt.h"
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

ntg_garbage* garbage;
ntg_border_9x flt_rounded_border, def_rounded_border;
ntg_panel root;
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

void init_north();
void init_center();
void init_south();
void init_flt_button();
void init_sflt_label();
void init_root();
void init_bs(); // border styles
void init_ap(); // attach policies

void task_loop_stop(void* _)
{
    ntg_loop_stop();
}

void task_change_text_1(void* _)
{
    ntg_button_set_text_cstr(&flt_button, "1");
}

void task_change_text_2a(void* _)
{
    ntg_button_set_text_cstr(&flt_button, "2a");
}

void task_change_text_2b(void* _)
{
    ntg_button_set_text_cstr(&flt_button, "2b");
}

bool flt_button_mouse_fn(ntg_button* button)
{
    int status;
    /*
    status = ntg_loop_schedule(task_loop_stop, NULL, 5000);
    status = ntg_loop_schedule(task_change_text_1, NULL, 1000);
    status = ntg_loop_schedule(task_change_text_2a, NULL, 2000);
    status = ntg_loop_schedule(task_change_text_2b, NULL, 2000);
    assert(!status);

    return true;
    */

    return false;
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
        if(nt_key_utf32_match(key, 'q'))
        {
            ntg_loop_stop();
            return true;
        }
        if(nt_key_utf32_match(key, 'd'))
        {
            ntg_button_disable(&flt_button);
            return true;
        }
        if(nt_key_utf32_match(key, 'e'))
        {
            ntg_button_enable(&flt_button);
            return true;
        }
    }

    return false;
}

/*
bool fs1_dispatch_key_fn(ntg_fcs_scope* scope, struct nt_key key)
{
    if(nt_key_esc_match(key, NT_ESC_KEY_ARR_UP))
    {
        ntg_text_wgt_scroll(ntg_txt(&north), ntg_dxy(0, -1));
        return true;
    }
    else if(nt_key_esc_match(key, NT_ESC_KEY_ARR_RIGHT))
    {
        ntg_text_wgt_scroll(ntg_txt(&north), ntg_dxy(1, 0));
        return true;
    }
    else if(nt_key_esc_match(key, NT_ESC_KEY_ARR_DOWN))
    {
        ntg_text_wgt_scroll(ntg_txt(&north), ntg_dxy(0, 1));
        return true;
    }
    else if(nt_key_esc_match(key, NT_ESC_KEY_ARR_LEFT))
    {
        ntg_text_wgt_scroll(ntg_txt(&north), ntg_dxy(-1, 0));
        return true;
    }
    else if(nt_key_utf32_match(key, 'w'))
    {
        struct ntg_label_opts north_opts = north.ro.opts;

        north_opts.text_opts.wrap = (north_opts.text_opts.wrap + 1) % 3;
        ntg_label_set_opts(&north, &north_opts);

        return true;
    }

    return false;
}

bool fs1_dispatch_mouse_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_widget* clicked)
{
    if(ntg_fcs_scope_dispatch_mouse_fn(scope, mouse, clicked))
        return true;

    struct ntg_xy curr_scroll = (ntg_txt(&north))->ro.scroll;

    if(mouse.type == NT_MOUSE_SCROLL_DOWN)
        curr_scroll = ntg_xy_add_y(curr_scroll, 1);
    else if(mouse.type == NT_MOUSE_SCROLL_UP)
        curr_scroll = ntg_xy_sub_y(curr_scroll, 1);

    ntg_text_wgt_set_scroll(ntg_txt(&north), curr_scroll);

    return true;
}
*/

void sflt_on_mouse_fn(ntg_object* subscriber, struct ntg_event event)
{
    if(event.type == NTG_EVENT_WIDGET_MOUSE)
    {
        ntg_label* label = ntg_lbl(subscriber);
        const struct ntg_event_widget_mouse_dt* event_dt = event.data;
        if(!event_dt || !event_dt->mouse) return;

        if(event_dt->mouse->mouse.type == NT_MOUSE_CLICK_LEFT)
            sflt_counter++;
        else if(event_dt->mouse->mouse.type == NT_MOUSE_CLICK_RIGHT)
            sflt_counter--;

        char buff[50];
        sprintf(buff, "Broj klikova: %d", sflt_counter);

        ntg_label_set_text_cstr(label, buff);
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    int status;
    struct ntg_opts opts = {0};
    // opts.alt_screen_mode = NTG_ALT_SCREEN_DISABLE;
    status = ntg_enable(&opts, "ntg_log.txt");
    // status = ntg_enable(&opts, NULL);
    assert(!status);

    garbage = ntg_garbage_new(0);
    if(!garbage) return 1;

    // INIT BORDER

    init_bs();
    init_ap();
    init_north();
    init_center();
    init_south();
    init_flt_button();
    init_sflt_label();
    init_root();

    status = ntg_scene_init(&scene, NULL);
    assert(!status);
    ntg_garbage_add_obj(garbage, &scene);
    ntg_obj_set_name(&scene, "scene");

    status = ntg_stage_init(&stage, NULL);
    assert(!status);
    ntg_garbage_add_obj(garbage, &stage);
    ntg_obj_set_name(&stage, "stage");

    struct ntg_loop_init_opts loop_init_opts = {
        .on_event_fn = loop_on_event_fn,
        .stage = &stage
    };
    status = ntg_loop_init(&loop_init_opts);
    assert(!status);

    // ATTACH ROOTS, SCENE, STAGE

    status = ntg_scene_add_root(&scene, ntg_wgt(&root));
    status = ntg_stage_set_scene(&stage, &scene);
    status = ntg_widget_anchor(ntg_wgt(&root), ntg_wgt(&flt_button));
    status = ntg_widget_anchor(ntg_wgt(&flt_button), ntg_wgt(&sflt_label));

    assert(!ntg_widget_set_anchor_policy(ntg_wgt(&flt_button), ntg_ap(&flt_ap)));
    assert(!ntg_widget_set_anchor_policy(ntg_wgt(&sflt_label), ntg_ap(&sflt_ap)));

    struct ntg_loop_start_opts loop_start_opts = {
        .mouse_mode = NTG_LOOP_MOUSE_ENABLE
    };
    status = ntg_loop_start(&loop_start_opts);
    ntg_log_log("STATUS: %d", status);
    assert(!status);

    nt_cursor_move(0, 0);
    nt_write_str_unsafe("Loop end", NT_GFX_ZERO);

    nt_buffer_flush();

    char c = getchar();
    if(c == 'a')
    {
        status = ntg_loop_start(&loop_start_opts);
        assert(!status);
    }

    ntg_garbage_destroy(garbage);

    status = ntg_loop_deinit();
    assert(!status);

    ntg_disable();

    return 0;
}

void init_north()
{
    int status;

    struct nt_gfx label_gfx = {
        .fg = nt_color_new_auto(255, 255, 255),
        .bg = nt_color_new_auto(143, 0, 255),
        .style = NT_STYLE_STRIKETHROUGH
    }; 
    struct ntg_label_opts north_label_opts = {
        .line_mode = NTG_TEXT_LINE_JUSTIFY,
        .gfx = label_gfx,
        .indent = 2
    };
    status = ntg_label_init(&north, &north_label_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &north);
    ntg_obj_set_name(&north, "north");

    status = ntg_label_set_text_cstr(&north, lorem);
    assert(!status);
}

void init_center()
{
    int status;

    status = ntg_box_init(&center, NULL);
    assert(!status);
    ntg_garbage_add_obj(garbage, &center);
    ntg_obj_set_name(&center, "center");

    status = ntg_clr_block_init(&c_cb1, nt_color_new_auto(200, 0, 40));
    assert(!status);
    ntg_garbage_add_obj(garbage, &c_cb1);
    ntg_obj_set_name(&c_cb1, "c_cb1");

    status = ntg_clr_block_init(&c_cb2, nt_color_new_auto(40, 0, 200));
    assert(!status);
    ntg_garbage_add_obj(garbage, &c_cb2);
    ntg_obj_set_name(&c_cb2, "c_cb2");

    struct ntg_lay_conf center_lay_conf;
    ntg_lay_conf_init(&center_lay_conf);
    center_lay_conf.cont_min_size = ntg_xy_new(0, 15);
    ntg_widget_set_lay_conf(ntg_wgt(&center), &center_lay_conf);
    
    // CONNECT

    status = ntg_box_add_child(&center, ntg_wgt(&c_cb1));
    status = ntg_box_add_child(&center, ntg_wgt(&c_cb2));
}

void init_south()
{
    int status;

    struct ntg_pad_opts pad_opts = {
        .pref_size = ntg_insets_new(1, 1, 1, 1),
        .enable = NTG_WIDGET_DCR_ENABLE_ALWAYS
    };

    struct ntg_bdr_opts bdr_opts = {
        .pref_size = ntg_insets_new(1, 1, 1, 1),
        .style = &def_rounded_border._base,
        .enable = NTG_WIDGET_DCR_ENABLE_ALWAYS
    };

    // SOUTH BOX

    struct ntg_box_opts south_box_opts = {
        .orient = NTG_ORIENT_V,
        .bg = ntg_vcell_new_full_bg(
                NT_COLOR_ZERO,
                nt_color_new_auto(255, 255, 255),
                0)
    };

    status = ntg_box_init(&south_box, &south_box_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &south_box);
    ntg_obj_set_name(&south_box, "south_box");

    // ntg_widget_set_pad_opts(ntg_wgt(&south_box), &pad_opts);

    ntg_widget_set_bdr_opts(ntg_wgt(&south_box), &bdr_opts);

    // SOUTH BOX LABEL1

    struct ntg_label_opts sb_label1_opts = {
        .gfx = {
            .bg = nt_color_new_auto(250, 0, 0),
            .fg = nt_color_new_auto(255, 255, 255)
        }
    };
    status = ntg_label_init(&sb_label1, &sb_label1_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &sb_label1);
    ntg_obj_set_name(&sb_label1, "sb_label1");

    status = ntg_label_set_text_cstr(&sb_label1, "Test1");
    assert(!status);

    ntg_widget_set_pad_opts(ntg_wgt(&sb_label1), &pad_opts);

    // SOUTH BOX LABEL2

    struct ntg_label_opts sb_label2_opts = {
        .gfx = {
            .bg = nt_color_new_auto(150, 0, 0),
            .fg = nt_color_new_auto(255, 255, 255)
        }
    };
    status = ntg_label_init(&sb_label2, &sb_label2_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &sb_label2);
    ntg_obj_set_name(&sb_label2, "sb_label2");

    status = ntg_label_set_text_cstr(&sb_label2, "Test2");
    ntg_widget_set_pad_opts(ntg_wgt(&sb_label2), &pad_opts);

    // SOUTH BOX LABEL3

    struct ntg_label_opts sb_label3_opts = {
        .gfx = {
            .bg = nt_color_new_auto(100, 0, 0),
            .fg = nt_color_new_auto(255, 255, 255)
        }
    };

    status = ntg_label_init(&sb_label3, &sb_label3_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &sb_label3);
    ntg_obj_set_name(&sb_label3, "sb_label3");

    status = ntg_label_set_text_cstr(&sb_label3, lorem);
    ntg_widget_set_pad_opts(ntg_wgt(&sb_label3), &pad_opts);
    
    // CONNECT

    status = ntg_box_add_child(&south_box, ntg_wgt(&sb_label1));
    status = ntg_box_add_child(&south_box, ntg_wgt(&sb_label2));
    status = ntg_box_add_child(&south_box, ntg_wgt(&sb_label3));

    // SOUTH LABEL

    struct ntg_label_opts s_label_opts = {
        .gfx = {
            .bg = nt_color_new_auto(50, 0, 0),
            .fg = nt_color_new_auto(255, 255, 255),
        }
    };
    status = ntg_label_init(&s_label, &s_label_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &s_label);
    ntg_obj_set_name(&s_label, "s_label");

    ntg_widget_set_pad_opts(ntg_wgt(&s_label), &pad_opts);

    status = ntg_label_set_text_cstr(&s_label, "ABCD");
    assert(!status);

    ntg_widget_set_bdr_opts(ntg_wgt(&s_label), &bdr_opts);

    // SOUTH

    struct ntg_box_opts south_opts = {
        .bg = ntg_vcell_new_full_bg(
                NT_COLOR_ZERO,
                nt_color_new_auto(255, 255, 0),
                0)
    };

    status = ntg_box_init(&south, &south_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &south);
    ntg_obj_set_name(&south, "south");

    ntg_widget_set_bdr_opts(ntg_wgt(&south), &bdr_opts);

    status = ntg_box_add_child(&south, ntg_wgt(&s_label));
    status = ntg_box_add_child(&south, ntg_wgt(&south_box));
}

void init_flt_button()
{
    int status;

    struct ntg_button_opts opts = {
        .wrap = NTG_TEXT_WRAP_WORD,
        .bg_mode = NTG_TEXT_BG_OVERLAY,
        .focused_gfx = {
            .fg = NT_COLOR_ZERO,
            .bg = nt_color_new_auto(0, 255, 0),
            .style = NT_STYLE_BOLD | NT_STYLE_REVERSE
        },
        .disabled_gfx = {
            .style = NT_STYLE_STRIKETHROUGH
        }
    };

    status = ntg_button_init(&flt_button, &opts, flt_button_mouse_fn);
    assert(!status);
    ntg_garbage_add_obj(garbage, &flt_button);
    ntg_obj_set_name(&flt_button, "flt_button");

    status = ntg_button_set_text_cstr(&flt_button, "Floating button example");

    ntg_widget_set_z_index(ntg_wgt(&flt_button), 1);

    struct ntg_pad_opts pad_opts = {
        .pref_size = ntg_insets_new(2, 2, 2, 2)
    };
    ntg_widget_set_pad_opts(ntg_wgt(&flt_button), &pad_opts);

    struct ntg_bdr_opts border_opts = {
        .pref_size = ntg_insets_new(1, 1, 1, 1),
        .style = &flt_rounded_border._base
    };
    ntg_widget_set_bdr_opts(ntg_wgt(&flt_button), &border_opts);

    // ntg_widget_set_user_min_size_cont(ntg_wgt(&flt_button), ntg_xy(1000, 1000));
}

void init_sflt_label()
{
    int status;

    struct ntg_label_opts opts = {
        .wrap = NTG_TEXT_WRAP_WORD,
        .line_mode = NTG_TEXT_LINE_ALIGN,
        .sec_align = NTG_ALIGN_3,
        .gfx = {
            .bg = nt_color_new_auto(255, 255, 255),
            .fg = nt_color_new_auto(0, 0, 0),
            .style = NT_STYLE_ITALIC
        }
    };
    status = ntg_label_init(&sflt_label, &opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &sflt_label);
    ntg_obj_set_name(&sflt_label, "sflt_label");

    status = ntg_object_event_bind(
            ntg_obj(&sflt_label),
            ntg_obj(&sflt_label),
            sflt_on_mouse_fn,
            &sflt_mouse_binding);

    ntg_widget_set_z_index(ntg_wgt(&sflt_label), 2);

    status = ntg_label_set_text_cstr(&sflt_label, "Floating label example - Sidefloat");

    ntg_widget_set_clickable(ntg_wgt(&sflt_label), NTG_WIDGET_CLKABLE_BDR);
}

void init_root()
{
    int status;

    status = ntg_panel_init(&root, NULL);
    assert(!status);
    ntg_garbage_add_obj(garbage, &root);
    ntg_obj_set_name(&root, "root");

    status = ntg_panel_set(&root, ntg_wgt(&north), NTG_PANEL_POS_N);
    status = ntg_panel_set(&root, ntg_wgt(&center), NTG_PANEL_POS_C);
    status = ntg_panel_set(&root, ntg_wgt(&south), NTG_PANEL_POS_S);
}

void init_bs()
{
    int status;

    status = ntg_border_9x_init_rounded_overlay(&flt_rounded_border, NT_COLOR_ZERO, 0);
    assert(!status);
    ntg_garbage_add_obj(garbage, &flt_rounded_border);

    status = ntg_border_9x_init_rounded(&def_rounded_border, NT_GFX_ZERO);
    assert(!status);
    ntg_garbage_add_obj(garbage, &def_rounded_border);
}

void init_ap()
{
    int status;

    struct ntg_float_opts flt_opts = {
        .prim_align = NTG_ALIGN_2,
        .sec_align = NTG_ALIGN_2,
        .enable = NTG_FLOAT_ENABLE_ALW
    };

    status = ntg_float_init(&flt_ap, &flt_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &flt_ap);

    struct ntg_sidefloat_opts sflt_opts = {
        .align = NTG_ALIGN_2,
        .side = NTG_DIR_S,
        .side_shrink = {3, 3},
        .size_cap = NTG_SIDEFLOAT_SZCAP_ANCH,
        .thresh = NTG_SIDEFLOAT_THRESH_MIN
    };

    status = ntg_sidefloat_init(&sflt_ap, &sflt_opts);
    assert(!status);
    ntg_garbage_add_obj(garbage, &sflt_ap);
}
