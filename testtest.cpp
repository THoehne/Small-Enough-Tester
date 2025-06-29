#include "set.h"
#include "set_asserts.h"

NO_SETUP;

NO_TEAR_DOWN;

TEST(Basic) { ASSERT_EQ(1, 1); }

SUIT(Basic) { ADD_TEST(Basic); }

BUNDLE() { ADD_SUIT(Basic); }
