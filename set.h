#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>

#ifndef INCLUDE_SET_H
#define INCLUDE_SET_H

#define SET_MAX_ERROR_MSG_SIZE 256
#define SET_MAX_NAME_SIZE 64

struct SETest
{
    void (*function)(struct SETest *test);
    const char *error_msg;
    const char *name;
    bool passed;
};

struct SETSuit
{
    const char *name;
    struct SETest *tests;
    int len;
    int shm_key;
    bool passed;
};

char *format_string(const char *fmt, ...);

// To be implemented by user.
void set_bundle_suits(struct SETSuit **suits, int *counter, bool count);

int create_shared_suit_space(size_t size);

#define BUNDLE()                                                               \
    void set_bundle_suits(struct SETSuit **suits, int *counter, bool count)

#define ADD_SUIT(suit_name)                                                    \
    if (!count)                                                                \
    {                                                                          \
        suits[(*counter)] = suit_name##_suit_contructor();                     \
    }                                                                          \
    (*counter)++;

// We manually allocate suits via mmap be able to mark them DONTFORK.
// Since we don't need the suits in a test fork.
#define SUIT(suit_name)                                                        \
    void suit_name##_suit(struct SETSuit *suit, bool count);                   \
    struct SETSuit *suit_name##_suit_contructor()                              \
    {                                                                          \
        struct SETSuit *suit =                                                 \
            mmap(NULL, sizeof(struct SETSuit), PROT_READ | PROT_WRITE,         \
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);                          \
        madvise(suit, sizeof(struct SETSuit), MADV_DONTFORK);                  \
        suit->len = 0;                                                         \
        suit_name##_suit(suit, true);                                          \
        suit->shm_key =                                                        \
            create_shared_suit_space(suit->len * sizeof(struct SETest));       \
        suit->name = #suit_name;                                               \
        suit->tests = shmat(suit->shm_key, 0, 0);                              \
        if (suit->tests == (struct SETest *)-1)                                \
        {                                                                      \
            fprintf(stderr, "Couldn't attach suit space.\n");                  \
            perror("shmat");                                                   \
            exit(1);                                                           \
        }                                                                      \
        suit->len = 0;                                                         \
        suit_name##_suit(suit, false);                                         \
        return suit;                                                           \
    }                                                                          \
    void suit_name##_suit(struct SETSuit *suit, bool count)

#define ADD_TEST(test_name)                                                    \
    if (!count)                                                                \
    {                                                                          \
        test_name##_test_constructor(&suit->tests[suit->len]);                 \
    }                                                                          \
    suit->len++;

// TEST MACRO
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
