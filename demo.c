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

bool loop_on_event_fn(ntg_loop* loop, struct nt_event event)
{
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

void gui_fn1(void* _)
{
    ntg_cleanup_batch* batch = ntg_cleanup_batch_new();

    struct ntg_border_style rounded_border_flt;
    ntg_border_style_init_rounded(&rounded_border_flt, NT_GFX_DEFAULT, true);

    // ROOT

    ntg_main_panel root;
    ntg_main_panel_init(&root);
    ntg_cleanup_batch_add(batch, &root, ntg_main_panel_deinit_, NULL);

    // NORTH

    ntg_label north;
    ntg_label_init(&north);
    ntg_cleanup_batch_add(batch, &north, ntg_label_deinit_, NULL);

    ntg_label_set_text(&north, lorem);
    struct nt_gfx label_gfx = {
        .fg = nt_color_new_auto(255, 255, 255),
        .bg = nt_color_new_auto(143, 0, 255),
        .style = nt_style_new_uniform(NT_STYLE_VAL_BOLD)
    };
    struct ntg_label_opts north_label_opts = ntg_label_opts_def();
    north_label_opts.gfx = label_gfx;
    north_label_opts.indent = 2;
    north_label_opts.wrap = NTG_LABEL_WRAP_WORD;
    ntg_label_set_opts(&north, &north_label_opts);

    struct ntg_padding_opts north_pad_opts = ntg_padding_opts_def();
    north_pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    //ntg_object_set_padding_opts(ntg_obj(&north), &north_pad_opts);

    // CENTER 

    ntg_box center;
    ntg_box_init(&center);
    ntg_cleanup_batch_add(batch, &center, ntg_box_deinit_, NULL);
    ntg_object_set_user_min_size_cont(ntg_obj(&center), ntg_xy(15, 15));
    //15ntg_object_set_def_bg(ntg_obj(&center), ntg_vcell_bg(nt_color_new_auto(0, 25, 100)));

    ntg_color_block cb1;
    ntg_color_block_init(&cb1);
    ntg_cleanup_batch_add(batch, &cb1, ntg_color_block_deinit_, NULL);
    ntg_color_block_set_color(&cb1, nt_color_new_auto(200, 0, 40));

    ntg_color_block cb2;
    ntg_color_block_init(&cb2);
    ntg_cleanup_batch_add(batch, &cb2, ntg_color_block_deinit_, NULL);
    ntg_color_block_set_color(&cb2, nt_color_new_auto(40, 0, 200));

    // FLOATING LABEL

    ntg_label flt_label;
    ntg_label_init(&flt_label);
    ntg_label_set_text(&flt_label, "Floating label example");
    ntg_cleanup_batch_add(batch, &flt_label, ntg_label_deinit_, NULL);
    ntg_object_set_z_index(ntg_obj(&flt_label), 1);
    struct ntg_label_opts flt_label_opts = ntg_label_opts_def();
    flt_label_opts.wrap = NTG_LABEL_WRAP_WORD;
    flt_label_opts.bg_mode = NTG_LABEL_BG_FLT;
    ntg_label_set_opts(&flt_label, &flt_label_opts);
    struct ntg_padding_opts flt_label_pad_opts = ntg_padding_opts_def();
    flt_label_pad_opts.pref_size = ntg_insets(2, 2, 2, 2);
    ntg_object_set_padding_opts(ntg_obj(&flt_label), &flt_label_pad_opts);
    struct ntg_border_opts flt_label_border_opts = ntg_border_opts_def();
    flt_label_border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    flt_label_border_opts.style = &rounded_border_flt;
    ntg_object_set_border_opts(ntg_obj(&flt_label), &flt_label_border_opts);

    // SCENE

    ntg_scene scene;
    ntg_scene_init(&scene);
    ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_, NULL);
    // ntg_scene_set_on_key_fn(&scene, scene_on_key_fn);

    // STAGE

    ntg_stage stage;
    ntg_stage_init(&stage);
    ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_, NULL);

    // LOOP

    ntg_loop loop;
    ntg_loop_init(&loop, &stage, NULL, 60, 4, loop_on_event_fn);
    ntg_cleanup_batch_add(batch, &loop, ntg_loop_deinit_, NULL);

    // ATTACH

    ntg_box_add_child(&center, ntg_obj(&cb1));
    ntg_box_add_child(&center, ntg_obj(&cb2));

    ntg_main_panel_set(&root, ntg_obj(&north), NTG_MAIN_PANEL_NORTH);
    ntg_main_panel_set(&root, ntg_obj(&center), NTG_MAIN_PANEL_CENTER);

    ntg_stage_set_scene(&stage, &scene);

    ntg_scene_attach_root(&scene, ntg_obj(&root), NULL, NULL);

   struct ntg_attach_policy_flt_opts flt_ap_opts = {
       .enable = NTG_ATTACH_POLICY_FLT_ENABLE_MIN,
       .shrink = ntg_insets(1, 1, 1, 1),
       .prim_align = NTG_ALIGN_2,
       .sec_align = NTG_ALIGN_2,
   };
   struct ntg_attach_policy flt_ap;
   ntg_attach_policy_init_flt(&flt_ap, ntg_obj(&north), &flt_ap_opts);
   ntg_scene_attach_root(&scene, ntg_obj(&flt_label), ntg_obj(&root), &flt_ap);

    ntg_loop_exit_status loop_status = ntg_loop_run(&loop);
    ntg_log_log("LOOP END | STATUS: %d", loop_status);

    ntg_cleanup_batch_finish(batch);
}

void gui_fn2(void* _)
{
    ntg_cleanup_batch* batch = ntg_cleanup_batch_new();

    ntg_color_block root;
    ntg_color_block_init(&root);
    ntg_color_block_set_color(&root, nt_color_new_auto(255, 0, 0));
    struct ntg_border_opts border_opts = ntg_border_opts_def();
    border_opts.pref_size = ntg_insets(1, 1, 1, 1);
    struct ntg_border_style style;
    ntg_border_style_init_monochrome(&style, nt_color_new_auto(255, 255, 0));
    border_opts.style = &style;
    ntg_object_set_border_opts(ntg_obj(&root), &border_opts);
    ntg_cleanup_batch_add(batch, &root, ntg_color_block_deinit_, NULL);

    // ntg_object_set_user_min_size_cont(ntg_obj(&root), ntg_xy(100, 10));

    ntg_scene scene;
    ntg_scene_init(&scene);
    ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_, NULL);

    ntg_stage stage;
    ntg_stage_init(&stage);
    ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_, NULL);

    ntg_loop loop;
    ntg_loop_init(&loop, &stage, NULL, 60, 4, loop_on_event_fn);
    ntg_cleanup_batch_add(batch, &loop, ntg_loop_deinit_, NULL);

    ntg_scene_attach_root(&scene, ntg_obj(&root), NULL, NULL);
    ntg_stage_set_scene(&stage, &scene);

    ntg_loop_run(&loop);

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
