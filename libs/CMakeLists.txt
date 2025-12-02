add_library(raylib STATIC IMPORTED GLOBAL
        raylib_namespaced.h raylib_namespaced.cpp)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(aarch64|arm64)")
    set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libraylib_aarch64.a"
    )

elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|amd64)")
    set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libraylib_amd64.a")
else ()
    message(WARNING "Unknown Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

target_include_directories(raylib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(raylib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/raylib_namespaced.h")
