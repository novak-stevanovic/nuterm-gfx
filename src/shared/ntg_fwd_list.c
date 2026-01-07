#include "shared/ntg_fwd_list.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static struct ntg_fwd_list_node* node_create(const void* data, size_t data_size)
{
    struct ntg_fwd_list_node* new = malloc(sizeof(struct ntg_fwd_list_node));
    assert(new != NULL);

    void* data_cpy = malloc(data_size);
    assert(data_cpy != NULL);

    memcpy(data_cpy, data, data_size);
    
    new->data = data_cpy;
    new->next = NULL;

    return new;
}

void ntg_fwd_list_init(ntg_fwd_list* list)
{
    assert(list != NULL);

    list->_head = NULL;
    list->_tail = NULL;
    list->_count = 0;
}

void ntg_fwd_list_deinit(ntg_fwd_list* list)
{
    assert(list != NULL);

    while(list->_count > 0)
        ntg_fwd_list_pop_front(list);

    list->_head = NULL;
    list->_tail = NULL;
}

void ntg_fwd_list_push_front(ntg_fwd_list* list, const void* data, size_t data_size)
{
    assert(list != NULL);
    assert(data != NULL);
    assert(data_size > 0);

    struct ntg_fwd_list_node* new = node_create(data, data_size);
    assert(new != NULL);

    if(list->_head == NULL)
    {
        list->_head = new;
        list->_tail = new;
    }
    else
    {
        new->next = list->_head;
        list->_head = new;
    }

    (list->_count)++;
}

void ntg_fwd_list_push_back(ntg_fwd_list* list, const void* data, size_t data_size)
{
    assert(list != NULL);
    assert(data != NULL);
    assert(data_size > 0);

    struct ntg_fwd_list_node* new = node_create(data, data_size);
    assert(new != NULL);

    if(list->_head == NULL)
    {
        list->_head = new;
        list->_tail = new;
    }
    else
    {
        list->_tail->next = new;
        list->_tail = new;
    }

    (list->_count)++;

}

void ntg_fwd_list_pop_front(ntg_fwd_list* list)
{
    assert(list != NULL);
    assert(list->_count > 0);

    free(list->_head->data);
    free(list->_head);

    (list->_count)--;

    if(list->_count == 0)
    {
        list->_head = NULL;
        list->_tail = NULL;
    }
}
