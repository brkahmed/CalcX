add_library(eval eval.c)

target_include_directories(eval
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}   # for error.h
)

target_link_libraries(eval
    PRIVATE
        m
)