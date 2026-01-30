/*

MIT License

Copyright (c) 2025 Novak Stevanović

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights  
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

#ifndef GENC_H
#define GENC_H

/* -------------------------------------------------------------------------- */

/* HEADER BEGIN */

/* -------------------------------------------------------------------------- */

/* Types: VECTOR, LIST, SIMPLE LIST. */

#include <stddef.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/* DEFINE */
/* -------------------------------------------------------------------------- */

typedef int (*genc_cmp_fn)(const void* container_data, const void* user_data);

#define GENC_SUCCESS 0
#define GENC_ERR_INVALID_ARG 1
#define GENC_ERR_ALLOC_FAIL 2
#define GENC_ERR_OUT_OF_BOUNDS 3
#define GENC_ERR_NO_DATA 4
#define GENC_ERR_UNEXPECTED 5

/* -------------------------------------------------------------------------- */
/* VECTOR */
/* -------------------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* VECTOR - PUBLIC */
/* ---------------------------------------------------------------- */

/* 
 * GENC_VECTOR_GENERATE(name, type, growf, cmp_fn) generates a type-safe dynamic
 * vector API.

 * `cmp_fn` is used for element comparisons, such as in `find()` or `rm`
 * functions. If `cmp_fn` is NULL, memcmp() is used instead.
 * `growf` specifies the vector’s growth factor. A value of 1.5 balances
 * memory usage and realloc efficiency.
 *
 * ---------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------
 * 
 * struct <name>
 * {
 *     <type>* data;
 *     size_t size;
 *     size_t cap;
 * };
 * 
 * void <name>_init(struct <name>* vec, size_t init_cap, int* out_status);
 * void <name>_deinit(struct <name>* vec, int* out_status);
 * void <name>_pushb(struct <name>* vec, <type> data, int* out_status);
 * void <name>_popb(struct <name>* vec, int* out_status);
 * void <name>_ins(struct <name>* vec, <type> data, size_t pos, int* out_status);
 * void <name>_rm_at(struct <name>* vec, size_t pos, int* out_status);
 * void <name>_empty(struct <name>* vec, int* out_status);
 * void <name>_rm(struct <name>* vec, <type> data, int* out_status);
 * size_t <name>_find(const struct <name>* vec, <type> data, int* out_status);
 * bool <name>_exists(const struct <name>* vec, <type> data, int* out_status);
 * void <name>_fit(struct <name>* vec, int* out_status); 
 *
 * ---------------------------------------------------------
 * BEHAVIOR
 * ---------------------------------------------------------
 *
 * * void <name>_init() initializes the vector. It allocates the initial chunk of memory.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL or `init_cap` is 0,
 * 2) GENC_ERR_ALLOC_FAIL - initial allocation failed.
 *
 * void <name>_deinit() deinitializes the vector and frees all used memory.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL.
 *
 * void <name>_pushb() appends `data` to the end (back) of the vector.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL,
 * 2) GENC_ERR_ALLOC_FAIL - vector attempted to grow, allocation/realloc failed.
 *
 * void <name>_popb() removes the last element from the vector.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL,
 * 2) GENC_ERR_OUT_OF_BOUNDS - vector is empty.
 *
 * void <name>_ins() inserts `data` at position `pos`. If pos == size, insertion appends.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL,
 * 2) GENC_ERR_OUT_OF_BOUNDS - `pos` is greater than the size of the vector,
 * 3) GENC_ERR_ALLOC_FAIL - vector attempted to grow, allocation/realloc failed.
 *
 * void <name>_rm_at() removes the element at position `pos`.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL,
 * 2) GENC_ERR_OUT_OF_BOUNDS - `pos` is greater than or equal to the size of the vector.
 *
 * void <name>_empty() empties the vector.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL.
 *
 * void <name>_rm() removes the first occurrence of `data` found in the vector.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL,
 * 2) GENC_ERR_NO_DATA - found no matching `data` in `vec`.
 *
 * size_t <name>_find() searches for `data` and returns the position of the
 * first occurrence. On failure it returns `SIZE_MAX`.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL.
 *
 * bool <name>_exists() returns true if `data` exists in the vector.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL.
 *
 * void <name>_fit() shrinks the vector's capacity to match its size.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `vec` is NULL,
 * 2) GENC_ERR_ALLOC_FAIL - realloc failed.
 *
 */

/* ---------------------------------------------------------------- */
/* VECTOR - PRIVATE */
/* ---------------------------------------------------------------- */

struct genc_vector
{
    void* data;
    size_t size;
    size_t cap;
};

void genc_vector_init(struct genc_vector* v, size_t init_cap, size_t _datasz,
                      int* out_status);
void genc_vector_deinit(struct genc_vector* v, int* out_status);

void genc_vector_ins(struct genc_vector* v, const void* _data, size_t pos,
                     size_t _datasz, double _growf, int* out_status);

void genc_vector_rm_at(struct genc_vector* v, size_t pos, size_t _datasz,
                       int* out_status);

void genc_vector_empty(struct genc_vector* v, int* out_status);

void genc_vector_fit(struct genc_vector* v, size_t _datasz, int* out_status);

size_t genc_vector_find(const struct genc_vector* v, const void* _data,
                        genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status);

void genc_vector_popb(struct genc_vector* v, size_t _datasz, int* out_status);

void genc_vector_rm(struct genc_vector* v, const void* _data,
                    genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status);

void genc_vector_pushb(struct genc_vector* v, const void* _data, size_t _datasz,
                       double _growf, int* out_status);

bool genc_vector_exists(const struct genc_vector* v, const void* _data,
                        genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status);

/* ---------------------------------------------------------------- */
/* VECTOR - GENERATOR MACRO */
/* ---------------------------------------------------------------- */

#define GENC_VECTOR_GENERATE(name, type, growf, cmp_fn)                        \
                                                                               \
struct name                                                                    \
{                                                                              \
    type * data;                                                               \
    size_t size;                                                               \
    size_t cap;                                                                \
};                                                                             \
                                                                               \
static inline void                                                             \
name##_init(struct name * v, size_t init_cap, int* out)                        \
{                                                                              \
    genc_vector_init((struct genc_vector*)v, init_cap, sizeof( type ), out);   \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_deinit(struct name * v, int* out)                                       \
{                                                                              \
    genc_vector_deinit((struct genc_vector*)v, out);                           \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_ins(struct name * v, type data, size_t pos, int* out)                   \
{                                                                              \
    genc_vector_ins((struct genc_vector*)v, (const void*)&data, pos,           \
                    sizeof( type ), growf, out);                               \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_rm_at(struct name * v, size_t pos, int* out)                            \
{                                                                              \
    genc_vector_rm_at((struct genc_vector*)v, pos, sizeof( type ), out);       \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_empty(struct name * v, int* out)                                        \
{                                                                              \
    genc_vector_empty((struct genc_vector*)v, out);                            \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_fit(struct name * v, int* out)                                          \
{                                                                              \
    genc_vector_fit((struct genc_vector*)v, sizeof( type ), out);              \
}                                                                              \
                                                                               \
static inline size_t                                                           \
name##_find(const struct name * v, type data, int* out)                        \
{                                                                              \
    return genc_vector_find((const struct genc_vector*)v, (const void*)&data,  \
                            cmp_fn, sizeof( type ), out);                      \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_popb(struct name * v, int* out)                                         \
{                                                                              \
    genc_vector_popb((struct genc_vector*)v, sizeof( type ), out);             \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_rm(struct name * v, type data, int* out)                                \
{                                                                              \
    genc_vector_rm((struct genc_vector*)v, (const void*)&data,                 \
                   cmp_fn, sizeof( type ), out);                               \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_pushb(struct name * v, type data, int* out)                             \
{                                                                              \
    genc_vector_pushb((struct genc_vector*)v, (const void*)&data,              \
                      sizeof( type ), growf, out);                             \
}                                                                              \
                                                                               \
static inline bool                                                             \
name##_exists(const struct name * v, type data, int* out)                      \
{                                                                              \
    return genc_vector_exists((const struct genc_vector*)v,                    \
                              (const void*)&data, cmp_fn,                      \
                              sizeof( type ), out);                            \
}

/* -------------------------------------------------------------------------- */
/* LIST */
/* -------------------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* LIST - PUBLIC */
/* ---------------------------------------------------------------- */

/* 
 * GENC_LIST_GENERATE(name, type, cmp_fn) generates a type-safe doubly-linked
 * list API.

 * `cmp_fn` is used for element comparisons, such as in `find()` or `rm`
 * functions. If `cmp_fn` is NULL, memcmp() is used instead.
 *
 * ---------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------
 * 
 * struct <name>_node
 * {
 *     <type> data;
 *     struct <name>_node* next;
 *     struct <name>_node* prev;
 * };
 * 
 * struct <name>
 * {
 *     size_t size;
 *     struct <name>_node* head;
 *     struct <name>_node* tail;
 * };
 *
 * void <name>_init(struct <name>* list, int* out_status);
 * void <name>_deinit(struct <name>* list, int* out_status);
 *
 * void <name>_pushb(struct <name>* list, <type> data, int* out_status);
 * void <name>_pushf(struct <name>* list, <type> data, int* out_status);
 * void <name>_popb(struct <name>* list, int* out_status);
 * void <name>_popf(struct <name>* list, int* out_status);
 *
 * struct <name>_node*
 * <name>_at(const struct <name>* list, size_t pos, int* out_status);

 * struct <name>_node*
 * <name>_find(const struct <name>* list, <type> data, int* out_status);
 * 
 * bool <name>_exists(const struct <name>* list, <type> data, int* out_status);
 *
 * void <name>_ins_after_node(struct <name>* list, <type> data,
 *                            struct <name>_node* node, int* out_status);
 * void <name>_ins_before_node(struct <name>* list, <type> data,
 *                             struct <name>_node* node, int* out_status);
 * void <name>_ins_at(struct <name>* list, <type> data, size_t pos,
                      int* out_status);
 *
 * void <name>_rm_node(struct <name>* list, struct <name>_node* node,
                       int* out_status);
 * void <name>_rm(struct <name>* list, <type> data, int* out_status);
 *
 * ---------------------------------------------------------
 * BEHAVIOR
 * ---------------------------------------------------------
 *
 * void <name>_init() initializes the list to empty.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL.
 *
 * void <name>_deinit() deinitializes the list and frees all nodes.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL.
 *
 * void <name>_pushb() appends `data` at the tail of the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_ALLOC_FAIL - allocation failed.
 *
 * void <name>_pushf() prepends `data` at the head of the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_ALLOC_FAIL - allocation failed.
 *
 * void <name>_popf() removes the head element.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_NO_DATA - list is empty.
 *
 * void <name>_popb() removes the tail element.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_NO_DATA - list is empty.
 *
 * struct <name>_node* <name>_at() returns pointer to node at index `pos`.
 * Returns NULL on invalid arg or out-of-range.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_OUT_OF_BOUNDS - `pos` >= size,
 *
 * struct <name>_node* <name>_find() finds the first node holding `data`.
 * Returns NULL if not found.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL.
 *
 * bool <name>_exists() returns true if `data` exists in the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2)
 *
 * void <name>_ins_after_node() inserts `data` after `node`.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_ALLOC_FAIL - allocation failed.
 *
 * void <name>_ins_before_node() inserts `data` before `node`.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL or `node` is NULL,
 * 2) GENC_ERR_ALLOC_FAIL - allocation failed.
 *
 * void <name>_ins_at() inserts `data` at index `pos`.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_OUT_OF_BOUNDS - `pos` > size,
 * 3) GENC_ERR_ALLOC_FAIL - allocation failed.
 *
 * void <name>_rm_node() removes the specified node from the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL or `node` is NULL.
 *
 * void <name>_rm() removes the first occurrence of `data` from the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_NO_DATA - no matching `data` found.
 *
 */

/* ---------------------------------------------------------------- */
/* LIST - PRIVATE */
/* ---------------------------------------------------------------- */

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

void genc_list_init(struct genc_list* list, int* out_status);
void genc_list_deinit(struct genc_list* list, int* out_status);

void genc_list_pushb(struct genc_list* list, const void* _data, size_t _datasz,
                     int* out_status);
void genc_list_pushf(struct genc_list* list, const void* _data, size_t _datasz,
                     int* out_status);
void genc_list_popf(struct genc_list* list, int* out_status);
void genc_list_popb(struct genc_list* list, int* out_status);

struct genc_list_node* 
genc_list_find(const struct genc_list* list, const void* _data,
               genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status);

struct genc_list_node*
genc_list_at(const struct genc_list* list, size_t pos, int* out_status);

void genc_list_ins_after_node(struct genc_list* list, const void* _data,
                              struct genc_list_node* node, size_t _datasz,
                              int* out_status);
void genc_list_ins_before_node(struct genc_list* list, const void* _data,
                               struct genc_list_node* node, size_t _datasz,
                               int* out_status);
void genc_list_rm_node(struct genc_list* list, struct genc_list_node* node,
                       int* out_status);
// Convenience

void genc_list_ins_at(struct genc_list* list, const void* _data, size_t pos,
                      size_t _datasz, int* out_status);
void genc_list_rm(struct genc_list* list, const void* _data, genc_cmp_fn _cmp_fn,
                  size_t _datasz, int* out_status);


bool genc_list_exists(const struct genc_list* list, const void* _data,
                      genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status);

/* ---------------------------------------------------------------- */
/* LIST - GENERATOR MACRO */
/* ---------------------------------------------------------------- */

#define GENC_LIST_GENERATE(name, type, cmp_fn)                                 \
                                                                               \
struct name##_node                                                             \
{                                                                              \
    type * data;                                                               \
    struct name##_node *next, *prev;                                           \
};                                                                             \
                                                                               \
struct name                                                                    \
{                                                                              \
    struct name##_node *head, *tail;                                           \
    size_t size;                                                               \
};                                                                             \
                                                                               \
static inline void                                                             \
name##_init(struct name * l, int* out)                                         \
{                                                                              \
    genc_list_init((struct genc_list*)l, out);                                 \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_deinit(struct name * l, int* out)                                       \
{                                                                              \
    genc_list_deinit((struct genc_list*)l, out);                               \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_pushb(struct name * l, type data, int* out)                             \
{                                                                              \
    genc_list_pushb((struct genc_list*)l, (const void*)&data,                  \
                    sizeof( type ), out);                                      \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_pushf(struct name * l, type data, int* out)                             \
{                                                                              \
    genc_list_pushf((struct genc_list*)l, (const void*)&data,                  \
                    sizeof( type ), out);                                      \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_popf(struct name * l, int* out)                                         \
{                                                                              \
    genc_list_popf((struct genc_list*)l, out);                                 \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_popb(struct name * l, int* out)                                         \
{                                                                              \
    genc_list_popb((struct genc_list*)l, out);                                 \
}                                                                              \
                                                                               \
static inline struct name##_node*                                              \
name##_find(const struct name * l, type data, int* out)                        \
{                                                                              \
    struct genc_list_node* _node;                                              \
    _node = genc_list_find((struct genc_list*)l, (const void*)&data, cmp_fn,   \
                           sizeof( type ), out);                               \
    return ( struct name##_node* )_node;                                       \
}                                                                              \
                                                                               \
static inline struct name##_node*                                              \
name##_at(const struct name * l, size_t pos, int* out)                         \
{                                                                              \
    struct genc_list_node* _node;                                              \
    _node = genc_list_at((struct genc_list*)l, pos, out);                      \
    return ( struct name##_node* )_node;                                       \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_ins_after_node(struct name * l, type data,                              \
                      struct name##_node* node, int* out)                      \
{                                                                              \
    genc_list_ins_after_node((struct genc_list*)l, (const void*)&data,         \
                             (struct genc_list_node*)node, sizeof( type ),     \
                             out);                                             \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_ins_before_node(struct name * l, type data,                             \
                       struct name##_node* node, int* out)                     \
{                                                                              \
    genc_list_ins_before_node((struct genc_list*)l, (const void*)&data,        \
                              (struct genc_list_node*)node, sizeof( type ),    \
                              out);                                            \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_rm_node(struct name * l, struct name##_node* node, int* out)            \
{                                                                              \
    genc_list_rm_node((struct genc_list*)l, (struct genc_list_node*)node,      \
                      out);                                                    \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_ins_at(struct name * l, type data, size_t pos, int* out)                \
{                                                                              \
    genc_list_ins_at((struct genc_list*)l, (const void*)&data, pos,            \
                     sizeof( type ), out);                                     \
                                                                               \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_rm(struct name * l, type data, int* out)                                \
{                                                                              \
    genc_list_rm((struct genc_list*)l, (const void*)&data, cmp_fn,             \
                 sizeof( type ), out);                                         \
}                                                                              \
                                                                               \
static inline bool                                                             \
name##_exists(const struct name * l, type data, int* out)                      \
{                                                                              \
    return genc_list_exists((const struct genc_list*)l, (const void*)&data,    \
                            cmp_fn, sizeof( type ), out);                      \
}

/* -------------------------------------------------------------------------- */
/* SIMPLE LIST */
/* -------------------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* SIMPLE LIST - PUBLIC */
/* ---------------------------------------------------------------- */

/* 
 * GENC_SIMPLE_LIST_GENERATE(name, type) generates a type-safe forward list
 * list API. This data structure is meant to be used for creating a stack
 * or queue.
 *
 * ---------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------
 * 
 * struct <name>_node
 * {
 *     <type> data;
 *     struct <name>_node* next;
 * };
 * 
 * struct <name>
 * {
 *     size_t size;
 *     struct <name>_node *head, *tail;
 * };
 *
 * void <name>_init(struct <name>* list, int* out_status);
 * void <name>_deinit(struct <name>* list, int* out_status);
 * void <name>_pushb(struct <name>* list, <type> data, int* out_status);
 * void <name>_pushf(struct <name>* list, <type> data, int* out_status);
 * void <name>_popf(struct <name>* list, int* out_status);
 *
 * ---------------------------------------------------------
 * BEHAVIOR
 * ---------------------------------------------------------
 * 
 * void genc_simple_list_init() initializes an empty list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL.
 *
 * void genc_simple_list_deinit() frees all nodes in the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_UNEXPECTED - an internal pop failed.
 *
 * void genc_simple_list_pushb() appends a copy of `data` to the end of the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` or `data` is NULL, or `data_size` is 0,
 * 2) GENC_ERR_ALLOC_FAIL - memory allocation failed.
 *
 * void genc_simple_list_pushf() prepends a copy of `data` to the front of the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` or `data` is NULL, or `data_size` is 0,
 * 2) GENC_ERR_ALLOC_FAIL - memory allocation failed.
 *
 * void genc_simple_list_popf() removes the first node from the list.
 * ERRORS:
 * 1) GENC_ERR_INVALID_ARG - `list` is NULL,
 * 2) GENC_ERR_NO_DATA - the list is empty.
 *
 */

/* ---------------------------------------------------------------- */
/* SIMPLE LIST - PRIVATE */
/* ---------------------------------------------------------------- */

struct genc_simple_list_node
{
    void* data;
    struct genc_simple_list_node* next;
};

struct genc_simple_list
{
    size_t size;
    struct genc_simple_list_node *head, *tail;
};

void genc_simple_list_init(struct genc_simple_list* list, int* out_status);
void genc_simple_list_deinit(struct genc_simple_list* list, int* out_status);

void genc_simple_list_pushb(struct genc_simple_list* list, const void* _data,
                            size_t _datasz, int* out_status);
void genc_simple_list_pushf(struct genc_simple_list* list, const void* _data,
                            size_t _datasz, int* out_status);
void genc_simple_list_popf(struct genc_simple_list* list, int* out_status);

/* ---------------------------------------------------------------- */
/* SIMPLE LIST - GENERATOR MACRO */
/* ---------------------------------------------------------------- */

#define GENC_SIMPLE_LIST_GENERATE(name, type)                                  \
                                                                               \
struct name##_node                                                             \
{                                                                              \
    type * data;                                                               \
    struct name##_node* next;                                                  \
};                                                                             \
                                                                               \
struct name                                                                    \
{                                                                              \
    size_t size;                                                               \
    struct name##_node *head, *tail;                                           \
};                                                                             \
                                                                               \
static inline void                                                             \
name##_init(struct name * list, int* out)                                      \
{                                                                              \
    genc_simple_list_init((struct genc_simple_list*)list, out);                \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_deinit(struct name * list, int* out)                                    \
{                                                                              \
    genc_simple_list_deinit((struct genc_simple_list*)list, out);              \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_pushb(struct name * l, type data, int* out)                             \
{                                                                              \
    genc_simple_list_pushb((struct genc_simple_list*)l,                        \
                           (const void*)&data, sizeof( type ), out);           \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_pushf(struct name * l, type data, int* out)                             \
{                                                                              \
    genc_simple_list_pushf((struct genc_simple_list*)l,                        \
                           (const void*)&data, sizeof( type ), out);           \
}                                                                              \
                                                                               \
static inline void                                                             \
name##_popf(struct name * l, int* out)                                         \
{                                                                              \
    genc_simple_list_popf((struct genc_simple_list*)l, out);                   \
}                                                                              \

/* -------------------------------------------------------------------------- */

/* HEADER END */

/* -------------------------------------------------------------------------- */

#endif // GENC_H

/* -------------------------------------------------------------------------- */

/* PRIVATE BEGIN */

/* -------------------------------------------------------------------------- */

#ifdef GENC_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>

#define GENC_SET_OUT(out_ptr, val) \
    if((out_ptr) != NULL) { (*(out_ptr)) = (val); }

/* ---------------------------------------------------------------- */
/* VECTOR */
/* ---------------------------------------------------------------- */

void genc_vector_init(struct genc_vector* v, size_t init_cap, size_t _datasz,
                      int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (init_cap == 0) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    v->data = malloc(_datasz * init_cap);
    if(v->data == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
        return;
    }

    v->size = 0;
    v->cap = init_cap;
}

void genc_vector_deinit(struct genc_vector* v, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(v == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->cap = 0;
}

void genc_vector_ins(struct genc_vector* v, const void* _data, size_t pos,
                     size_t _datasz, double _growf, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }
    if(pos > v->size)
    {
        GENC_SET_OUT(out_status, GENC_ERR_OUT_OF_BOUNDS);
        return;
    }

    if(v->size >= v->cap) // grow
    {
        size_t new_cap = (size_t)(v->size * _growf);
        if(new_cap <= v->cap) ++new_cap; // just in case the vector wouldn't grow

        void* new_data = realloc(v->data, new_cap * _datasz);
        if(new_data == NULL)
        {
            GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
            return;
        }

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
}

void genc_vector_rm_at(struct genc_vector* v, size_t pos, size_t _datasz,
                       int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }
    if(pos >= v->size)
    {
        GENC_SET_OUT(out_status, GENC_ERR_OUT_OF_BOUNDS);
        return;
    }

    char* vector_data = (char*)v->data;

    if(pos < (v->size - 1))
    {
        memmove(vector_data + (pos * _datasz),
                vector_data + ((pos + 1) * _datasz),
                (v->size - pos - 1) * _datasz);
    }

    --(v->size);
}

void genc_vector_empty(struct genc_vector* v, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(v == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    v->size = 0;
}

void genc_vector_fit(struct genc_vector* v, size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    if(v->size == v->cap) return;

    void* new_data = realloc(v->data, v->size * _datasz);
    if(new_data == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
        return;
    }
    else
    {
        v->data = new_data;
        v->cap = v->size;
    }
}

size_t genc_vector_find(const struct genc_vector* v, const void* _data,
                        genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return SIZE_MAX;
    }

    const char* vector_data = (const char*)v->data;

    size_t i;
    const void* it_data;

    if(_cmp_fn != NULL)
    {
        for(i = 0; i < v->size; i++)
        {
            it_data = vector_data + (i * _datasz);

            if(_cmp_fn(it_data, _data) == 0)
                return i;
        }
    }
    else
    {
        for(i = 0; i < v->size; i++)
        {
            it_data = vector_data + (i * _datasz);

            if(memcmp(it_data, _data, _datasz) == 0)
                return i;
        }
    }

    return SIZE_MAX;
}

void genc_vector_popb(struct genc_vector* v, size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(v == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    if(v->size == 0)
    {
        GENC_SET_OUT(out_status, GENC_ERR_OUT_OF_BOUNDS);
        return;
    }

    int _status;
    genc_vector_rm_at(v, v->size - 1, _datasz, &_status);

    switch(_status)
    {
        case GENC_SUCCESS:
            return;
        default:
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return;
    }
}

void genc_vector_rm(struct genc_vector* v, const void* _data,
                    genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    int _status;

    size_t pos = genc_vector_find(v, _data, _cmp_fn, _datasz, &_status);
    if(_status != GENC_SUCCESS)
    {
        GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
        return;
    }

    if(pos == SIZE_MAX)
    {
        GENC_SET_OUT(out_status, GENC_ERR_NO_DATA);
        return;
    }

    genc_vector_rm_at(v, pos, _datasz, &_status);

    switch(_status)
    {
        case GENC_SUCCESS:
            return;
        default:
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return;
    }
}

void genc_vector_pushb(struct genc_vector* v, const void* _data, size_t _datasz,
                       double _growf, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    int _status;
    genc_vector_ins(v, _data, v->size, _datasz, _growf, &_status);

    switch(_status)
    {
        case GENC_SUCCESS:
            return;
        case GENC_ERR_ALLOC_FAIL:
            GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
            return;
        default:
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return;
    }
}

bool genc_vector_exists(const struct genc_vector* v, const void* _data,
                        genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((v == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return false;
    }

    int _status;
    size_t pos = genc_vector_find(v, _data, _cmp_fn, _datasz, &_status);

    switch(_status)
    {
        case GENC_SUCCESS:
            return (pos != SIZE_MAX);
        default:
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return false;
    }
}

/* ---------------------------------------------------------------- */
/* LIST */
/* ---------------------------------------------------------------- */

static struct genc_list_node* 
_genc_list_node_create(const void* data, size_t data_size)
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

void genc_list_init(struct genc_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void genc_list_deinit(struct genc_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    while(list->size > 0) genc_list_popf(list, NULL);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void genc_list_pushb(struct genc_list* list, const void* _data,
                     size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    struct genc_list_node* node = _genc_list_node_create(_data, _datasz);
    if(node == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
        return;
    }

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
}

void genc_list_pushf(struct genc_list* list, const void* _data,
                     size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    struct genc_list_node* node = _genc_list_node_create(_data, _datasz);
    if(node == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
        return;
    }

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
}

void genc_list_popf(struct genc_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }
    if(list->size == 0)
    {
        GENC_SET_OUT(out_status, GENC_ERR_NO_DATA);
        return;
    }

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
}

void genc_list_popb(struct genc_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }
    if(list->size == 0)
    {
        GENC_SET_OUT(out_status, GENC_ERR_NO_DATA);
        return;
    }

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
}

struct genc_list_node*
genc_list_at(const struct genc_list* list, size_t pos, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return NULL;
    }
    if(pos >= list->size)
    {
        GENC_SET_OUT(out_status, GENC_ERR_OUT_OF_BOUNDS);
        return NULL;
    }

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

struct genc_list_node* 
genc_list_find(const struct genc_list* list, const void* _data,
               genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return NULL;
    }

    struct genc_list_node* it_node = list->head;

    if(_cmp_fn != NULL)
    {
        while(it_node != NULL)
        {
            if(_cmp_fn(it_node->data, _data) == 0) return it_node;
            it_node = it_node->next;
        }
    }
    else
    {
        while(it_node != NULL)
        {
            if(memcmp(it_node->data, _data, _datasz) == 0) return it_node;
            it_node = it_node->next;
        }
    }

    return NULL;
}

void genc_list_ins_after_node(struct genc_list* list, const void* _data,
                              struct genc_list_node* node, size_t _datasz,
                              int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    if((node == NULL) || (node == list->tail))
    {
        int _status;

        if(node == NULL)
            genc_list_pushf(list, _data, _datasz, &_status);
        else if(node == list->tail)
            genc_list_pushb(list, _data, _datasz, &_status);

        switch(_status)
        {
            case GENC_SUCCESS: return;
            case GENC_ERR_ALLOC_FAIL:
                GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
                return;
            default:
                GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
                return;
        }
    }
    else
    {
        struct genc_list_node* new_node = _genc_list_node_create(_data, _datasz);
        if(new_node == NULL)
        {
            GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
            return;
        }

        struct genc_list_node* next = node->next;

        node->next = new_node;

        new_node->prev = node;
        new_node->next = next;

        next->prev = new_node;

        ++(list->size);
    }
}

void genc_list_ins_before_node(struct genc_list* list, const void* _data,
                               struct genc_list_node* node, size_t _datasz,
                               int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0) || (node == NULL))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    int _status;
    genc_list_ins_after_node(list, _data, node->prev, _datasz, &_status);

    switch(_status)
    {
        case GENC_SUCCESS:
            return;
        case GENC_ERR_ALLOC_FAIL:
            GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
            return;
        default:
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return;
    }
}

void genc_list_rm_node(struct genc_list* list, struct genc_list_node* node,
                       int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (node == NULL))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    if((node == list->head) || (node == list->tail))
    {
        int _status;

        if(node == list->head)
            genc_list_popf(list, &_status);
        else if(node == list->tail)
            genc_list_popb(list, &_status);

        switch(_status)
        {
            case GENC_SUCCESS:
                return;
            default:
                GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
                return;
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
    }
}

void genc_list_ins_at(struct genc_list* list, const void* _data, size_t pos,
                      size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    if(pos > list->size)
    {
        GENC_SET_OUT(out_status, GENC_ERR_OUT_OF_BOUNDS);
        return;
    }

    int _status;

    if(pos == list->size)
    {
        genc_list_pushb(list, _data, _datasz, &_status);
        switch(_status)
        {
            case GENC_SUCCESS:
                return;
            case GENC_ERR_ALLOC_FAIL:
                GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
                return;
            default:
                GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
                return;
        }
    }
    else
    {
        struct genc_list_node* node = genc_list_at(list, pos, &_status);
        if(node == NULL)
        {
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return;
        }

        genc_list_ins_before_node(list, _data, node, _datasz, &_status);
        switch(_status)
        {
            case GENC_SUCCESS:
                return;
            case GENC_ERR_ALLOC_FAIL:
                GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
                return;
            default:
                GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
                return;
        }
    }
}

void genc_list_rm(struct genc_list* list, const void* _data, genc_cmp_fn _cmp_fn,
                  size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);

    if((list == NULL) || (_data == NULL) || (_datasz == 0))
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    int _status;

    struct genc_list_node* node;
    node = genc_list_find(list, _data, _cmp_fn, _datasz, &_status);

    if(_status != GENC_SUCCESS)
    {
        GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
        return;
    }

    if(node == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_NO_DATA);
        return;
    }

    genc_list_rm_node(list, node, &_status);
    if(_status != GENC_SUCCESS) GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
}

bool genc_list_exists(const struct genc_list* list, const void* _data,
                      genc_cmp_fn _cmp_fn, size_t _datasz, int* out_status)
{
    return (genc_list_find(list, _data, _cmp_fn, _datasz, out_status) != NULL);
}

/* -------------------------------------------------------------------------- */
/* SIMPLE LIST */
/* -------------------------------------------------------------------------- */

static struct genc_simple_list_node*
_genc_simple_list_node_create(const void* data, size_t data_size)
{
    size_t node_size = sizeof(struct genc_simple_list_node);
    struct genc_simple_list_node* node = (struct genc_simple_list_node*)
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

void genc_simple_list_init(struct genc_simple_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);
    
    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void genc_simple_list_deinit(struct genc_simple_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);
    
    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }
    
    int _status;
    
    while(list->size > 0)
    {
        genc_simple_list_popf(list, &_status);

        if(_status != GENC_SUCCESS)
        {
            GENC_SET_OUT(out_status, GENC_ERR_UNEXPECTED);
            return;
        }
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void genc_simple_list_pushb(struct genc_simple_list* list, const void* _data,
                            size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);
    
    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }
    
    struct genc_simple_list_node* new_node = _genc_simple_list_node_create(
            _data, _datasz);
    if(new_node == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
        return;
    }

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
}

void genc_simple_list_pushf(struct genc_simple_list* list, const void* _data,
                            size_t _datasz, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);
    
    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    struct genc_simple_list_node*
    new_node = _genc_simple_list_node_create(_data, _datasz);

    if(new_node == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_ALLOC_FAIL);
        return;
    }

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
}

void genc_simple_list_popf(struct genc_simple_list* list, int* out_status)
{
    GENC_SET_OUT(out_status, GENC_SUCCESS);
    
    if(list == NULL)
    {
        GENC_SET_OUT(out_status, GENC_ERR_INVALID_ARG);
        return;
    }

    if(list->size == 0)
    {
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        struct genc_simple_list_node* old_head = list->head;

        list->head = list->head->next;

        free(old_head->data);
        free(old_head);
    }

    --(list->size);
}

/* -------------------------------------------------------------------------- */

/* PRIVATE END */

/* -------------------------------------------------------------------------- */

#endif // GENC_IMPLEMENTATION
