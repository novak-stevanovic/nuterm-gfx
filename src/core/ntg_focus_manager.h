#ifndef _NTG_FOCUS_MANAGER_H_
#define _NTG_FOCUS_MANAGER_H_

typedef struct ntg_object ntg_object;

void __ntg_focus_manager_init__();
void __ntg_focus_manager_deinit__();

ntg_object* ntg_focus_manager_get_focused();
void ntg_focus_manager_set_focused(ntg_object* object);

#endif // _NTG_FOCUS_MANAGER_H_
