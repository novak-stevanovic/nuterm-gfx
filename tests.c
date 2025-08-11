#include <stdio.h>
#include <unistd.h>
#include "base/ntg_event.h"
#include "ntg.h"

#include "core/ntg_simple_stage.h"
#include "core/ntg_stage.h"
#include "core/ntg_scene.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"
#include "base/ntg_event_types.h"
#include "base/ntg_event_participants.h"

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_simple_stage* main_stage = ntg_simple_stage_new();

    ntg_launch(NTG_STAGE(main_stage), NULL, NULL);

    ntg_wait();

    ntg_simple_stage_destroy(main_stage);

    ntg_destroy();

    return 0;
}
