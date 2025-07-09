#include "set.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "set_asserts.h"

int fac(int n)
{
    if (n <= 1)
        return 1;
    return n * fac(n - 1);
}

void heavy_load() { sleep(5); }

SETUP()
{
    printf("Setting up.\n");
    return true;
}
NO_TEAR_DOWN

TEST(Faculty_Basic)
{
    ASSERT_EQ(fac(5), 120);
    ASSERT_EQ(fac(4), 24);
    ASSERT_EQ(fac(11), 39916800);
}

TEST(Faculty_Negative)
{
    ASSERT_EQ(fac(-2), 1);
    ASSERT_EQ(fac(0), 1);
    ASSERT_TRUE_MSG(false, "Hello");
}

SUIT_SETUP(Basic_Setup)
{
    fprintf(stdout, "Some suit setup.\n");
    return true;
}

SUIT_TEAR_DOWN(Basic_Tear_Down)
{
    fprintf(stdout, "Some suit teardown\n");
    return true;
}

SUIT_ST(Basic, Basic_Setup, Basic_Tear_Down)
{
    ADD_TEST(Faculty_Basic);
    ADD_TEST(Faculty_Negative);
}

TEST(Other_Basic)
{
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
}

TEST(Other_With_Malloc)
{
    int *some_array = set_malloc(20 * sizeof(int));
    int *array = set_malloc(20);

    for (int i = 0; i < 20; i++)
    {
        some_array[i] = i;
    }

    for (int i = 0; i < 20; i++)
    {
        fprintf(stdout, "%d ", some_array[i]);
    }
    fprintf(stdout, ".\n");

    set_free(array);
    set_free(some_array);

    void *some_other = set_malloc(15);
    ASSERT_TRUE(false);
}

SUIT_ST(Other, Basic_Setup, EMPTY)
{
    ADD_TEST(Other_Basic);
    ADD_TEST(Other_With_Malloc);
}

BUNDLE()
{
    ADD_SUIT(Basic);
    ADD_SUIT(Other);
}
