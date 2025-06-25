#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "list.h"
#include <unistd.h>

struct SETBlockMeta *block_meta_head = NULL;

char *format_string(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    char *out = (char *)malloc(size + 1);

    if (!out)
    {
        fprintf(stderr, "%s", "Failed to allocate for format string.\n");
    }

    va_end(args);

    va_start(args, fmt);
    vsnprintf(out, size + 1, fmt, args);
    va_end(args);

    return out;
}

int create_shared_suit_space(size_t size)
{
    int suit_space_id = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);

    if (suit_space_id == -1)
    {
        fprintf(stdout, "Couldn't create suite space of size: %lu\n", size);
        perror("shmget");
        exit(1);
    }

    return suit_space_id;
}

void *set_malloc(size_t n)
{
    void *blocks = malloc(n + sizeof(struct SETBlockMeta));
    struct SETBlockMeta *meta = blocks;
    meta->next = NULL;

    if (block_meta_head == NULL)
    {
        meta->end = meta;
        block_meta_head = meta;
    }
    else
    {
        meta->end = NULL;
        set_ll_append(block_meta_head->end, meta);
        block_meta_head->end = meta;
    }

    return blocks + sizeof(struct SETBlockMeta);
}

void set_free(void *address)
{
    block_meta_head = set_ll_free_one(block_meta_head, address);
}

void set_free_all()
{
    set_ll_free_all(block_meta_head);
    block_meta_head = NULL;
}
