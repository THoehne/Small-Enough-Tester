#include "set.h"
#include <stdio.h>
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

TEAR_DOWN()
{
    printf("Ending execution.");
    return true;
}

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
}

SUIT_SETUP(Basic)
{
    fprintf(stdout, "Some suit setup.\n");
    return true;
}

SUIT_TEAR_DOWN(Basic)
{
    fprintf(stdout, "Some suit teardown\n");
    return true;
}

SUIT_ST(Basic, Basic, Basic)
{
    ADD_TEST(Faculty_Basic);
    ADD_TEST(Faculty_Negative);
}

TEST(Other_Basic)
{
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
}

SUIT_ST(Other, Basic, EMPTY) { ADD_TEST(Other_Basic); }

BUNDLE()
{
    ADD_SUIT(Basic);
    ADD_SUIT(Other);
}
