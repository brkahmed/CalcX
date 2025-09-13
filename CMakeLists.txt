cmake_minimum_required(VERSION 3.15)

project(CalcX C)

include(CheckIncludeFile)
include(CheckLibraryExists)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

# fallback to libmath if libquadmath wasn't found
check_include_file("quadmath.h" HAVE_QUADMATH_H)
check_library_exists(quadmath logq "" HAVE_QUADMATH_LIB)
if(HAVE_QUADMATH_H AND HAVE_QUADMATH_LIB)
    add_definitions(-DHAVE_QUADMATH)
    set(MATH_LIBRARY quadmath)
else()
    message(WARNING "quadmath not found, falling back to math.h")
    set(MATH_LIBRARY m)
endif()


include_directories(src/common)

add_subdirectory(src/eval)
add_subdirectory(src/repl) 
add_subdirectory(extern/replxx)

add_executable(CalcX src/main.c)

target_link_libraries(CalcX PRIVATE eval repl)

enable_testing()
add_subdirectory(tests)