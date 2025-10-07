cmake_minimum_required(VERSION 3.28)
project(skinner)

set(CMAKE_CXX_STANDARD 20)

# Used for storing finished skinner timesheets
add_compile_definitions(ARCHIVE_LOCATION="${CMAKE_CURRENT_SOURCE_DIR}/archive/")

add_subdirectory(src)
add_subdirectory(libs)
enable_testing()
add_subdirectory(test)

add_executable(skinner main.cpp)
target_link_libraries(skinner PRIVATE src_library raylib)
target_compile_options(skinner PUBLIC -Werror=switch)
