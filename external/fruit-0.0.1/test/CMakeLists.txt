include(GoogleTest)

enable_testing()

# download GTest
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

# add tests
add_executable(FruitWorkTest main.cpp comparators.cpp test_state.h comparators.h)
target_link_libraries(FruitWorkTest GTest::main FruitWork) 

gtest_add_tests(TARGET FruitWorkTest)