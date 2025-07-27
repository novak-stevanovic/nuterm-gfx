#ifndef _NTG_H_
#define _NTG_H_

#include "nt_shared.h"
#include "nt_gfx.h"
#include <sys/types.h>
#include "base/ntg_event.h"

typedef struct ntg_stage ntg_stage;

typedef void (*ntg_gui_fn)(ntg_stage* main_stage, void* data);

void __ntg_init__();

void ntg_launch(ntg_stage* main_stage, ntg_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in nui_launch(). */
void* ntg_wait();

void ntg_destroy();

#define NTG_FRAMERATE_DEFAULT 30
#define NTG_WAIT_TIMEOUT(fps) (1000.0 / (fps)) /* Can be used for custom loop */

void ntg_loop(ntg_stage* main_stage, uint framerate);

struct ntg_xy ntg_get_size();

#endif // _NTG_H_
