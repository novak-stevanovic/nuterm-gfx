#ifndef _NTG_H_
#define _NTG_H_

#include "shared/ntg_status.h"
#include "nt_shared.h"
#include "nt_gfx.h"
#include <sys/types.h>

typedef void (*ntg_gui_fn)(void*);

void ntg_initialize(ntg_gui_fn gui_fn, void* data);

void ntg_launch();

#define NTG_FRAMERATE_DEFAULT 60
#define NTG_WAIT_TIMEOUT(fps) (1000.0 / (fps)) /* Can be used for custom loop */

void ntg_loop(uint framerate);
void ntg_loop_break();

/* This function returns the return value of `gui_fn` passed in
 * nui_launch(). */
void* ntg_destroy();

#endif // _NTG_H_
