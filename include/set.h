#ifndef __cplusplus
#define _GNU_SOURCE
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>

#ifdef __cplusplus
#include <cstdio>

#endif

#include "utils.h"

#ifndef INCLUDE_SET_H
#define INCLUDE_SET_H

/**
 * Meta data for one test function.
 *
 * See also: struct SETSuit
 */
struct SETest
{
    void (*function)(struct SETest *test);
    const char *error_msg;
    const char *name;
    bool passed;
};

/**
 * Meta data for one test suit.
 *
 * See also: struct SETest
 */
struct SETSuit
{
    const char *name;
    struct SETest *tests;
    int len;
    int shm_key;
    bool (*setup)();
    bool (*tear_down)();
    bool passed;
};

/**
 * Internal function header for bundle. Use BUNDLE() macro to define bundle in
 * test.
 */
void set_bundle_suits(struct SETSuit **suits, int *counter, bool count);

/**
 * Empty suit_setup. Resolving EMPTY in a suit constructor gets
 * us here.
 *
 * Note: Use EMPTY instead of this function.
 */
static bool EMPTY_suit_setup() { return true; }

/**
 * Empty suit_tear_down. Resolving EMPTY in a suit constructor gets
 * us here.
 *
 * Note: Use EMPTY instead of this function.
 */
static bool EMPTY_suit_tear_down() { return true; }

/**
 * Internal header for global setup. Use SETUP() macro instead.
 */
bool set_up();

/**
 * Internal header for global tear down. Use TEAR_DOWN() macro instead.
 */
bool tear_down();

/**
 * Global setup function.
 * Gets called at the start of the whole test program.
 *
 * Should return True if setup was successfull, false if not.
 * Tests are not executed on false.
 *
 * Note: This function has to be set in the tests even if it is not used.
 *
 * See also: NO_SETUP()
 */
#define SETUP() bool set_up()

/**
 * Empty setup function returning true.
 */
#define NO_SETUP                                                               \
    bool set_up() { return true; }

/**
 * Setup function that allways fails. May be usefull to set if tests shouldn't
 * be executed.
 */
#define DEACTIVATE                                                             \
    bool set_up()                                                              \
    {                                                                          \
        printf("All tests are deactivated.\n");                                \
        return false;                                                          \
    }

/**
 * Global tear down function.
 * Gets called at the end of the while test program.
 *
 * Should return true if tear down was successfull, false if not.
 *
 * Note: This function has to be set in the tests even if it is not used.
 */
#define TEAR_DOWN() bool tear_down()

/**
 * Empty tear down function returning true.
 */
#define NO_TEAR_DOWN                                                           \
    bool tear_down() { return true; }

/**
 * Section in the tests to register all suits for execution.
 *
 * See also: ADD_SUIT()
 * See also: SUIT, SUIT_ST
 */
#define BUNDLE()                                                               \
    void set_bundle_suits(struct SETSuit **suits, int *counter, bool count)

/**
 * Used in BUNDLE() to register suit.
 *
 * suit_name (Parameter) Name of the test suit to add.
 */
#define ADD_SUIT(suit_name)                                                    \
    if (!count)                                                                \
    {                                                                          \
        suits[(*counter)] = suit_name##_suit_contructor();                     \
    }                                                                          \
    (*counter)++;

/**
 * Creates a new test suit with local setup and tear_down function.
 *
 * suit_name (Parameter) Name of the suit. (Must be globally unique for suits.)
 * setup_func (SUIT_SETUP) Suit setup function name. (Write name EMPTY if no
 * setup is required.) tear_down_func (SUIT_TEAR_DOWN) Suit tear down function
 * name. (Write name EMPTY if no tear down is required)
 *
 * See also: SUIT_SETUP()
 * See also: SUIT_TEAR_DOWN()
 *
 * Technical Note: We manually allocate suits via mmap to be able to mark them
 * DONTFORK. Since we don't need the suits in a test fork.
 */
#define SUIT_ST(suit_name, setup_func, tear_down_func)                         \
    void suit_name##_suit(struct SETSuit *suit, bool count);                   \
    struct SETSuit *suit_name##_suit_contructor()                              \
    {                                                                          \
        struct SETSuit *suit = (struct SETSuit *)mmap(                         \
            NULL, sizeof(struct SETSuit), PROT_READ | PROT_WRITE,              \
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);                               \
        madvise(suit, sizeof(struct SETSuit), MADV_DONTFORK);                  \
        suit->len = 0;                                                         \
        suit_name##_suit(suit, true);                                          \
        suit->shm_key =                                                        \
            create_shared_suit_space(suit->len * sizeof(struct SETest));       \
        suit->name = #suit_name;                                               \
        suit->tests = (struct SETest *)shmat(suit->shm_key, 0, 0);             \
        if ((uint64_t)suit->tests == -1)                                       \
        {                                                                      \
            fprintf(stderr, "Couldn't attach suit space.\n");                  \
            perror("shmat");                                                   \
            exit(1);                                                           \
        }                                                                      \
        suit->len = 0;                                                         \
        suit->setup = &setup_func##_suit_setup;                                \
        suit->tear_down = &tear_down_func##_suit_tear_down;                    \
        suit_name##_suit(suit, false);                                         \
        return suit;                                                           \
    }                                                                          \
    void suit_name##_suit(struct SETSuit *suit, bool count)

/**
 * Suit local setup function. If registered in suit it get's executed before the
 * suit starts the first test.
 *
 * suit_name (Parameter) Name of the suit setup function. (Must be globally
 * unique for suit setup functions!)
 */
#define SUIT_SETUP(suit_name) bool suit_name##_suit_setup()

/**
 * Suit local tear down function. If registered in suit it get's executed after
 * the last test was executed.
 *
 * suit_name (Parameter) Name of the suit setup function. (Must be globally
 * unique for suit tear down functions!)
 */
#define SUIT_TEAR_DOWN(suit_name) bool suit_name##_suit_tear_down()

/**
 * Suit without setup or tear down.
 *
 * suit_name (Parameter) Name of the suit. (Must be globally unique for suits!)
 */
#define SUIT(suit_name) SUIT_ST(suit_name, EMPTY, EMPTY)

/**
 * Registers a test with a test suit.
 *
 * test_name (Parameter) Name of the test.
 *
 * See also: TEST()
 */
#define ADD_TEST(test_name)                                                    \
    if (!count)                                                                \
    {                                                                          \
        test_name##_test_constructor(&suit->tests[suit->len]);                 \
    }                                                                          \
    suit->len++;

/**
 * Test function. If registered in suit it gets executed be that suit.
 *
 * test_name (Parameter) Name of the test (Must be globally unique for tests!)
 *
 * See also: set_asserts.h
 */
#define TEST(test_name)                                                        \
    void test_name##_test(struct SETest *test);                                \
    void test_name##_test_constructor(struct SETest *test)                     \
    {                                                                          \
        test->name = #test_name;                                               \
        test->function = &test_name##_test;                                    \
        test->passed = true;                                                   \
        test->error_msg = NULL;                                                \
    }                                                                          \
    void test_name##_test(struct SETest *test)

#endif // !INCLUDE_SET_H
