#include "list.h"

#include <stdlib.h>

void set_ll_append(struct SETBlockMeta *head, struct SETBlockMeta *next)
{
    while (head->next)
        head = head->next;

    head->next = next;
    next->prev = head;
}

void set_ll_free_all(struct SETBlockMeta *head)
{
    while (head)
    {
        void *block_start = head;
        head = head->next;
        free(block_start);
    }
}

struct SETBlockMeta *set_ll_free_one(struct SETBlockMeta *head, void *address)
{
    struct SETBlockMeta *meta = address - (sizeof(struct SETBlockMeta));

    if (meta == head)
    {
        struct SETBlockMeta *ret = meta->next;
        ret->prev = NULL;
        ret->end = meta->end;
        free(meta);
        return ret;
    }

    meta->prev->next = meta->next;
    if (meta->next)
        meta->next->prev = meta->prev;

    free(meta);

    return head;
}
