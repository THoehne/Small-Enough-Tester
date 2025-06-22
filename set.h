#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef INCLUDE_SET_H
#define INCLUDE_SET_H

struct SETest
{
    char *name;
    void (*function)(struct SETest *test);
    bool passed;
    char *error_msg;
};

struct SETSuit
{
    char *name;
    struct SETest **tests;
    bool passed;
    int len;
};

char *format_string(const char *fmt, ...);

// To be implemented by user.
void set_bundle_suits(struct SETSuit **suits, int *counter, bool count);

#define BUNDLE()                                                               \
    void set_bundle_suits(struct SETSuit **suits, int *counter, bool count)

#define ADD_SUIT(suit_name)                                                    \
    if (!count)                                                                \
    {                                                                          \
        suits[(*counter)] = suit_name##_suit_contructor();                     \
    }                                                                          \
    (*counter)++;

#define SUIT(suit_name)                                                        \
    void suit_name##_suit(struct SETSuit *suit, bool count);                   \
    struct SETSuit *suit_name##_suit_contructor()                              \
    {                                                                          \
        struct SETSuit *suit =                                                 \
            (struct SETSuit *)malloc(sizeof(struct SETSuit));                  \
        suit->name = #suit_name;                                               \
        suit_name##_suit(suit, true);                                          \
        suit->tests =                                                          \
            (struct SETest **)malloc(suit->len * sizeof(struct SETest *));     \
        suit->len = 0;                                                         \
        suit_name##_suit(suit, false);                                         \
        return suit;                                                           \
    }                                                                          \
    void suit_name##_suit(struct SETSuit *suit, bool count)

#define ADD_TEST(test_name)                                                    \
    if (!count)                                                                \
    {                                                                          \
        suit->tests[suit->len] = test_name##_test_constructor();               \
    }                                                                          \
    suit->len++;

// TEST MACRO
#define TEST(test_name)                                                        \
    void test_name##_test(struct SETest *test);                                \
    struct SETest *test_name##_test_constructor()                              \
    {                                                                          \
        struct SETest *test = (struct SETest *)malloc(sizeof(struct SETest));  \
        test->name = #test_name;                                               \
        test->function = &test_name##_test;                                    \
        test->passed = true;                                                   \
        test->error_msg = NULL;                                                \
        return test;                                                           \
    }                                                                          \
    void test_name##_test(struct SETest *test)

#endif // !INCLUDE_SET_H
