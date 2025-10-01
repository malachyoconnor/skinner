add_library(src_library
        SkinningCommander.cpp
        SkinningCommander.h
        SkinningSession.h
        FileReadingUtil.h
        utils.h
)

target_include_directories(src_library
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
