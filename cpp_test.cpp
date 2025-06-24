#include "set.h"
#include "set_asserts.h"

TEST(Basic) { ASSERT_EQ(2, 2); }

SUIT(Basic) { ADD_TEST(Basic); }

BUNDLE() { ADD_SUIT(Basic); }
