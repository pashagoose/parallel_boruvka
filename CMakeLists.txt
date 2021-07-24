cmake_minimum_required(VERSION 3.14)

# set the project name
project(boruvka)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

add_subdirectory(boruvka_lib)
add_subdirectory(tests)
add_subdirectory(benchmarks)

