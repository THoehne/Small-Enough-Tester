#include <stddef.h>

struct SETBlockMeta
{
    struct SETBlockMeta *next;
    struct SETBlockMeta *prev;
    struct SETBlockMeta *end;
};

/**
 * Adds a new element to the list. The MetaData should sit at the start
 * of a block.
 *
 * head (SETBlockMeta) List head.
 * next (SETBlockMeta) Next meta to append.
 */
void set_ll_append(struct SETBlockMeta *head, struct SETBlockMeta *next);

/**
 * Frees all blocks in the list.
 *
 * head (SETBlockMeta) List head.
 */
void set_ll_free_all(struct SETBlockMeta *head);

/**
 * Frees the block with the given address and returns the new head.
 *
 * head (SETBlockMeta) List head.
 * address (void*) Address to free.
 */

struct SETBlockMeta *set_ll_free_one(struct SETBlockMeta *head, void *address);
