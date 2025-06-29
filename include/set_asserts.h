#include <math.h>
#include <string.h>

#include "set.h"

#ifndef INCLUDE_SET_ASSERT_H
#define INCLUDE_SET_ASSERT_H

#define STATIC_ASSERT(condition) (void)sizeof(char[1 - 2 * (!(condition))])

#define ASSERT_EQ(exp, act)                                                    \
    if (exp != act)                                                            \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg = format_string("Expected %d to be %d\n.", act, exp);  \
        return;                                                                \
    }

#define ASSERT_NEQ(exp, act)                                                   \
    if (exp == act)                                                            \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg =                                                      \
            format_string("Expect %d to be %d.\n", act, exp) return;           \
    }

#define ASSERT_EQ_MSG(exp, act, msg)                                           \
    if (exp != act)                                                            \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg = format_string(                                       \
            "Expected %d to be %d\n    Failed with message: " msg, act, exp);  \
        return;                                                                \
    }

#define ASSERT_TRUE(x)                                                         \
    if (!x)                                                                    \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg = format_string("Expected true got false.");           \
        return;                                                                \
    }

#define ASSERT_FALSE(x)                                                        \
    if (x)                                                                     \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg = format_string("Expected false got true.");           \
        return;                                                                \
    }

#define ASSERT_EQ_FLOAT(exp, act, epsilon)                                     \
    if (fabs(exp - act) <= epsilon)                                            \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg =                                                      \
            format_string("Expected %f to be %f (e: %f)", act, exp, epsilon);  \
        return;                                                                \
    }

#define ASSERT_NEQ_FLOAT(exp, act, epsilon)                                    \
    if (fabs(exp - act) > epsilon)                                             \
    {                                                                          \
        test->passed = false;                                                  \
        test->error_msg =                                                      \
            format_string("Expected %f to be %f (e: %f)", act, exp, epsilon);  \
        return;                                                                \
    }

#define ASSERT_EQ_STR(exp, act)                                                \
    if (strcmp(exp, act) != 0)                                                 \
    {                                                                          \
        test->passed = false;                                                  \
        type->error_msg =                                                      \
            format_string("Expected: %d\nBut got:  %d", act, exp);             \
        return;                                                                \
    }

#define ASSERT_NEQ_STR(exp, act)                                               \
    if (strcmp(exp, act) == 0)                                                 \
    {                                                                          \
        test->passed = false;                                                  \
        type->error_msg =                                                      \
            format_string("Expected: %d\nBut got:  %d", act, exp);             \
        return;                                                                \
    }

#endif
