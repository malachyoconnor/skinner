add_library(src_library
        SkinningCommander.cpp
        SkinningCommander.h
        SkinningSession.h
        SkinningInterval.h
        FileReadingUtil.h
        utils.h
)

target_include_directories(src_library
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(flag flag.h)
set_target_properties(flag PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(flag
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)


