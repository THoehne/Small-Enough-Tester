#include <stddef.h>

/*
 * Returns pointer to formatted the string.
 *
 * fmt (const char*) Formatter string.
 * ... (args) Arguments.
 *
 * Note: The string is dynamically allocated and must be freed.
 */
char *format_string(const char *fmt, ...);

/**
 * Internal function to create shared memory for all tests in a suit.
 *
 * size (size_t) Size of space in bytes.
 */
int create_shared_suit_space(size_t size);

/**
 * Wrapper for malloc function that notes down the allocated blocks. All
 * blocks allocated with this function get freed after test execution.
 * This function should only be used if the object might still be in memory
 * once a test fails. Manual allocation should be preferred.
 *
 * n (size_t) Amount of bytes to allocate.
 */
void *set_malloc(size_t n);

/**
 * Wrapper for fee(void*).
 *
 * See also: set_malloc()
 */
void set_free(void *address);

/**
 * Internal function to free all allocated and registered memory.
 */
void set_free_all();

/**
 * Wrapper for calloc.
 *
 * See also: set_malloc()
 */
void *set_calloc(size_t n, size_t size);

/**
 * Wrapper for realloc().
 *
 * See also: set_malloc()
 */
void *set_realloc(size_t n);
