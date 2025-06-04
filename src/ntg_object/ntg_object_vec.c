#include "ntg_object/ntg_object_vec.h"
#include "ntg_object/def/ntg_object_def.h"
#include <stdlib.h>
#include <string.h>

#define INIT_CAPACITY 5
#define RESIZE_FACTOR 2

struct ntg_object_vec
{
    ntg_object_t** data;
    size_t count;
    size_t capacity;
};

ntg_object_vec_t* ntg_object_vec_new()
{
    ntg_object_vec_t* new = (ntg_object_vec_t*)malloc
        (sizeof(struct ntg_object_vec));

    if(new == NULL) return NULL;

    new->data = (ntg_object_t**)malloc(INIT_CAPACITY * sizeof(ntg_object_t*));

    if(new->data == NULL)
    {
        free(new);
        return NULL;
    }

    new->count = 0;
    new->capacity = INIT_CAPACITY;

    return new;
}

void ntg_object_vec_destroy(ntg_object_vec_t* vec)
{
    if(vec == NULL) return;

    free(vec->data);
    free(vec);
}

void ntg_object_vec_append(ntg_object_vec_t* vec, ntg_object_t* obj)
{
    if(vec == NULL) return;
    if(obj == NULL) return;

    if(vec->count == vec->capacity)
    {
        size_t new_capacity = vec->count * RESIZE_FACTOR;
        ntg_object_t** new_data = realloc(vec->data, new_capacity);
        if(new_data == NULL) return;

        vec->data = new_data;
        vec->capacity = new_capacity;
    }

    vec->data[vec->count] = obj;
    vec->count++;
}

void ntg_object_vec_remove(ntg_object_vec_t* vec, ntg_object_t* obj)
{
    size_t i;
    for(i = 0; i < vec->count; i++)
    {
        if(vec->data[i] == obj)
        {
            memmove(vec->data + 1, vec->data, vec->count - i - 1);
            vec->count--;
        }
    }
}

size_t ntg_object_vec_size(const ntg_object_vec_t* vec)
{
    return vec->count;
}

ntg_object_t* ntg_object_vec_at(const ntg_object_vec_t* vec, size_t pos)
{
    if(vec == NULL) return NULL;
    if(pos >= vec->count) return NULL;

    return vec->data[pos];
}
