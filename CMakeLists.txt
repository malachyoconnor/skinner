cmake_minimum_required(VERSION 3.28)
project(skinner)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
enable_testing()
add_subdirectory(test)

add_executable(skinner main.cpp)
target_link_libraries(skinner PRIVATE src_library)
target_compile_options(skinner PUBLIC -Werror=switch)
