include(GoogleTest)

enable_testing()

# download GTest
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

# add tests
add_executable(OpenCellTest main.cpp)
target_link_libraries(OpenCellTest GTest::main FruitWork OpenCell.lib) 

gtest_add_tests(TARGET OpenCellTest)