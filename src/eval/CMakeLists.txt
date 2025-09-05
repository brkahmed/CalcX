add_library(eval eval.c functions.c table.c)

target_include_directories(eval
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(eval
    PRIVATE
        m
)
