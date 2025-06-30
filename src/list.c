#include "list.h"

#include <stdio.h>
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
    struct SETBlockMeta *meta =
        (struct SETBlockMeta *)(address - (sizeof(struct SETBlockMeta)));

    printf("Freeing: %p\n", meta);

    if (meta->prev == NULL)
    {
        struct SETBlockMeta *ret = meta->next;
        if (ret)
        {
            ret->prev = NULL;
            ret->end = meta->end;
        }
        free(meta);
        return ret;
    }

    meta->prev->next = meta->next;

    if (meta->next != NULL)
        meta->next->prev = meta->prev;
    else
        head->end = meta->prev;

    free(meta);

    return head;
}
