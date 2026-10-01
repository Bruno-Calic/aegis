cmake_minimum_required(VERSION 3.20)
project(aegis
        VERSION 0.1.0
        LANGUAGES CXX
       )
      

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AEGIS_BUILD_TESTS "Build Aegis tests" ON)
option(AEGIS_BUILD_BENCHMARK "Build Aegis benchmark" ON)


set(AEGIS_CORE_SOURCES
      src/core/*.cpp
      src/*.cpp
      )
add_executable(${PROJECT_NAME} 
                main.cpp
            #${AEGIS_CORE_SOURCES}
                )

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE 
                       #programs
                       core
                       )

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
find_package(GTest CONFIG)
if(TARGET GTest::gtest_main)
      add_executable(aegis_tests
                        tests/PathFinderTests.cpp
                        tests/SimulationTests.cpp
                        #${AEGIS_CORE_SOURCES}
                        )
      target_link_libraries(aegis_tests PRIVATE
                              GTest::gtest_main
                              #programs
                              core
                              )
      include(GoogleTest)
      gtest_discover_tests(aegis_tests)
endif()