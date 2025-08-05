#ifndef _NTG_CONSTRAINTS_OBJECT_H_
#define _NTG_CONSTRAINTS_OBJECT_H_

#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object;

struct ntg_object_constraints
{
    ntg_object* object;
    struct ntg_constr constr;
};

typedef struct ntg_constraints_object
{
    struct ntg_object_constraints* __data;
} ntg_constraints_object;

void __ntg_constraints_object_init__(ntg_constraints_object* constr_object, ntg_object* parent);
void __ntg_constraints_object_deinit__(ntg_constraints_object* constr_object);

void ntg_constraints_object_set(ntg_constraints_object* constr_object,
        ntg_object* child, struct ntg_constr constr);

#endif // _NTG_CONSTRAINTS_OBJECT_H_
