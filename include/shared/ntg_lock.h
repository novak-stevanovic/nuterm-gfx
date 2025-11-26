#ifndef _NTG_LOCK_H_
#define _NTG_LOCK_H_

typedef struct ntg_lock ntg_lock;

ntg_lock* ntg_lock_new();
void ntg_lock_destroy(ntg_lock* lock);

void ntg_lock_lock(ntg_lock* lock);
void ntg_lock_unlock(ntg_lock* lock);

#endif // _NTG_LOCK_H_
