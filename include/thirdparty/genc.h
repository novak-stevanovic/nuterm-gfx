/*

MIT License

Copyright (c) 2025 Novak Stevanović

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights  a
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
copies of the Software, and to permit persons to whom the Software is  
furnished to do so, subject to the following conditions:  

The above copyright notice and this permission notice shall be included in all  
copies or substantial portions of the Software.  

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN  
THE SOFTWARE.  

*/

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* HEADER - PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#ifndef GENC_H
#define GENC_H

#include <stddef.h>
#include <stdbool.h>

#define GENC_ERR_BASE 1000
#define GENC_ERR_INV_ARG (GENC_ERR_BASE + 1)
#define GENC_ERR_ALLOC_FAIL (GENC_ERR_BASE + 2)
#define GENC_ERR_OUT_OF_BOUNDS (GENC_ERR_BASE + 3)
#define GENC_ERR_NO_DATA (GENC_ERR_BASE + 4)
#define GENC_ERR_UNEXPECTED (GENC_ERR_BASE + 100)

/* ========================================================================== */
/* VECTOR */
/* ========================================================================== */

/*

* GENC_VECTOR_GENERATE(name, type, growf) generates a type-safe dynamic
* vector API.

* The generated structure must be zero-initialized before its first use.

|----------------------------------------------------------|
| Types |
|----------------------------------------------------------|

* If the built-in operations satisfy your program's needs, then you may treat
* the generated types as opaque.

* However, if you wish to implement your own operations, you may have to modify
* the underlying `genc_vector` directly.

struct <name>
{
    struct genc_vector _data;
};

|----------------------------------------------------------|
| Accessors |
|----------------------------------------------------------|

<type>* <name>_data(const struct <name>* vec);
size_t <name>_size(const struct <name>* vec);
size_t <name>_cap(const struct <name>* vec);

|----------------------------------------------------------|
| Operations |
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

int <name>_fit(struct <name>* vec);

|----------------------------------------------------------|

* Increases vector capacity by `size` elements.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `vec` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_prealloc(struct <name>* vec, size_t size);

|----------------------------------------------------------|

*/

/* -------------------------------------------------------------------------- */
/* VECTOR - GENERATOR MACRO */
/* -------------------------------------------------------------------------- */

#define GENC_VECTOR_GENERATE(name, type, growf)                                \
                                                                               \
struct name                                                                    \
{                                                                              \
    struct genc_vector _data;                                                  \
};                                                                             \
                                                                               \
static inline type *                                                           \
name##_data(const struct name * v)                                             \
{                                                                              \
    return (v ? (type *)(v->_data.data) : NULL);                               \
}                                                                              \
                                                                               \
static inline size_t                                                           \
name##_size(const struct name * v)                                             \
{                                                                              \
    return (v ? v->_data.size : 0);                                            \
}                                                                              \
                                                                               \
static inline size_t                                                           \
name##_cap(const struct name * v)                                              \
{                                                                              \
    return (v ? v->_data.cap : 0);                                             \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_deinit(struct name * v)                                                 \
{                                                                              \
    return genc_vector_deinit((v ? &v->_data : NULL));                         \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_ins(struct name * v, type data, size_t pos)                             \
{                                                                              \
    return genc_vector_ins(                                                    \
            (v ? &v->_data : NULL),                                            \
            (const void*)&data,                                                \
            pos,                                                               \
            sizeof( type ),                                                    \
            growf);                                                            \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_rm_at(struct name * v, size_t pos)                                      \
{                                                                              \
    return genc_vector_rm_at(                                                  \
            (v ? &v->_data : NULL),                                            \
            pos,                                                               \
            sizeof( type ));                                                   \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_empty(struct name * v)                                                  \
{                                                                              \
    return genc_vector_empty((v ? &v->_data : NULL));                          \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_fit(struct name * v)                                                    \
{                                                                              \
    return genc_vector_fit((v ? &v->_data : NULL), sizeof( type ));            \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_popb(struct name * v)                                                   \
{                                                                              \
    return genc_vector_popb((v ? &v->_data : NULL));                           \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_pushb(struct name * v, type data)                                       \
{                                                                              \
    return genc_vector_pushb(                                                  \
            (v ? &v->_data : NULL),                                            \
            (const void*)&data,                                                \
            sizeof( type ),                                                    \
            growf);                                                            \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_prealloc(struct name * v, size_t size)                                  \
{                                                                              \
    return genc_vector_prealloc((v ? &v->_data : NULL), size, sizeof( type )); \
}                                                                              \

/* ========================================================================== */
/* LIST */
/* ========================================================================== */

/*

* GENC_LIST_GENERATE(name, type) generates a type-safe doubly-linked list API.

* The generated structure must be zero-initialized before its first use.

|----------------------------------------------------------|
| Types |
|----------------------------------------------------------|

* If the built-in operations satisfy your program's needs, then you may treat
* the generated types as opaque.

* However, if you wish to implement your own operations, you may have to modify
* the underlying `genc_list` directly.

struct <name>
{
    struct genc_list _data;
};

struct <name>_node;

|----------------------------------------------------------|
| Accessors |
|----------------------------------------------------------|

<name>_node* <name>_head(const struct <name>* list);
<name>_node* <name>_tail(const struct <name>* list);
size_t <name>_size(const struct <name>* list);

|----------------------------------------------------------|

<type>* <name>_node_data(const struct <name>_node* node);
<name>_node* <name>_node_next(const struct <name>_node* node);
<name>_node* <name>_node_prev(const struct <name>_node* node);

|----------------------------------------------------------|
| Operations |
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

* Returns the node at `pos`, or NULL if `pos` is invalid.

* RETURN VALUE: Node at `pos`, or NULL on failure.

struct <name>_node* <name>_at(const struct <name>* list, size_t pos);

|----------------------------------------------------------|

* Inserts an element after `node`, or at the front if `node` is NULL.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_ins_after_node(struct <name>* list, <type> data,
                          struct <name>_node* node);

|----------------------------------------------------------|

* Inserts an element before `node`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` or `node` is NULL.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_ins_before_node(struct <name>* list, <type> data,
                           struct <name>_node* node);

|----------------------------------------------------------|

* Inserts an element at `pos`.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` is NULL.
* GENC_ERR_OUT_OF_BOUNDS: `pos` is greater than the list size.
* GENC_ERR_ALLOC_FAIL: Memory allocation failed.

int <name>_ins_at(struct <name>* list, <type> data, size_t pos);

|----------------------------------------------------------|

* Removes `node` from the list and frees it.

* RETURN VALUE: 0 on success, error code on failure.

* ERROR CODES:
* GENC_ERR_INV_ARG: `list` or `node` is NULL.

int <name>_rm_node(struct <name>* list, struct <name>_node* node);

|----------------------------------------------------------|

*/

/* -------------------------------------------------------------------------- */
/* LIST - GENERATOR MACRO */
/* -------------------------------------------------------------------------- */

#define GENC_LIST_GENERATE(name, type)                                         \
                                                                               \
struct name##_node;                                                            \
                                                                               \
struct name                                                                    \
{                                                                              \
    struct genc_list _data;                                                    \
};                                                                             \
                                                                               \
static inline struct name##_node *                                             \
name##_head(const struct name * l)                                             \
{                                                                              \
    return (struct name##_node *)(l ? l->_data.head : NULL);                   \
}                                                                              \
                                                                               \
static inline struct name##_node *                                             \
name##_tail(const struct name * l)                                             \
{                                                                              \
    return (struct name##_node *)(l ? l->_data.tail : NULL);                   \
}                                                                              \
                                                                               \
static inline size_t                                                           \
name##_size(const struct name * l)                                             \
{                                                                              \
    return (l ? l->_data.size : 0);                                            \
}                                                                              \
                                                                               \
static inline type *                                                           \
name##_node_data(const struct name##_node * n)                                 \
{                                                                              \
    const struct genc_list_node *node =                                        \
            (const struct genc_list_node *)n;                                  \
    return (type *)(node ? node->data : NULL);                                 \
}                                                                              \
                                                                               \
static inline struct name##_node *                                             \
name##_node_next(const struct name##_node * n)                                 \
{                                                                              \
    const struct genc_list_node *node =                                        \
            (const struct genc_list_node *)n;                                  \
    return (struct name##_node *)(node ? node->next : NULL);                   \
}                                                                              \
                                                                               \
static inline struct name##_node *                                             \
name##_node_prev(const struct name##_node * n)                                 \
{                                                                              \
    const struct genc_list_node *node =                                        \
            (const struct genc_list_node *)n;                                  \
    return (struct name##_node *)(node ? node->prev : NULL);                   \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_deinit(struct name * l)                                                 \
{                                                                              \
    return genc_list_deinit(l ? &l->_data : NULL);                             \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_pushb(struct name * l, type data)                                       \
{                                                                              \
    return genc_list_pushb(                                                    \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            sizeof(type));                                                     \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_pushf(struct name * l, type data)                                       \
{                                                                              \
    return genc_list_pushf(                                                    \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            sizeof(type));                                                     \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_popf(struct name * l)                                                   \
{                                                                              \
    return genc_list_popf(l ? &l->_data : NULL);                               \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_popb(struct name * l)                                                   \
{                                                                              \
    return genc_list_popb(l ? &l->_data : NULL);                               \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_empty(struct name * l)                                                  \
{                                                                              \
    return genc_list_empty(l ? &l->_data : NULL);                              \
}                                                                              \
                                                                               \
static inline struct name##_node *                                             \
name##_at(const struct name * l, size_t pos)                                   \
{                                                                              \
    return (struct name##_node *)                                              \
            genc_list_at(l ? &l->_data : NULL, pos);                           \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_ins_after_node(struct name * l, type data, struct name##_node * node)   \
{                                                                              \
    return genc_list_ins_after_node(                                           \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            (struct genc_list_node *)node,                                     \
            sizeof(type));                                                     \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_ins_before_node(struct name * l, type data, struct name##_node * node)  \
{                                                                              \
    return genc_list_ins_before_node(                                          \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            (struct genc_list_node *)node,                                     \
            sizeof(type));                                                     \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_rm_node(struct name * l, struct name##_node * node)                     \
{                                                                              \
    return genc_list_rm_node(                                                  \
            l ? &l->_data : NULL,                                              \
            (struct genc_list_node *)node);                                    \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_ins_at(struct name * l, type data, size_t pos)                          \
{                                                                              \
    return genc_list_ins_at(                                                   \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            pos,                                                               \
            sizeof(type));                                                     \
}                                                                              \

/* ========================================================================== */
/* FWD LIST */
/* ========================================================================== */

/*

* GENC_FWD_LIST_GENERATE(name, type) generates a type-safe forward list
* list API. This data structure is meant to be used for creating a stack
* or queue.

* The generated structure must be zero-initialized before its first use.

|----------------------------------------------------------|
| Types |
|----------------------------------------------------------|

* If the built-in operations satisfy your program's needs, then you may treat
* the generated types as opaque.

* However, if you wish to implement your own operations, you may have to modify
* the underlying `genc_fwd_list` directly.

struct <name>
{
    struct genc_fwd_list _data;
};

struct <name>_node;

|----------------------------------------------------------|
| Accessors |
|----------------------------------------------------------|

<name>_node* <name>_head(const struct <name>* list);
<name>_node* <name>_tail(const struct <name>* list);
size_t <name>_size(const struct <name>* list);

|----------------------------------------------------------|

<type>* <name>_node_data(const struct <name>_node* node);
<name>_node* <name>_node_next(const struct <name>_node* node);

|----------------------------------------------------------|
| Operations |
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

|----------------------------------------------------------|

*/

/* -------------------------------------------------------------------------- */
/* FWD LIST - GENERATOR MACRO */
/* -------------------------------------------------------------------------- */

#define GENC_FWD_LIST_GENERATE(name, type)                                     \
                                                                               \
struct name##_node;                                                            \
                                                                               \
struct name                                                                    \
{                                                                              \
    struct genc_fwd_list _data;                                                \
};                                                                             \
                                                                               \
static inline struct name##_node *                                             \
name##_head(const struct name * l)                                             \
{                                                                              \
    return (struct name##_node *)(l ? l->_data.head : NULL);                   \
}                                                                              \
                                                                               \
static inline struct name##_node *                                             \
name##_tail(const struct name * l)                                             \
{                                                                              \
    return (struct name##_node *)(l ? l->_data.tail : NULL);                   \
}                                                                              \
                                                                               \
static inline size_t                                                           \
name##_size(const struct name * l)                                             \
{                                                                              \
    return (l ? l->_data.size : 0);                                            \
}                                                                              \
                                                                               \
static inline type *                                                           \
name##_node_data(const struct name##_node * n)                                 \
{                                                                              \
    const struct genc_fwd_list_node *node =                                    \
            (const struct genc_fwd_list_node *)n;                              \
    return (type *)(node ? node->data : NULL);                                 \
}                                                                              \
                                                                               \
static inline struct name##_node *                                             \
name##_node_next(const struct name##_node * n)                                 \
{                                                                              \
    const struct genc_fwd_list_node *node =                                    \
            (const struct genc_fwd_list_node *)n;                              \
    return (struct name##_node *)(node ? node->next : NULL);                   \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_deinit(struct name * l)                                                 \
{                                                                              \
    return genc_fwd_list_deinit(l ? &l->_data : NULL);                         \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_pushb(struct name * l, type data)                                       \
{                                                                              \
    return genc_fwd_list_pushb(                                                \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            sizeof(type));                                                     \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_pushf(struct name * l, type data)                                       \
{                                                                              \
    return genc_fwd_list_pushf(                                                \
            l ? &l->_data : NULL,                                              \
            (const void *)&data,                                               \
            sizeof(type));                                                     \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_popf(struct name * l)                                                   \
{                                                                              \
    return genc_fwd_list_popf(l ? &l->_data : NULL);                           \
}                                                                              \
                                                                               \
static inline int                                                              \
name##_empty(struct name * l)                                                  \
{                                                                              \
    return genc_fwd_list_empty(l ? &l->_data : NULL);                          \
}                                                                              \

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* HEADER - INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* VECTOR - INTERNAL */
/* ========================================================================== */

struct genc_vector
{
    void* data;
    size_t size;
    size_t cap;
};

int genc_vector_deinit(struct genc_vector* v);

int genc_vector_ins(struct genc_vector* v, const void* _data, size_t pos,
                    size_t _datasz, double _growf);

int genc_vector_rm_at(struct genc_vector* v, size_t pos, size_t _datasz);

int genc_vector_empty(struct genc_vector* v);

int genc_vector_fit(struct genc_vector* v, size_t _datasz);

int genc_vector_prealloc(struct genc_vector* v, size_t size, size_t _datasz);

int genc_vector_popb(struct genc_vector* v);

int genc_vector_pushb(struct genc_vector* v, const void* _data, size_t _datasz,
                      double _growf);

/* ========================================================================== */
/* LIST - INTERNAL */
/* ========================================================================== */

struct genc_list_node
{
    void* data;
    struct genc_list_node *next, *prev;
};

struct genc_list
{
    struct genc_list_node *head, *tail;
    size_t size;
};

int genc_list_deinit(struct genc_list* list);

int genc_list_pushb(struct genc_list* list, const void* _data, size_t _datasz);
int genc_list_pushf(struct genc_list* list, const void* _data, size_t _datasz);

int genc_list_popf(struct genc_list* list);
int genc_list_popb(struct genc_list* list);
int genc_list_empty(struct genc_list* list);

struct genc_list_node* genc_list_at(const struct genc_list* list, size_t pos);

int genc_list_ins_after_node(struct genc_list* list, const void* _data,

                              struct genc_list_node* node, size_t _datasz);
int genc_list_ins_before_node(struct genc_list* list, const void* _data,
                               struct genc_list_node* node, size_t _datasz);

int genc_list_rm_node(struct genc_list* list, struct genc_list_node* node);

int genc_list_ins_at(struct genc_list* list, const void* _data, size_t pos,
                      size_t _datasz);

/* ========================================================================== */
/* FWD LIST - INTERNAL */
/* ========================================================================== */

struct genc_fwd_list_node
{
    void* data;
    struct genc_fwd_list_node* next;
};

struct genc_fwd_list
{
    size_t size;
    struct genc_fwd_list_node *head, *tail;
};

int genc_fwd_list_deinit(struct genc_fwd_list* list);

int genc_fwd_list_pushb(struct genc_fwd_list* list, const void* _data,
                        size_t _datasz);

int genc_fwd_list_pushf(struct genc_fwd_list* list, const void* _data,
                        size_t _datasz);

int genc_fwd_list_popf(struct genc_fwd_list* list);

int genc_fwd_list_empty(struct genc_fwd_list* list);

/* ========================================================================== */

#endif // GENC_H

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* IMPLEMENTATION - INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#if defined(GENC_IMPLEMENTATION) && !defined(GENC_IMPLEMENTATION_INCLUDED)
#define GENC_IMPLEMENTATION_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>

#define GENC_NOT_NULL(ptr)                                                     \
    do                                                                         \
    {                                                                          \
        if(!(ptr)) return GENC_ERR_INV_ARG;                                    \
    } while(0)

/* ========================================================================== */
/* VECTOR */
/* ========================================================================== */

int genc_vector_deinit(struct genc_vector* v)
{
    GENC_NOT_NULL(v);

    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->cap = 0;

    return 0;
}

int genc_vector_ins(struct genc_vector* v, const void* _data, size_t pos,
                     size_t _datasz, double _growf)
{
    GENC_NOT_NULL(v);
    GENC_NOT_NULL(_data);

    if(pos > v->size)
        return GENC_ERR_OUT_OF_BOUNDS;

    if(v->size >= v->cap) // grow
    {
        // Calculate new_cap
        size_t new_cap = v->size * _growf;
        if(new_cap <= v->cap)
            new_cap = v->cap + 1;

        void* new_data = realloc(v->data, new_cap * _datasz);
        if(new_data == NULL)
            return GENC_ERR_ALLOC_FAIL;

        v->data = new_data;
        v->cap = new_cap;
    }

    char* vector_data = (char*)v->data;

    if(pos < v->size) // make space
    {
        memmove(vector_data + ((pos + 1) * _datasz),
                vector_data + (pos * _datasz), 
                (v->size - pos) * _datasz);
    }

    memcpy(vector_data + (pos * _datasz), _data, _datasz);
    ++(v->size);

    return 0;
}

int genc_vector_rm_at(struct genc_vector* v, size_t pos, size_t _datasz)
{
    GENC_NOT_NULL(v);

    if(pos >= v->size)
        return GENC_ERR_OUT_OF_BOUNDS;

    // Pop back
    if(pos == (v->size - 1))
    {
        int err = genc_vector_popb(v);
        switch(err)
        {
            case 0:
                return 0;
            default:
                return GENC_ERR_UNEXPECTED;
        }
    }

    char* vector_data = (char*)v->data;

    memmove(vector_data + (pos * _datasz),
            vector_data + ((pos + 1) * _datasz),
            (v->size - pos - 1) * _datasz);

    --(v->size);

    return 0;
}

int genc_vector_empty(struct genc_vector* v)
{
    GENC_NOT_NULL(v);

    v->size = 0;

    return 0;
}

int genc_vector_fit(struct genc_vector* v, size_t _datasz)
{
    GENC_NOT_NULL(v);

    if(v->size == v->cap) return 0;

    if(v->size == 0)
    {
        free(v->data);
        v->data = NULL;
        v->cap = 0;
        return 0;
    }

    void* new_data = realloc(v->data, v->size * _datasz);
    if(new_data == NULL)
        return GENC_ERR_ALLOC_FAIL;
    else
    {
        v->data = new_data;
        v->cap = v->size;
    }

    return 0;
}

int genc_vector_prealloc(struct genc_vector* v, size_t size, size_t _datasz)
{
    GENC_NOT_NULL(v);

    if(size == 0) return 0;

    size_t new_cap = v->cap + size;

    void* new_data = realloc(v->data, new_cap * _datasz);
    if(!new_data)
        return GENC_ERR_ALLOC_FAIL;

    v->data = new_data;
    v->cap = new_cap;

    return 0;
}

int genc_vector_popb(struct genc_vector* v)
{
    GENC_NOT_NULL(v);

    if(v->size == 0)
        return GENC_ERR_NO_DATA;

    (v->size)--;

    return 0;
}

int genc_vector_pushb(struct genc_vector* v, const void* _data, size_t _datasz,
                      double _growf)
{
    GENC_NOT_NULL(v);
    GENC_NOT_NULL(_data);

    int err = genc_vector_ins(v, _data, v->size, _datasz, _growf);

    switch(err)
    {
        case 0:
            return 0;
        case GENC_ERR_ALLOC_FAIL:
            return GENC_ERR_ALLOC_FAIL;
        default:
            return GENC_ERR_UNEXPECTED;
    }

    return 0;
}

/* ========================================================================== */
/* LIST */
/* ========================================================================== */

static struct genc_list_node* 
genc__list_node_create(const void* data, size_t data_size)
{
    size_t node_size = sizeof(struct genc_list_node);
    struct genc_list_node* node = (struct genc_list_node*)malloc(node_size);

    if(node == NULL) return NULL;

    node->data = malloc(data_size);
    if(node->data == NULL)
    {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, data_size);
    node->next = NULL;
    node->prev = NULL;

    return node;
}

int genc_list_deinit(struct genc_list* list)
{
    GENC_NOT_NULL(list);

    genc_list_empty(list);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return 0;
}

int genc_list_pushb(struct genc_list* list, const void* _data, size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(_data);

    struct genc_list_node* node = genc__list_node_create(_data, _datasz);
    if(node == NULL)
        return GENC_ERR_ALLOC_FAIL;

    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;

        list->tail = node;
    }

    ++(list->size);

    return 0;
}

int genc_list_pushf(struct genc_list* list, const void* _data, size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(_data);

    struct genc_list_node* node = genc__list_node_create(_data, _datasz);
    if(node == NULL)
        return GENC_ERR_ALLOC_FAIL;

    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->head->prev = node;
        node->next = list->head;

        list->head = node;
    }

    ++(list->size);

    return 0;
}

int genc_list_popf(struct genc_list* list)
{
    GENC_NOT_NULL(list);

    if(list->size == 0)
        return GENC_ERR_NO_DATA;

    if(list->size == 1)
    {
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        struct genc_list_node* old_head = list->head;

        list->head = list->head->next;
        list->head->prev = NULL;

        free(old_head->data);
        free(old_head);
    }

    --(list->size);

    return 0;
}

int genc_list_popb(struct genc_list* list)
{
    GENC_NOT_NULL(list);

    if(list->size == 0)
        return GENC_ERR_NO_DATA;

    if(list->size == 1)
    {
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        struct genc_list_node* old_tail = list->tail;
        list->tail = list->tail->prev;
        list->tail->next = NULL;

        free(old_tail->data);
        free(old_tail);
    }

    --(list->size);

    return 0;
}

int genc_list_empty(struct genc_list* list)
{
    GENC_NOT_NULL(list); 

    while(list->size > 0)
        genc_list_popf(list);

    return 0;
}

struct genc_list_node*
genc_list_at(const struct genc_list* list, size_t pos)
{
    if(!list) return NULL;
    if(pos >= list->size) return NULL;

    struct genc_list_node* it_node;
    size_t i;
    if(pos >= (list->size / 2))
    {
        it_node = list->tail;
        for(i = list->size - 1; i > pos; i--)
            it_node = it_node->prev;
    }
    else
    {
        it_node = list->head;
        for(i = 0; i < pos; i++)
            it_node = it_node->next;
    }

    return it_node;
}

int genc_list_ins_after_node(struct genc_list* list, const void* _data,
                             struct genc_list_node* node, size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(_data);

    if((node == NULL) || (node == list->tail))
    {
        int err;

        if(node == NULL)
            err = genc_list_pushf(list, _data, _datasz);
        else if(node == list->tail)
            err = genc_list_pushb(list, _data, _datasz);

        switch(err)
        {
            case 0: 
                return 0;
            case GENC_ERR_ALLOC_FAIL:
                return GENC_ERR_ALLOC_FAIL;
            default:
                return GENC_ERR_UNEXPECTED;
        }
    }
    else
    {
        struct genc_list_node* new_node = genc__list_node_create(_data, _datasz);
        if(new_node == NULL)
            return GENC_ERR_ALLOC_FAIL;

        struct genc_list_node* next = node->next;

        node->next = new_node;

        new_node->prev = node;
        new_node->next = next;

        next->prev = new_node;

        ++(list->size);

        return 0;
    }
}

int genc_list_ins_before_node(struct genc_list* list, const void* _data,
                              struct genc_list_node* node, size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(node);
    GENC_NOT_NULL(_data);

    int err = genc_list_ins_after_node(list, _data, node->prev, _datasz);

    switch(err)
    {
        case 0:
            return 0;
        case GENC_ERR_ALLOC_FAIL:
            return GENC_ERR_ALLOC_FAIL;
        default:
            return GENC_ERR_UNEXPECTED;
    }
}

int genc_list_rm_node(struct genc_list* list, struct genc_list_node* node)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(node);

    if((node == list->head) || (node == list->tail))
    {
        int err;

        if(node == list->head)
            err = genc_list_popf(list);
        else if(node == list->tail)
            err = genc_list_popb(list);

        switch(err)
        {
            case 0:
                return 0;
            default:
                return GENC_ERR_UNEXPECTED;
        }
    }
    else
    {
        struct genc_list_node* prev = node->prev;
        struct genc_list_node* next = node->next;

        prev->next = next;
        next->prev = prev;

        free(node->data);
        free(node);

        --(list->size);

        return 0;
    }
}

int genc_list_ins_at(struct genc_list* list, const void* _data, size_t pos,
                     size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(_data);

    if(pos > list->size)
        return GENC_ERR_OUT_OF_BOUNDS;

    int err;

    if(pos == list->size)
    {
        err = genc_list_pushb(list, _data, _datasz);
        switch(err)
        {
            case 0:
                return 0;
            case GENC_ERR_ALLOC_FAIL:
                return GENC_ERR_ALLOC_FAIL;
            default:
                return GENC_ERR_UNEXPECTED;
        }
    }
    else
    {
        struct genc_list_node* node = genc_list_at(list, pos);
        if(node == NULL)
            return GENC_ERR_UNEXPECTED;

        err = genc_list_ins_before_node(list, _data, node, _datasz);
        switch(err)
        {
            case 0:
                return 0;
            case GENC_ERR_ALLOC_FAIL:
                return GENC_ERR_ALLOC_FAIL;
            default:
                return GENC_ERR_UNEXPECTED;
        }
    }
}

/* ========================================================================== */
/* FWD LIST */
/* ========================================================================== */

static struct genc_fwd_list_node*
genc__fwd_list_node_create(const void* data, size_t data_size)
{
    size_t node_size = sizeof(struct genc_fwd_list_node);
    struct genc_fwd_list_node* node = (struct genc_fwd_list_node*)
        malloc(node_size);

    if(node == NULL) return NULL;

    node->data = malloc(data_size);
    if(node->data == NULL)
    {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, data_size);
    node->next = NULL;

    return node;
}

int genc_fwd_list_deinit(struct genc_fwd_list* list)
{
    GENC_NOT_NULL(list);
    
    genc_fwd_list_empty(list);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return 0;
}

int genc_fwd_list_pushb(struct genc_fwd_list* list, const void* _data,
                        size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(_data);

    struct genc_fwd_list_node* new_node;  
    new_node = genc__fwd_list_node_create(_data, _datasz);
    if(new_node == NULL)
        return GENC_ERR_ALLOC_FAIL;

    if(list->size == 0)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    ++(list->size);

    return 0;
}

int genc_fwd_list_pushf(struct genc_fwd_list* list, const void* _data,
                        size_t _datasz)
{
    GENC_NOT_NULL(list);
    GENC_NOT_NULL(_data);
    
    struct genc_fwd_list_node*
    new_node = genc__fwd_list_node_create(_data, _datasz);
    if(new_node == NULL)
        return GENC_ERR_ALLOC_FAIL;

    if(list->size == 0)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        new_node->next = list->head;
        list->head = new_node;
    }

    ++(list->size);

    return 0;
}

int genc_fwd_list_popf(struct genc_fwd_list* list)
{
    GENC_NOT_NULL(list);
    
    if(list->size == 0)
        return GENC_ERR_NO_DATA;

    if(list->size == 1)
    {
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        struct genc_fwd_list_node* old_head = list->head;

        list->head = list->head->next;

        free(old_head->data);
        free(old_head);
    }

    --(list->size);

    return 0;
}

int genc_fwd_list_empty(struct genc_fwd_list* list)
{
    GENC_NOT_NULL(list);

    while(list->size > 0)
        genc_fwd_list_popf(list);

    return 0;
}

#endif // GENC_IMPLEMENTATION && !GENC_IMPLEMENTATION_INCLUDED/*
