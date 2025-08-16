cmake_minimum_required(VERSION 3.15)
project(CalcProject C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define DEBUG macro when building in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()



# Add subdirectories
add_subdirectory(eval)
add_subdirectory(repl)
add_subdirectory(replxx)  # third-party lib

# Main program
add_executable(calc main.c)

target_include_directories(calc
    PRIVATE
        ${CMAKE_SOURCE_DIR}   # for error.h
)

target_link_libraries(calc
    PRIVATE
        eval
        repl
)

enable_testing()

add_test(NAME EvalTest COMMAND test_eval)

if(TARGET test_repl)
    add_test(NAME ReplTest COMMAND test_repl)
endif()

