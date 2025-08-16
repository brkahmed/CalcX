# eval library
add_library(eval eval.c)

target_include_directories(eval
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}   # for error.h
)

target_link_libraries(eval
    PUBLIC
        m   # link math library here
)

# test_eval program
add_executable(test_eval test_eval.c)

target_link_libraries(test_eval
    PRIVATE
        eval
)
