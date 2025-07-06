#ifndef _NTG_SIMPLE_STAGE_H_
#define _NTG_SIMPLE_STAGE_H_

#include "core/ntg_stage.h"
#include "nt_charbuff.h"

typedef struct ntg_simple_stage
{
    ntg_stage __base;

    nt_charbuff* __buff;
} ntg_simple_stage;

/* To specify default behavior for `ntg_stage_process_key_fn`,
 * look at ntg_stage.h */

void __ntg_simple_stage_init__(ntg_simple_stage* stage,
        ntg_stage_process_key_fn process_key_fn);
void __ntg_simple_stage_deinit__(ntg_simple_stage* stage);

ntg_simple_stage* ntg_simple_stage_new(ntg_stage_process_key_fn process_key_fn);
void ntg_simple_stage_destroy(ntg_simple_stage* stage);

#endif // _NTG_SIMPLE_STAGE_H_
