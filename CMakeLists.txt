cmake_minimum_required(VERSION 3.16)
project(Aegis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8
)
FetchContent_Declare(implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.16
)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(imgui implot googletest)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# --- Zajednička GUI biblioteka ---
add_library(aegis_gui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)
target_include_directories(aegis_gui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${implot_SOURCE_DIR}
)
target_link_libraries(aegis_gui PUBLIC glfw OpenGL::GL)

# --- Core ---
add_subdirectory(src)

# --- Glavni executable ---
add_executable(aegis main.cpp src/programs.cpp)
target_include_directories(aegis PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(aegis PRIVATE core aegis_gui)

# --- Testovi ---
add_executable(aegis_tests
    tests/SimulationTests.cpp
    tests/PathFinderTests.cpp
)
target_include_directories(aegis_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(aegis_tests PRIVATE core GTest::gtest_main)