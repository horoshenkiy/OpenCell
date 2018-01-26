#include <gtest\gtest.h>

#include "TestState.h"
#include "TestLogging.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
