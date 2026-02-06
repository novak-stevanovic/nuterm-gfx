#include "ntg.h"
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

// bool scene_on_key_fn(ntg_scene* scene, struct nt_key_event key)
// {
//     if(nt_key_event_utf32_check(key, 'd', true))
//     {
//         ntg_loop_break(scene->_stage->_loop, NTG_LOOP_STOP_CLEAN);
//         return true;
//     }
// 
//     return false;
// }

void gui_fn1(void* _)
{
    ntg_cleanup_batch* batch = ntg_cleanup_batch_new();

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
    ntg_label_set_opts(&north, north_label_opts);

    struct ntg_padding_opts north_padding_opts = {
        .pref_size = ntg_insets(2, 2, 2, 2)
    };
    ntg_object_set_padding_opts(ntg_obj(&north), north_padding_opts);

    // CENTER 

    ntg_box center;
    ntg_box_init(&center);
    ntg_cleanup_batch_add(batch, &center, ntg_box_deinit_, NULL);
    ntg_object_set_user_min_size_cont(ntg_obj(&center), ntg_xy(NTG_OBJECT_MIN_SIZE_UNSET, 15));

    ntg_color_block cb1;
    ntg_color_block_init(&cb1);
    ntg_cleanup_batch_add(batch, &cb1, ntg_color_block_deinit_, NULL);
    ntg_color_block_set_color(&cb1, nt_color_new_auto(200, 0, 40));
    struct ntg_border_opts cb1_border_opts = {
        .enable = NTG_OBJECT_DCR_ENABLE_MIN,
        .style = ntg_border_style_new_single(NT_GFX_DEFAULT),
        .pref_size = ntg_insets(1, 2, 1, 2)
    };
    ntg_object_set_border_opts(ntg_obj(&cb1), cb1_border_opts);

    ntg_color_block cb2;
    ntg_color_block_init(&cb2);
    ntg_cleanup_batch_add(batch, &cb2, ntg_color_block_deinit_, NULL);
    ntg_color_block_set_color(&cb2, nt_color_new_auto(40, 0, 200));

    // FLOATING LABEL

    ntg_label flabel;
    ntg_label_init(&flabel);
    ntg_label_set_text(&flabel, "Floating text example");
    ntg_cleanup_batch_add(batch, &flabel, ntg_label_deinit_, NULL);
    ntg_object_set_z_index(ntg_obj(&flabel), 1);
    struct ntg_label_opts flabel_opts = ntg_label_opts_def();
    flabel_opts.wrap = NTG_LABEL_WRAP_WORD;
    ntg_label_set_opts(&flabel, flabel_opts);
    struct ntg_padding_opts flabel_padding_opts = {
        .pref_size = ntg_insets(2, 2, 2, 2)
    };
    ntg_object_set_padding_opts(ntg_obj(&flabel), flabel_padding_opts);

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

    ntg_scene_attach_root(
            &scene,
            ntg_obj(&root),
            NULL,
            ntg_attach_policy_new_root());

    struct ntg_attach_policy_flt_dt flt_dt = {
        .base = ntg_obj(&cb2),
        .enable = NTG_ATTACH_POLICY_FLT_ENABLE_MIN,
        .shrink = ntg_insets(1, 1, 1, 1),
        .prim_align = NTG_ALIGN_2,
        .sec_align = NTG_ALIGN_2,
    };
    ntg_scene_attach_root(
            &scene,
            ntg_obj(&flabel),
            ntg_obj(&root),
            ntg_attach_policy_new_flt(flt_dt));

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
    ntg_cleanup_batch_add(batch, &root, ntg_color_block_deinit_, NULL);

    struct nt_gfx root_border_style_gfx = {
        .fg = nt_color_new_auto(255, 255, 255),
        .bg = nt_color_new_auto(245, 245, 245),
        .style = NT_STYLE_DEFAULT
    };
    struct ntg_border_opts root_border_opts = {
        .style = ntg_border_style_new_rounded(root_border_style_gfx),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN,
        .pref_size = ntg_insets(1, 1, 1, 1),
    };
    ntg_object_set_border_opts(ntg_obj(&root), root_border_opts);
    ntg_object_set_user_min_size_cont(ntg_obj(&root), ntg_xy(100, 10));

    ntg_scene scene;
    ntg_scene_init(&scene);
    ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_, NULL);

    ntg_stage stage;
    ntg_stage_init(&stage);
    ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_, NULL);

    ntg_loop loop;
    ntg_loop_init(&loop, &stage, NULL, 60, 4, loop_on_event_fn);
    ntg_cleanup_batch_add(batch, &loop, ntg_loop_deinit_, NULL);

    ntg_loop_run(&loop);

    ntg_cleanup_batch_finish(batch);
}

int main(int argc, char *argv[])
{
    ntg_init();

    ntg_launch(gui_fn1, NULL);

    ntg_wait();

    ntg_deinit();

    return 0;
}
