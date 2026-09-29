cmake_minimum_required(VERSION 3.16)
project(aegis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE programs)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)