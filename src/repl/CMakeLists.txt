add_library(repl repl.c)

target_include_directories(repl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(repl
    PRIVATE
        eval
        replxx
        stdc++
        pthread
)