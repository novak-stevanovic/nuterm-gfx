#include "ntg.h"
#include "nt.h"
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

#define UCONV_IMPLEMENTATION
#include "thirdparty/uconv.h"

#define SARENA_IMPLEMENTATION
#include "thirdparty/sarena.h"


/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* DATA */
/* ========================================================================== */

static bool _enabled = false;
static struct ntg_opts _opts = {0};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

struct ntg_opts ntg_opts_default(void)
{
    return (struct ntg_opts) {
        .alt_screen_mode = NTG_ALT_SCREEN_ENABLE,
        // .cursor_mode = NTG_CURSOR_HIDE,
        .unsupported_term_mode = NTG_UNSUPPORTED_TERM_IGNORE
    };
}

int ntg_enable(
        const struct ntg_opts* opts,
        const char* log_filepath)
{
    int _status;

    _opts = (opts ? (*opts) : ntg_opts_default());

    if(log_filepath != NULL)
    {
        _status = ntg_log_init(log_filepath);
        if(_status)
            return _status;
    }

    _status = nt_init();

    switch(_status)
    {
        case 0:
            break;
        case NT_ERR_TERM_NOT_SUPP:
            if(_opts.unsupported_term_mode == NTG_UNSUPPORTED_TERM_IGNORE)
                break;

            ntg_log_deinit();
            return NTG_ERR_UNSUPP_TERM;
        default:
            ntg_log_deinit();
            return NTG_ERR_UNEXPECTED;
    }

    int alt_screen_err = 0;

    // if(_opts.cursor_mode == NTG_CURSOR_HIDE)
    nt_cursor_hide();

    if(_opts.alt_screen_mode == NTG_ALT_SCREEN_ENABLE)
        alt_screen_err = nt_alt_screen_enable();
    if(alt_screen_err)
        _opts.alt_screen_mode = NTG_ALT_SCREEN_DISABLE;

    _enabled = true;
    return 0;
}

const struct ntg_opts* ntg_get_opts(void)
{
    return (_enabled ? &_opts : NULL);
}

void ntg_disable(void)
{
    if(!_enabled) return;

    // if(_opts.cursor_mode == NTG_CURSOR_HIDE)
    nt_cursor_show();

    if(_opts.alt_screen_mode == NTG_ALT_SCREEN_ENABLE)
        nt_alt_screen_disable();
    else
    {
        nt_erase_screen();
        nt_erase_scrollback();
        nt_cursor_move(0, 0);
    }

    nt_deinit();

    ntg_log_deinit();

    _enabled = false;

}
