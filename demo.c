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

struct ctx_data
{
    ntg_label* label;
    ntg_def_border* label_border;
    ntg_def_scene* scene;
    ntg_border_box* border_box;
};

bool loop_process_fn(ntg_loop* loop, struct nt_event event, ntg_loop_ctx* ctx)
{
    struct ctx_data ctx_data = *(struct ctx_data*)ctx->data;
    ntg_label* label = ctx_data.label;
    ntg_def_border* label_border = ctx_data.label_border;
    ntg_def_scene* scene = ctx_data.scene;
    ntg_border_box* border_box = ctx_data.border_box;

    if(event.type == NT_EVENT_KEY)
    {
        struct nt_key_event key = *(struct nt_key_event*)event.data;
        if(nt_key_event_utf32_check(key, 'q', false))
        {
            ntg_loop_ctx_break(ctx, false);
            return true;
        }
        if(nt_key_event_utf32_check(key, '1', false))
        {
            ntg_widget_set_border(ntg_wdg(label), ntg_dcr(label_border));
            return true;
        }
        if(nt_key_event_utf32_check(key, '2', false))
        {
            ntg_widget_set_border(ntg_wdg(label), NULL);
            return true;
        }
        if(nt_key_event_utf32_check(key, '3', false))
        {
            ntg_scene_set_root(ntg_scn(scene), ntg_wdg(border_box));
            return true;
        }
        if(nt_key_event_utf32_check(key, '4', false))
        {
            ntg_scene_set_root(ntg_scn(scene), NULL);
            return true;
        }
        if(nt_key_event_utf32_check(key, '5', false))
        {
            ntg_entity_destroy(ntg_ent(scene));
            return true;
        }
        if(nt_key_event_utf32_check(key, '6', false))
        {
            ntg_entity_destroy(ntg_ent(label));
            return true;
        }
        if(nt_key_event_utf32_check(key, '7', false))
        {
            ntg_entity_destroy(ntg_ent(label_border));
            return true;
        }
        if(nt_key_event_utf32_check(key, '8', false))
        {
            ntg_entity_destroy(ntg_ent(border_box));
            return true;
        }
    }

    return ntg_loop_dispatch_def(loop, event, ctx);
}

void gui_fn1(ntg_entity_system* es, ntg_loop* loop, void* _)
{
    ntg_label* label = ntg_label_new(es);
    ntg_label_init(label);

    ntg_def_scene* scene = ntg_def_scene_new(es);
    ntg_def_scene_init(scene);

    ntg_def_stage* stage = ntg_def_stage_new(es);
    ntg_def_stage_init(stage);

    ntg_def_renderer* renderer = ntg_def_renderer_new(es);
    ntg_def_renderer_init(renderer);

    ntg_label_set_text(label, ntg_strv_cstr(lorem));
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
    ntg_label_set_opts(label, opts);

    ntg_def_padding* label_padding = ntg_def_padding_new(es);
    ntg_def_padding_init(label_padding);
    ntg_widget_set_padding(ntg_wdg(label), ntg_dcr(label_padding));

    ntg_def_border* label_border = ntg_def_border_new(es);
    ntg_def_border_init(label_border);
    struct nt_gfx border_gfx = {
        .bg = nt_color_new_auto(nt_rgb_new(143, 0, 255)),
        .fg = nt_color_new_auto(nt_rgb_new(200, 200, 200)),
        .style = NT_STYLE_DEFAULT
    };
    ntg_def_border_set_style(label_border, ntg_def_border_style_single(border_gfx));

    ntg_border_box* border_box = ntg_border_box_new(es);
    ntg_border_box_init(border_box);

    ntg_color_block* west = ntg_color_block_new(es);
    ntg_color_block_init(west);
    ntg_color_block_set_color(west, nt_color_new_auto(nt_rgb_new(200, 0, 40)));

    ntg_color_block* east = ntg_color_block_new(es);
    ntg_color_block_init(east);
    ntg_color_block_set_color(east, nt_color_new_auto(nt_rgb_new(40, 0, 200)));

    ntg_widget_set_user_min_size(ntg_wdg(west), ntg_xy(5, NTG_WIDGET_MIN_SIZE_UNSET));
    ntg_widget_set_user_min_size(ntg_wdg(east), ntg_xy(5, NTG_WIDGET_MIN_SIZE_UNSET));

    ntg_box* center = ntg_box_new(es);
    ntg_box_init(center);

    ntg_border_box_set(border_box, ntg_wdg(label), NTG_BORDER_BOX_NORTH);
    ntg_border_box_set(border_box, ntg_wdg(west), NTG_BORDER_BOX_WEST);
    ntg_border_box_set(border_box, ntg_wdg(east), NTG_BORDER_BOX_EAST);

    ntg_stage_set_scene(ntg_stg(stage), ntg_scn(scene));
    ntg_scene_set_root(ntg_scn(scene), ntg_wdg(border_box));

    struct ctx_data ctx_data = {
        .label = label,
        .label_border = label_border,
        .scene = scene,
        .border_box = border_box
    };

    struct ntg_loop_run_data loop_data = {
        .process_fn = loop_process_fn,
        .stage = ntg_stg(stage),
        .renderer = ntg_rdr(renderer),
        .framerate = 60,
        .workers = 4,
        .ctx_data = &ctx_data
    };

    ntg_loop_run(loop, loop_data);

}

int main(int argc, char *argv[])
{
    ntg_init();

    ntg_launch(gui_fn1, NULL);

    ntg_wait();

    ntg_deinit();

    return 0;
}
