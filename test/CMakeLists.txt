# These two lines download the library - including the include header files
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# This basically unzips it
FetchContent_MakeAvailable(googletest)

# This is normal, create an executable
add_executable(test_flag_library flag_test.cpp)

# Give our tests access to the gtest headers
target_link_libraries(
        test_flag_library
        PRIVATE
        flag
        GTest::gtest_main
)

# This allows cmake to use googleTest commands - and discovering those tests is one of them
include(GoogleTest)
gtest_discover_tests(test_flag_library)
