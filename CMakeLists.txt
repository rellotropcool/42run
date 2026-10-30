cmake_minimum_required(VERSION 3.20)

project(42run
    VERSION 0.1.0
    DESCRIPTION "Temple-run style endless runner"
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Build settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put the executable directly in build/ (not build/Debug, build/Release) so
# shaders/ and future assets/ live next to it predictably on every platform.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Third-party
# ---------------------------------------------------------------------------
# GLFW ships its own CMake project — just add it and link the target.
set(GLFW_BUILD_DOCS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL       OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

# GLAD is a single .c file + headers. Compiled directly into the target.
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

# ---------------------------------------------------------------------------
# Project target
# ---------------------------------------------------------------------------
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(42run ${PROJECT_SOURCES})

target_include_directories(42run PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb"
)

target_link_libraries(42run PRIVATE
    glfw
    glad
    ${CMAKE_DL_LIBS}
)

# ---------------------------------------------------------------------------
# Copy shaders next to the executable (dev-time convenience)
# ---------------------------------------------------------------------------
add_custom_command(TARGET 42run POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:42run>/shaders"
    COMMENT "Copying shaders/ to build output"
)