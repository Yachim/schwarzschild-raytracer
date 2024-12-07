cmake_minimum_required(VERSION 3.0.0)
project(schwarzschild_raytracer VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(schwarzschild_raytracer
    src/main.cpp
    src/glad.c


    src/lib/Camera/camera.cpp
    src/lib/Camera/camera.h

    src/lib/Object/object.cpp
    src/lib/Object/object.h

    src/lib/Material/material.cpp
    src/lib/Material/material.h

    src/lib/MaterialObject/materialObject.cpp
    src/lib/MaterialObject/materialObject.h

    src/lib/Transform/transform.cpp
    src/lib/Transform/transform.h

    src/lib/Sphere/sphere.cpp
    src/lib/Sphere/sphere.h

    src/lib/Plane/plane.cpp
    src/lib/Plane/plane.h

    src/lib/Disk/disk.cpp
    src/lib/Disk/disk.h

    src/lib/HollowDisk/hollowDisk.cpp
    src/lib/HollowDisk/hollowDisk.h

    src/lib/LateralCylinder/lateralCylinder.cpp
    src/lib/LateralCylinder/lateralCylinder.h

    src/lib/Light/light.cpp
    src/lib/Light/light.h

    src/lib/utils/utils.cpp
    src/lib/utils/utils.h

    src/lib/image_utils/image_utils.cpp
    src/lib/image_utils/image_utils.h
    src/lib/image_utils/stb_image.h

    src/lib/shader_utils/shader_utils.cpp
    src/lib/shader_utils/shader_utils.h

    src/lib/Input/input.cpp
    src/lib/Input/input.h
)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(schwarzschild_raytracer copy_assets)

include_directories(include)
target_link_libraries(schwarzschild_raytracer
    glfw
    OpenGL::GL
)