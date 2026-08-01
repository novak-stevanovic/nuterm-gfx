#include "ntg.h"
#include "nt.h"
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "core/loop/ntg_loop_internal.h"
#include "shared/ntg_shared_internal.h"

#define UCONV_IMPLEMENTATION
#include "thirdparty/uconv.h"

#define SARENA_IMPLEMENTATION
#include "thirdparty/sarena.h"

#define GENC_IMPLEMENTATION
#include "thirdparty/genc.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* OPTIONS */
/* ------------------------------------------------------ */

struct ntg_opts ntg_opts_default(void)
{
    return (struct ntg_opts) {
        .mouse_mode = NTG_MOUSE_ENABLE,
        .alt_screen_mode = NTG_ALT_SCREEN_ENABLE,
        .cursor_mode = NTG_CURSOR_HIDE,
        .unsupported_term_mode = NTG_UNSUPPORTED_TERM_IGNORE
    };
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* DATA */
/* -------------------------------------------------------------------------- */

static bool _enabled = false;
static struct ntg_opts _opts = {0};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* ENABLE/DISABLE */
/* ------------------------------------------------------ */

void ntg_enable(
        const struct ntg_opts* opts,
        const char* log_filepath,
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    _opts = (opts ? (*opts) : ntg_opts_default());

    if(log_filepath != NULL)
    {
        ntg_log_init(log_filepath, &_status);
        if(_status)
            ntg_vreturn(out_status, _status);
    }

    nt_init(&_status);

    switch(_status)
    {
        case 0:
            break;
        case NT_ERR_TERM_NOT_SUPP:
            if(_opts.unsupported_term_mode == NTG_UNSUPPORTED_TERM_IGNORE)
                break;

            ntg_log_deinit();
            ntg_vreturn(out_status, NTG_ERR_UNSUPP_TERM);
        default:
            ntg_log_deinit();
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    int alt_screen_err = 0;

    if(_opts.mouse_mode == NTG_MOUSE_ENABLE)
        nt_mouse_mode_enable(NULL);

    if(_opts.cursor_mode == NTG_CURSOR_HIDE)
        nt_cursor_hide(NULL);

    if(_opts.alt_screen_mode == NTG_ALT_SCREEN_ENABLE)
        nt_alt_screen_enable(&alt_screen_err);
    if(alt_screen_err)
        _opts.alt_screen_mode = NTG_ALT_SCREEN_DISABLE;

    _enabled = true;
}

const struct ntg_opts* ntg_get_opts(void)
{
    return (_enabled ? &_opts : NULL);
}

void ntg_disable(void)
{
    if(!_enabled) return;

    if(_opts.mouse_mode == NTG_MOUSE_ENABLE)
        nt_mouse_mode_disable(NULL);

    if(_opts.cursor_mode == NTG_CURSOR_HIDE)
        nt_cursor_show(NULL);

    if(_opts.alt_screen_mode == NTG_ALT_SCREEN_ENABLE)
        nt_alt_screen_disable(NULL);
    else
    {
        nt_erase_screen(NULL);
        nt_erase_scrollback(NULL);
        nt_cursor_move(0, 0, NULL);
    }

    nt_deinit(NULL);

    ntg_log_deinit();

    _enabled = false;
}
