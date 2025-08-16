# repl library
add_library(repl repl.c)

target_include_directories(repl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}   # for error.h
)

target_link_libraries(repl
    PUBLIC
        eval
        replxx
        stdc++
        pthread
)

# future test_repl
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_repl.c")
    add_executable(test_repl test_repl.c)
    target_link_libraries(test_repl
        PRIVATE
            repl
    )
endif()
