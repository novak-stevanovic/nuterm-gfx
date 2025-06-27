#ifndef _NTG_BR_STAGE_H_
#define _NTG_BR_STAGE_H_

#include "core/ntg_stage.h"
#include "nt_charbuff.h"

typedef struct ntg_br_stage
{
    ntg_stage _base;

    nt_charbuff* __buff;
} ntg_br_stage;

/* To specify default behavior for `ntg_stage_process_key_fn`,
 * look at ntg_stage.h */

void __ntg_br_stage_init__(ntg_br_stage* stage,
        ntg_stage_process_key_fn process_key_fn);
void __ntg_br_stage_deinit__(ntg_br_stage* stage);

ntg_br_stage* ntg_br_stage_new(ntg_stage_process_key_fn process_key_fn);
void ntg_br_stage_destroy(ntg_br_stage* stage);

#endif // _NTG_BR_STAGE_H_
