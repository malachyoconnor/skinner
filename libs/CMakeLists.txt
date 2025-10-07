add_library(raylib INTERFACE)

target_sources(raylib INTERFACE raylib.h)

target_include_directories(raylib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})