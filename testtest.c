#include "set.h"
#include "set_asserts.h"

int fac(int n)
{
    if (n <= 1)
        return 1;
    return n * fac(n - 1);
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

SUIT(Basic)
{
    ADD_TEST(Faculty_Basic);
    ADD_TEST(Faculty_Negative);
}

TEST(Other_Basic)
{
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
}

SUIT(Other) { ADD_TEST(Other_Basic); }

BUNDLE()
{
    ADD_SUIT(Basic);
    ADD_SUIT(Other);
}
