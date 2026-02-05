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

    ntg_label label;
    ntg_label_init(&label);
    ntg_cleanup_batch_add(batch, &label, ntg_label_deinit_, NULL);

    ntg_label_set_text(&label, lorem, strlen(lorem));
    struct nt_gfx label_gfx = {
        .fg = nt_color_new_auto(255, 255, 255),
        .bg = nt_color_new_auto(143, 0, 255),
        .style = nt_style_new_uniform(NT_STYLE_VAL_BOLD)
    };
    struct ntg_label_opts opts = {
        .orient = NTG_ORIENT_H,
        .gfx = label_gfx,
        .mode = NTG_LABEL_ALIGN,
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1,
        .wrap = NTG_LABEL_WRAP_WORD,
        .autotrim = true,
        .indent = 2
    };
    ntg_label_set_opts(&label, opts);

    struct ntg_border_opts label_border_opts = {
        .style = ntg_border_style_new_single(NT_GFX_DEFAULT),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN,
        .pref_size = ntg_insets(1, 1, 1, 1)
    };
    struct ntg_padding_opts label_padding_opts = {
        .enable = NTG_OBJECT_DCR_ENABLE_MIN,
        .pref_size = ntg_insets(2, 2, 2, 2)
    };
    ntg_object_set_border_opts(ntg_obj(&label), label_border_opts);
    ntg_object_set_padding_opts(ntg_obj(&label), label_padding_opts);

    ntg_scene scene;
    ntg_scene_init(&scene);
    ntg_cleanup_batch_add(batch, &scene, ntg_scene_deinit_, NULL);
    // ntg_scene_set_on_key_fn(&scene, scene_on_key_fn);

    ntg_stage stage;
    ntg_stage_init(&stage);
    ntg_cleanup_batch_add(batch, &stage, ntg_stage_deinit_, NULL);

    ntg_def_renderer renderer;
    ntg_def_renderer_init(&renderer);
    ntg_cleanup_batch_add(batch, &renderer, ntg_def_renderer_deinit_, NULL);

    ntg_main_panel main_panel;
    ntg_main_panel_init(&main_panel);
    ntg_cleanup_batch_add(batch, &main_panel, ntg_main_panel_deinit_, NULL);
    struct ntg_border_opts root_border_opts = {
        .style = ntg_border_style_new_rounded(NT_GFX_DEFAULT),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN,
        .pref_size = ntg_insets(1, 1, 1, 1)
    };
    ntg_object_set_border_opts(ntg_obj(&main_panel), root_border_opts);

    ntg_color_block west;
    ntg_color_block_init(&west);
    ntg_cleanup_batch_add(batch, &west, ntg_color_block_deinit_, NULL);
    ntg_color_block_set_color(&west, nt_color_new_auto(200, 0, 40));

    ntg_color_block east;
    ntg_color_block_init(&east);
    ntg_cleanup_batch_add(batch, &east, ntg_color_block_deinit_, NULL);
    ntg_color_block_set_color(&east, nt_color_new_auto(40, 0, 200));
    ntg_object_set_user_min_size_cont(ntg_obj(&west), ntg_xy(10, NTG_OBJECT_MIN_SIZE_UNSET));
    ntg_object_set_user_min_size_cont(ntg_obj(&east), ntg_xy(10, NTG_OBJECT_MIN_SIZE_UNSET));

    ntg_box center;
    ntg_box_init(&center);
    ntg_cleanup_batch_add(batch, &center, ntg_box_deinit_, NULL);

    ntg_main_panel_set(&main_panel, ntg_obj(&label), NTG_MAIN_PANEL_NORTH);
    ntg_main_panel_set(&main_panel, ntg_obj(&west), NTG_MAIN_PANEL_WEST);
    ntg_main_panel_set(&main_panel, ntg_obj(&east), NTG_MAIN_PANEL_EAST);

    ntg_scene_attach_root(&scene, ntg_obj(&main_panel), NULL, ntg_attach_policy_root());
    ntg_stage_set_scene(&stage, &scene);

    ntg_loop loop;
    ntg_loop_init(&loop, &stage, ntg_rnd(&renderer), 60, 4, loop_on_event_fn);
    ntg_cleanup_batch_add(batch, &loop, ntg_loop_deinit_, NULL);

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

    ntg_def_renderer renderer;
    ntg_def_renderer_init(&renderer);
    ntg_cleanup_batch_add(batch, &renderer, ntg_def_renderer_deinit_, NULL);

    ntg_loop loop;
    ntg_loop_init(&loop, &stage, ntg_rnd(&renderer), 60, 4, loop_on_event_fn);
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
