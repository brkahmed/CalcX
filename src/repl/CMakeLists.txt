add_library(repl repl.c)

target_include_directories(repl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}   # for error.h
)

target_link_libraries(repl
    PRIVATE
        eval
        replxx
        stdc++
        pthread
)