/*
 * MIT License
 *
 * Copyright (c) 2025 Novak Stevanović
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* DEFINE */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#ifndef GENC_H
#define GENC_H

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#error "C99 or newer is required"
#endif /* C99 check */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef GENC_ERR_BASE
#define GENC_ERR_BASE 1000
#endif // GENC_ERR_BASE

#define GENC_ERR_INV_ARG (GENC_ERR_BASE + 1)
#define GENC_ERR_ALLOC_FAIL (GENC_ERR_BASE + 2)
#define GENC_ERR_OUT_OF_BOUNDS (GENC_ERR_BASE + 3)
#define GENC_ERR_NO_DATA (GENC_ERR_BASE + 4)
#define GENC_ERR_OVERFLOW (GENC_ERR_BASE + 5)
#define GENC_ERR_UNEXPECTED (GENC_ERR_BASE + 100)

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* VECTOR */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* GENC_VECTOR_DECLARE() and GENC_VECTOR_DEFINE() generate a type-safe dynamic
 * vector API. GENC_VECTOR_INLINE() generates both with `static inline`.
 *
 * The generated structure must be zero-initialized before its first use. */

/* ========================================================================== */
/* VECTOR - PROTOTYPES */
/* ========================================================================== */

/* --------------------------------------------------------|

struct <name>
{
    <type>* data;
    size_t size;
    size_t cap;
};

|----------------------------------------------------------|

* Deinitializes the vector and frees allocated memory.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.

int <name>_deinit(struct <name>* vec);

|----------------------------------------------------------|

* Appends an element to the vector.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.
* GENC_ERR_OVERFLOW: The required capacity cannot be represented.

int <name>_pushb(struct <name>* vec, <type> data);

|----------------------------------------------------------|

* Removes the last element from the vector.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_NO_DATA: The vector is empty.

int <name>_popb(struct <name>* vec);

|----------------------------------------------------------|

* Inserts an element at `pos`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_OUT_OF_BOUNDS: `pos` is greater than the vector size.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.
* GENC_ERR_OVERFLOW: The required capacity cannot be represented.

int <name>_ins(struct <name>* vec, <type> data, size_t pos);

|----------------------------------------------------------|

* Removes the element at `pos`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_OUT_OF_BOUNDS: `pos` is outside the vector.

int <name>_rm_at(struct <name>* vec, size_t pos);

|----------------------------------------------------------|

* Removes all elements while retaining allocated capacity.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.

int <name>_empty(struct <name>* vec);

|----------------------------------------------------------|

* Shrinks allocated capacity to the current vector size.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.
* GENC_ERR_OVERFLOW: The vector size in bytes cannot be represented.

int <name>_fit(struct <name>* vec);

|----------------------------------------------------------|

* Increases vector capacity by `size` elements.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.
* GENC_ERR_OVERFLOW: The requested capacity cannot be represented.

int <name>_prealloc(struct <name>* vec, size_t size);

|-------------------------------------------------------- */

/* ========================================================================== */
/* VECTOR - GENERATOR MACROS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* VECTOR - DECLARE */
/* -------------------------------------------------------------------------- */

#define GENC_VECTOR_DECLARE(NAME, TYPE, GROWF, FN_PREFIX)                      \
                                                                               \
struct NAME                                                                    \
{                                                                              \
    TYPE * data;                                                               \
    size_t size;                                                               \
    size_t cap;                                                                \
};                                                                             \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_deinit(struct NAME * v);                                                \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_ins(struct NAME * v, TYPE data, size_t pos);                            \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_rm_at(struct NAME * v, size_t pos);                                     \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_empty(struct NAME * v);                                                 \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_fit(struct NAME * v);                                                   \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_prealloc(struct NAME * v, size_t size);                                 \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popb(struct NAME * v);                                                  \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushb(struct NAME * v, TYPE data);                                      \

/* -------------------------------------------------------------------------- */
/* VECTOR - DEFINE */
/* -------------------------------------------------------------------------- */

#define GENC_VECTOR_DEFINE(NAME, TYPE, GROWF, FN_PREFIX)                       \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_deinit(struct NAME * v)                                                 \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    free(v->data);                                                             \
    v->data = NULL;                                                            \
    v->size = 0;                                                               \
    v->cap = 0;                                                                \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_ins(struct NAME * v, TYPE data, size_t pos)                             \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
    if(pos > v->size) return GENC_ERR_OUT_OF_BOUNDS;                           \
                                                                               \
    if(v->size >= v->cap) /* grow */                                           \
    {                                                                          \
        /* Calculate new_cap */                                                \
        size_t new_cap = v->size * GROWF;                                      \
        if(new_cap <= v->cap)                                                  \
        {                                                                      \
            if(v->cap == SIZE_MAX) return GENC_ERR_OVERFLOW;                   \
            new_cap = v->cap + 1;                                              \
        }                                                                      \
                                                                               \
        if(new_cap > (SIZE_MAX / sizeof(TYPE)))                                \
            return GENC_ERR_OVERFLOW;                                          \
                                                                               \
        void* new_data = realloc(v->data, new_cap * sizeof(TYPE));             \
        if(new_data == NULL)                                                   \
            return GENC_ERR_ALLOC_FAIL;                                        \
                                                                               \
        v->data = new_data;                                                    \
        v->cap = new_cap;                                                      \
    }                                                                          \
                                                                               \
    char* vector_data = (char*)v->data;                                        \
                                                                               \
    if(pos < v->size) /* make space */                                         \
    {                                                                          \
        memmove(vector_data + ((pos + 1) * sizeof(TYPE)),                      \
                vector_data + (pos * sizeof(TYPE)),                            \
                (v->size - pos) * sizeof(TYPE));                               \
    }                                                                          \
                                                                               \
    v->data[pos] = data;                                                       \
    ++(v->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_rm_at(struct NAME * v, size_t pos)                                      \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
    if(pos >= v->size) return GENC_ERR_OUT_OF_BOUNDS;                          \
                                                                               \
    if(pos == (v->size - 1))                                                   \
        return NAME##_popb(v);                                                 \
                                                                               \
    char* vector_data = (char*)v->data;                                        \
                                                                               \
    memmove(vector_data + (pos * sizeof(TYPE)),                                \
            vector_data + ((pos + 1) * sizeof(TYPE)),                          \
            (v->size - pos - 1) * sizeof(TYPE));                               \
                                                                               \
    --(v->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_empty(struct NAME * v)                                                  \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    v->size = 0;                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_fit(struct NAME * v)                                                    \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    if(v->size == v->cap) return 0;                                            \
                                                                               \
    if(v->size == 0)                                                           \
    {                                                                          \
        free(v->data);                                                         \
        v->data = NULL;                                                        \
        v->cap = 0;                                                            \
        return 0;                                                              \
    }                                                                          \
                                                                               \
    if(v->size > (SIZE_MAX / sizeof(TYPE)))                                    \
        return GENC_ERR_OVERFLOW;                                              \
                                                                               \
    void* new_data = realloc(v->data, v->size * sizeof(TYPE));                 \
    if(new_data == NULL)                                                       \
        return GENC_ERR_ALLOC_FAIL;                                            \
                                                                               \
    v->data = new_data;                                                        \
    v->cap = v->size;                                                          \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_prealloc(struct NAME * v, size_t size)                                  \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    if(size == 0) return 0;                                                    \
                                                                               \
    if(size > (SIZE_MAX - v->cap))                                             \
        return GENC_ERR_OVERFLOW;                                              \
                                                                               \
    size_t new_cap = v->cap + size;                                            \
                                                                               \
    if(new_cap > (SIZE_MAX / sizeof(TYPE)))                                    \
        return GENC_ERR_OVERFLOW;                                              \
                                                                               \
    void* new_data = realloc(v->data, new_cap * sizeof(TYPE));                 \
    if(new_data == NULL)                                                       \
        return GENC_ERR_ALLOC_FAIL;                                            \
                                                                               \
    v->data = new_data;                                                        \
    v->cap = new_cap;                                                          \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popb(struct NAME * v)                                                   \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    if(v->size == 0)                                                           \
        return GENC_ERR_NO_DATA;                                               \
                                                                               \
    --(v->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushb(struct NAME * v, TYPE data)                                       \
{                                                                              \
    if(!v) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    int err = NAME##_ins(v, data, v->size);                                    \
                                                                               \
    switch(err)                                                                \
    {                                                                          \
        case 0:                                                                \
            return 0;                                                          \
        case GENC_ERR_ALLOC_FAIL:                                              \
            return GENC_ERR_ALLOC_FAIL;                                        \
        case GENC_ERR_OVERFLOW:                                                \
            return GENC_ERR_OVERFLOW;                                          \
        default:                                                               \
            return GENC_ERR_UNEXPECTED;                                        \
    }                                                                          \
}                                                                              \

/* -------------------------------------------------------------------------- */
/* VECTOR - INLINE */
/* -------------------------------------------------------------------------- */

#define GENC_VECTOR_INLINE(NAME, TYPE, GROWF)                                  \
    GENC_VECTOR_DECLARE(NAME, TYPE, GROWF, static inline)                      \
    GENC_VECTOR_DEFINE(NAME, TYPE, GROWF, static inline)                       \

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* LIST */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* GENC_LIST_DECLARE() and GENC_LIST_DEFINE() generate a type-safe doubly-linked
 * list API. GENC_LIST_INLINE() generates both with `static inline`.
 *
 * The generated structure must be zero-initialized before its first use. */

/* ========================================================================== */
/* LIST - PROTOTYPES */
/* ========================================================================== */

/* --------------------------------------------------------|

struct <name>_node
{
    <type> data;
    struct <name>_node *next, *prev;
};

|----------------------------------------------------------|

struct <name>
{
    struct <name>_node *head, *tail;
    size_t size;
};

|----------------------------------------------------------|

* Deinitializes the list and frees all nodes.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.

int <name>_deinit(struct <name>* list);

|----------------------------------------------------------|

* Appends an element to the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_pushb(struct <name>* list, <type> data);

|----------------------------------------------------------|

* Prepends an element to the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_pushf(struct <name>* list, <type> data);

|----------------------------------------------------------|

* Removes the last element from the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_NO_DATA: The list is empty.

int <name>_popb(struct <name>* list);

|----------------------------------------------------------|

* Removes the first element from the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_NO_DATA: The list is empty.

int <name>_popf(struct <name>* list);

|----------------------------------------------------------|

* Removes all elements and frees all nodes.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.

int <name>_empty(struct <name>* list);

|----------------------------------------------------------|

* Inserts an element after `node`, or at the front if `node` is NULL.
* If non-NULL, `node` must belong to `list`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_ins_after(struct <name>* list, <type> data,
                      struct <name>_node* node);

|----------------------------------------------------------|

* Inserts an element before `node`. `node` must belong to `list`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` or `node` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_ins_before(struct <name>* list, <type> data,
                       struct <name>_node* node);

|----------------------------------------------------------|

* Removes `node` from the list and frees it. `node` must belong to `list`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` or `node` is NULL.

int <name>_rm(struct <name>* list, struct <name>_node* node);

|-------------------------------------------------------- */

/* ========================================================================== */
/* LIST - GENERATOR MACROS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* LIST - DECLARE */
/* -------------------------------------------------------------------------- */

#define GENC_LIST_DECLARE(NAME, TYPE, FN_PREFIX)                               \
                                                                               \
struct NAME                                                                    \
{                                                                              \
    struct NAME##_node *head, *tail;                                           \
    size_t size;                                                               \
};                                                                             \
                                                                               \
struct NAME##_node                                                             \
{                                                                              \
    TYPE data;                                                                 \
    struct NAME##_node *next, *prev;                                           \
};                                                                             \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_deinit(struct NAME * l);                                                \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushb(struct NAME * l, TYPE data);                                      \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushf(struct NAME * l, TYPE data);                                      \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popf(struct NAME * l);                                                  \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popb(struct NAME * l);                                                  \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_empty(struct NAME * l);                                                 \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_ins_after(struct NAME * l, TYPE data, struct NAME##_node* n);           \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_ins_before(struct NAME * l, TYPE data, struct NAME##_node* n);          \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_rm(struct NAME * l, struct NAME##_node* n);                             \

/* -------------------------------------------------------------------------- */
/* LIST - DEFINE */
/* -------------------------------------------------------------------------- */

#define GENC_LIST_DEFINE(NAME, TYPE, FN_PREFIX)                                \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_deinit(struct NAME * l)                                                 \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    struct NAME##_node* it = l->head;                                          \
    struct NAME##_node* next;                                                  \
    while(it)                                                                  \
    {                                                                          \
        next = it->next;                                                       \
        free(it);                                                              \
        it = next;                                                             \
    }                                                                          \
                                                                               \
    l->size = 0;                                                               \
    l->head = NULL;                                                            \
    l->tail = NULL;                                                            \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushb(struct NAME * l, TYPE data)                                       \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    struct NAME##_node* node = malloc(sizeof(struct NAME##_node));             \
    if(node == NULL) return GENC_ERR_ALLOC_FAIL;                               \
                                                                               \
    node->data = data;                                                         \
    node->next = NULL;                                                         \
    node->prev = NULL;                                                         \
                                                                               \
    if(l->size == 0)                                                           \
    {                                                                          \
        l->head = node;                                                        \
        l->tail = node;                                                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        l->tail->next = node;                                                  \
        node->prev = l->tail;                                                  \
        l->tail = node;                                                        \
    }                                                                          \
                                                                               \
    ++(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushf(struct NAME * l, TYPE data)                                       \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    struct NAME##_node* node = malloc(sizeof(struct NAME##_node));             \
    if(node == NULL) return GENC_ERR_ALLOC_FAIL;                               \
                                                                               \
    node->data = data;                                                         \
    node->prev = NULL;                                                         \
    node->next = NULL;                                                         \
                                                                               \
    if(l->size == 0)                                                           \
    {                                                                          \
        l->head = node;                                                        \
        l->tail = node;                                                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        l->head->prev = node;                                                  \
        node->next = l->head;                                                  \
        l->head = node;                                                        \
    }                                                                          \
                                                                               \
    ++(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popf(struct NAME * l)                                                   \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
    if(l->size == 0) return GENC_ERR_NO_DATA;                                  \
                                                                               \
    if(l->size == 1)                                                           \
    {                                                                          \
        free(l->head);                                                         \
        l->head = NULL;                                                        \
        l->tail = NULL;                                                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        struct NAME##_node* old_head = l->head;                                \
        l->head = l->head->next;                                               \
        l->head->prev = NULL;                                                  \
        free(old_head);                                                        \
    }                                                                          \
                                                                               \
    --(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popb(struct NAME * l)                                                   \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
    if(l->size == 0) return GENC_ERR_NO_DATA;                                  \
                                                                               \
    if(l->size == 1)                                                           \
    {                                                                          \
        free(l->head);                                                         \
        l->head = NULL;                                                        \
        l->tail = NULL;                                                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        struct NAME##_node* old_tail = l->tail;                                \
        l->tail = l->tail->prev;                                               \
        l->tail->next = NULL;                                                  \
        free(old_tail);                                                        \
    }                                                                          \
                                                                               \
    --(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_empty(struct NAME * l)                                                  \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    while(l->size > 0)                                                         \
        NAME##_popf(l);                                                        \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_ins_after(struct NAME * l, TYPE data, struct NAME##_node* n)            \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    if((n == NULL) || (n == l->tail))                                          \
    {                                                                          \
        int err;                                                               \
                                                                               \
        if(n == NULL)                                                          \
            err = NAME##_pushf(l, data);                                       \
        else                                                                   \
            err = NAME##_pushb(l, data);                                       \
                                                                               \
        switch(err)                                                            \
        {                                                                      \
            case 0:                                                            \
                return 0;                                                      \
            case GENC_ERR_ALLOC_FAIL:                                          \
                return GENC_ERR_ALLOC_FAIL;                                    \
            default:                                                           \
                return GENC_ERR_UNEXPECTED;                                    \
        }                                                                      \
    }                                                                          \
                                                                               \
    struct NAME##_node* new_node = malloc(sizeof(struct NAME##_node));         \
    if(new_node == NULL) return GENC_ERR_ALLOC_FAIL;                           \
                                                                               \
    new_node->data = data;                                                     \
                                                                               \
    struct NAME##_node* next = n->next;                                        \
                                                                               \
    n->next = new_node;                                                        \
    new_node->prev = n;                                                        \
    new_node->next = next;                                                     \
    next->prev = new_node;                                                     \
                                                                               \
    ++(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_ins_before(struct NAME * l, TYPE data, struct NAME##_node* n)           \
{                                                                              \
    if(!l || !n) return GENC_ERR_INV_ARG;                                      \
                                                                               \
    int err = NAME##_ins_after(l, data, n->prev);                              \
                                                                               \
    switch(err)                                                                \
    {                                                                          \
        case 0:                                                                \
            return 0;                                                          \
        case GENC_ERR_ALLOC_FAIL:                                              \
            return GENC_ERR_ALLOC_FAIL;                                        \
        default:                                                               \
            return GENC_ERR_UNEXPECTED;                                        \
    }                                                                          \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_rm(struct NAME * l, struct NAME##_node* n)                              \
{                                                                              \
    if(!l || !n) return GENC_ERR_INV_ARG;                                      \
                                                                               \
    if((n == l->head) || (n == l->tail))                                       \
    {                                                                          \
        int err;                                                               \
                                                                               \
        if(n == l->head)                                                       \
            err = NAME##_popf(l);                                              \
        else                                                                   \
            err = NAME##_popb(l);                                              \
                                                                               \
        switch(err)                                                            \
        {                                                                      \
            case 0:                                                            \
                return 0;                                                      \
            default:                                                           \
                return GENC_ERR_UNEXPECTED;                                    \
        }                                                                      \
    }                                                                          \
                                                                               \
    struct NAME##_node* prev = n->prev;                                        \
    struct NAME##_node* next = n->next;                                        \
                                                                               \
    prev->next = next;                                                         \
    next->prev = prev;                                                         \
                                                                               \
    free(n);                                                                   \
    --(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \

/* -------------------------------------------------------------------------- */
/* LIST - INLINE */
/* -------------------------------------------------------------------------- */

#define GENC_LIST_INLINE(NAME, TYPE)                                           \
    GENC_LIST_DECLARE(NAME, TYPE, static inline)                               \
    GENC_LIST_DEFINE(NAME, TYPE, static inline)                                \

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* FWD LIST */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* GENC_FWD_LIST_DECLARE() and GENC_FWD_LIST_DEFINE() generate a type-safe
 * forward list API intended for stack or queue use. GENC_FWD_LIST_INLINE()
 * generates both with `static inline`.
 *
 * The generated structure must be zero-initialized before its first use. */

/* ========================================================================== */
/* FWD LIST - PROTOTYPES */
/* ========================================================================== */

/* --------------------------------------------------------|

struct <name>_node
{
    <type> data;
    struct <name>_node* next;
};

|----------------------------------------------------------|

struct <name>
{
    struct <name>_node *head, *tail;
    size_t size;
};

|----------------------------------------------------------|

* Deinitializes the list and frees all nodes.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.

int <name>_deinit(struct <name>* list);

|----------------------------------------------------------|

* Appends an element to the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_pushb(struct <name>* list, <type> data);

|----------------------------------------------------------|

* Prepends an element to the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_pushf(struct <name>* list, <type> data);

|----------------------------------------------------------|

* Removes the first element from the list.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_NO_DATA: The list is empty.

int <name>_popf(struct <name>* list);

|----------------------------------------------------------|

* Removes all elements and frees all nodes.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.

int <name>_empty(struct <name>* list);

|-------------------------------------------------------- */

/* ========================================================================== */
/* FWD LIST - GENERATOR MACROS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FWD LIST - DECLARE */
/* -------------------------------------------------------------------------- */

#define GENC_FWD_LIST_DECLARE(NAME, TYPE, FN_PREFIX)                           \
                                                                               \
struct NAME                                                                    \
{                                                                              \
    struct NAME##_node *head, *tail;                                           \
    size_t size;                                                               \
};                                                                             \
                                                                               \
struct NAME##_node                                                             \
{                                                                              \
    TYPE data;                                                                 \
    struct NAME##_node* next;                                                  \
};                                                                             \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_deinit(struct NAME * l);                                                \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushb(struct NAME * l, TYPE data);                                      \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushf(struct NAME * l, TYPE data);                                      \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popf(struct NAME * l);                                                  \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_empty(struct NAME * l);                                                 \

/* -------------------------------------------------------------------------- */
/* FWD LIST - DEFINE */
/* -------------------------------------------------------------------------- */

#define GENC_FWD_LIST_DEFINE(NAME, TYPE, FN_PREFIX)                            \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_deinit(struct NAME * l)                                                 \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    struct NAME##_node* it = l->head;                                          \
    struct NAME##_node* next;                                                  \
    while(it)                                                                  \
    {                                                                          \
        next = it->next;                                                       \
        free(it);                                                              \
        it = next;                                                             \
    }                                                                          \
                                                                               \
    l->size = 0;                                                               \
    l->head = NULL;                                                            \
    l->tail = NULL;                                                            \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushb(struct NAME * l, TYPE data)                                       \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    struct NAME##_node* node = malloc(sizeof(struct NAME##_node));             \
    if(node == NULL) return GENC_ERR_ALLOC_FAIL;                               \
                                                                               \
    node->data = data;                                                         \
    node->next = NULL;                                                         \
                                                                               \
    if(l->size == 0)                                                           \
    {                                                                          \
        l->head = node;                                                        \
        l->tail = node;                                                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        l->tail->next = node;                                                  \
        l->tail = node;                                                        \
    }                                                                          \
                                                                               \
    ++(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_pushf(struct NAME * l, TYPE data)                                       \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    struct NAME##_node* node = malloc(sizeof(struct NAME##_node));             \
    if(node == NULL) return GENC_ERR_ALLOC_FAIL;                               \
                                                                               \
    node->data = data;                                                         \
    node->next = l->head;                                                      \
                                                                               \
    if(l->size == 0)                                                           \
        l->tail = node;                                                        \
                                                                               \
    l->head = node;                                                            \
    ++(l->size);                                                               \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_popf(struct NAME * l)                                                   \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
    if(l->size == 0) return GENC_ERR_NO_DATA;                                  \
                                                                               \
    struct NAME##_node* old_head = l->head;                                    \
                                                                               \
    l->head = l->head->next;                                                   \
    free(old_head);                                                            \
                                                                               \
    --(l->size);                                                               \
                                                                               \
    if(l->size == 0)                                                           \
        l->tail = NULL;                                                        \
                                                                               \
    return 0;                                                                  \
}                                                                              \
                                                                               \
FN_PREFIX int                                                                  \
NAME##_empty(struct NAME * l)                                                  \
{                                                                              \
    if(!l) return GENC_ERR_INV_ARG;                                            \
                                                                               \
    while(l->size > 0)                                                         \
        NAME##_popf(l);                                                        \
                                                                               \
    return 0;                                                                  \
}                                                                              \

/* -------------------------------------------------------------------------- */
/* FWD LIST - INLINE */
/* -------------------------------------------------------------------------- */

#define GENC_FWD_LIST_INLINE(NAME, TYPE)                                       \
    GENC_FWD_LIST_DECLARE(NAME, TYPE, static inline)                           \
    GENC_FWD_LIST_DEFINE(NAME, TYPE, static inline)                            \

#endif // GENC_H
