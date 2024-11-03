cmake_minimum_required(VERSION 3.0.0)
project(schwarzschild_raytracer VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(schwarzschild_raytracer
    src/main.cpp
    src/stb_image.h
    src/glad.c
)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(schwarzschild_raytracer copy_assets)

target_link_libraries(schwarzschild_raytracer
    glfw
    OpenGL::GL
)