cmake_minimum_required(VERSION 3.15)

project(CalcX C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src/eval)
add_subdirectory(src/repl) 
add_subdirectory(extern/replxx)

add_executable(CalcX src/main.c)

target_link_libraries(CalcX PRIVATE eval repl)

enable_testing()
add_subdirectory(tests)