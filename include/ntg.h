#ifndef _NTG_H_
#define _NTG_H_

#include "shared/ntg_status.h"
#include "nt_shared.h"
#include "nt_gfx.h"

typedef void (*ntg_gui_fn)(void*);

void ntg_launch(ntg_gui_fn gui_fn, void* data, ntg_status_t* out_status);

void ntg_loop();

/* This function returns the return value of `gui_func` passed in
 * nui_launch() func. */
void* ntg_destroy();

#endif // _NTG_H_
