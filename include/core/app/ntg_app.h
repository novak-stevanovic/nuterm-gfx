#ifndef _NTG_APP_H_
#define _NTG_APP_H_

#include <sys/types.h>
#include "nt_event.h"

typedef struct ntg_scene ntg_scene;
typedef struct ntg_app_renderer ntg_app_renderer;

/* -------------------------------------------------------------------------- */

void __ntg_app_init__();
void __ntg_app_deinit__();

/* -------------------------------------------------------------------------- */

#define NTG_APP_FRAMERATE_DEFAULT 30
#define NTG_APP_WAIT_TIMEOUT(fps) (1000.0 / (fps)) /* Can be used for custom loop */

typedef void (*ntg_app_gui_fn)(void* data);

void ntg_app_launch(ntg_app_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in nui_launch(). */
void* ntg_app_wait();

/* ------------------------------------------------------ */

/* Default loop - used in `ntg_app_gui_fn`.
 * For custom loop, use `nt_wait_for_event()`. */ 

typedef enum ntg_app_status
{
    NTG_APP_CONTINUE,
    NTG_APP_QUIT
} ntg_app_status;

struct ntg_app_loop_context
{
    ntg_scene* scene;
};

typedef ntg_app_status (*ntg_app_process_key_fn)(
        struct nt_key_event key_event,
        struct ntg_app_loop_context* context,
        void* data);

void ntg_app_loop(
        ntg_scene* scene,
        uint framerate,
        ntg_app_renderer* renderer,
        ntg_app_process_key_fn process_key_fn,
        void* data);

/* -------------------------------------------------------------------------- */

#endif // _NTG_APP_H_
