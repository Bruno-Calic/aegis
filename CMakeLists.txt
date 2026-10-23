cmake_minimum_required(VERSION 3.20)

project(aegis
        VERSION 0.1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AEGIS_BUILD_TESTS "Build Aegis tests" ON)
option(AEGIS_BUILD_BENCHMARK "Build Aegis benchmark" ON)

# Trenutno se ne koristi jer CMake ne širi wildcard *.cpp sam od sebe.
# Ostavljeno je radi tvoje postojeće strukture.
set(AEGIS_CORE_SOURCES
    src/core/*.cpp
    src/*.cpp
)

# ------------------------------------------------------------
# GUI ovisnosti: Dear ImGui + ImPlot + GLFW + OpenGL
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8
)

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.16
)

FetchContent_MakeAvailable(imgui implot)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# ImGui i ImPlot nemaju univerzalni službeni CMake target,
# zato njihove potrebne .cpp datoteke gradimo kao jednu biblioteku.
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

target_link_libraries(aegis_gui PUBLIC
    glfw
    OpenGL::GL
)

target_compile_options(aegis_gui PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# ------------------------------------------------------------
# Core simulacije
# ------------------------------------------------------------
add_subdirectory(src)

# ------------------------------------------------------------
# Glavni program
# ------------------------------------------------------------
add_executable(${PROJECT_NAME}
    main.cpp
    src/programs.cpp
    # ${AEGIS_CORE_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    core
    aegis_gui
    # programs
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# ------------------------------------------------------------
# Testovi
# ------------------------------------------------------------
if(AEGIS_BUILD_TESTS)
    enable_testing()

    find_package(GTest CONFIG REQUIRED)

    if(TARGET GTest::gtest_main)
        add_executable(aegis_tests
            tests/PathFinderTests.cpp
            tests/SimulationTests.cpp
            # ${AEGIS_CORE_SOURCES}
        )

        target_include_directories(aegis_tests PRIVATE
            include
        )

        target_link_libraries(aegis_tests PRIVATE
            GTest::gtest_main
            core
            # programs
        )

        target_compile_options(aegis_tests PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )

        include(GoogleTest)
        gtest_discover_tests(aegis_tests)
    endif()
endif()