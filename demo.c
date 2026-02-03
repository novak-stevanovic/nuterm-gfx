#include "ntg.h"
#include <unistd.h>
#include <assert.h>

const char* lorem = 
"Lorem ipsum dolor sit amet, consew9eiufhewioufhewoihfi0ewfioehwiofhewiohfctetur adipiscing elit. Morbi ullamcorper "
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

    ntg_scene scene;
    ntg_scene_init(&scene);
    // ntg_scene_set_on_key_fn(&scene, scene_on_key_fn);

    ntg_stage stage;
    ntg_stage_init(&stage);

    ntg_def_renderer renderer;
    ntg_def_renderer_init(&renderer);

    ntg_scene_layer layer;
    ntg_scene_layer_init(&layer);

    ntg_label_set_text(&label, lorem, strlen(lorem));
    struct nt_gfx label_gfx = {
        .fg = nt_color_new_auto(nt_rgb_new(255, 255, 255)),
        .bg = nt_color_new_auto(nt_rgb_new(143, 0, 255)),
        .style = nt_style_new_uniform(NT_STYLE_VAL_BOLD)
    };
    struct ntg_label_opts opts = {
        .orient = NTG_ORIENT_H,
        .gfx = label_gfx,
        .palign = NTG_LABEL_ALIGN_JUSTIFY,
        .salign = NTG_ALIGN_1,
        .wrap = NTG_LABEL_WRAP_WORD,
        .autotrim = true,
        .indent = 2
    };
    ntg_label_set_opts(&label, opts);

    struct ntg_border_style label_border_style = ntg_def_border_style_new(
            ntg_def_border_style_single(NT_GFX_DEFAULT));
    struct ntg_border_opts label_border_opts = {
        .style = label_border_style,
        .enable = NTG_OBJECT_DCR_ENABLE_MIN,
        .pref_size = ntg_insets(1, 1, 1, 1)
    };
    ntg_object_set_border_opts(ntg_obj(&label), label_border_opts);

    ntg_main_panel main_panel;
    ntg_main_panel_init(&main_panel);

    ntg_color_block west;
    ntg_color_block_init(&west);
    ntg_color_block_set_color(&west, nt_color_new_auto(nt_rgb_new(200, 0, 40)));

    ntg_color_block east;
    ntg_color_block_init(&east);
    ntg_color_block_set_color(&east, nt_color_new_auto(nt_rgb_new(40, 0, 200)));

    ntg_object_set_user_min_size(ntg_obj(&west), ntg_xy(5, NTG_OBJECT_MIN_SIZE_UNSET));
    ntg_object_set_user_min_size(ntg_obj(&east), ntg_xy(5, NTG_OBJECT_MIN_SIZE_UNSET));

    ntg_box center;
    ntg_box_init(&center);

    ntg_main_panel_set(&main_panel, ntg_obj(&label), NTG_MAIN_PANEL_NORTH);
    ntg_main_panel_set(&main_panel, ntg_obj(&west), NTG_MAIN_PANEL_WEST);
    ntg_main_panel_set(&main_panel, ntg_obj(&east), NTG_MAIN_PANEL_EAST);

    ntg_stage_set_scene(&stage, &scene);
    ntg_scene_layer_set_root(&layer, ntg_obj(&main_panel));
    ntg_scene_attach_layer(&scene, &layer, NULL, NTG_SCENE_ATTACH_POLICY_ROOT);

    ntg_loop loop;
    ntg_loop_init(&loop, &stage, ntg_rnd(&renderer), 60, 4, loop_on_event_fn);

    ntg_cleanup_batch_add_many(batch,
        NTG_CLEANUP_PAIR(&loop, ntg_loop_deinit_),
        NTG_CLEANUP_PAIR(&renderer, ntg_def_renderer_deinit_),
        NTG_CLEANUP_PAIR(&stage, ntg_stage_deinit_),
        NTG_CLEANUP_PAIR(&scene, ntg_scene_deinit_),
        NTG_CLEANUP_PAIR(&layer, ntg_scene_layer_deinit_),
        NTG_CLEANUP_PAIR(&center, ntg_box_deinit_),
        NTG_CLEANUP_PAIR(&label, ntg_label_deinit_),
        NTG_CLEANUP_PAIR(&west, ntg_color_block_deinit_),
        NTG_CLEANUP_PAIR(&east, ntg_color_block_deinit_),
        NTG_CLEANUP_PAIR(&main_panel, ntg_main_panel_deinit_),
    );

    ntg_log_log("LOOP END | STATUS: %d", ntg_loop_run(&loop));

    ntg_cleanup_batch_destroy(batch);
}

int main(int argc, char *argv[])
{
    ntg_init();

    ntg_launch(gui_fn1, NULL);

    ntg_wait();

    ntg_deinit();

    return 0;
}
