#include <gtest\gtest.h>

TEST(SimpleCase, SimpleTest) {
	printf("Simple test\n");
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
