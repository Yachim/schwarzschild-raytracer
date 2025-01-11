cmake_minimum_required(VERSION 3.0.0)
project(schwarzschild_raytracer VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(schwarzschild_raytracer
    src/main.cpp
    src/glad.c


    src/lib/Objects/Camera/camera.cpp
    src/lib/Objects/Camera/camera.h

    src/lib/Objects/Object/object.cpp
    src/lib/Objects/Object/object.h

    src/lib/Objects/Material/material.cpp
    src/lib/Objects/Material/material.h

    src/lib/Objects/MaterialObject/materialObject.cpp
    src/lib/Objects/MaterialObject/materialObject.h

    src/lib/Objects/Transform/transform.cpp
    src/lib/Objects/Transform/transform.h

    src/lib/Objects/Sphere/sphere.cpp
    src/lib/Objects/Sphere/sphere.h

    src/lib/Objects/Plane/plane.cpp
    src/lib/Objects/Plane/plane.h

    src/lib/Objects/Disk/disk.cpp
    src/lib/Objects/Disk/disk.h

    src/lib/Objects/HollowDisk/hollowDisk.cpp
    src/lib/Objects/HollowDisk/hollowDisk.h

    src/lib/Objects/LateralCylinder/lateralCylinder.cpp
    src/lib/Objects/LateralCylinder/lateralCylinder.h

    src/lib/Objects/Rectangle/rectangle.cpp
    src/lib/Objects/Rectangle/rectangle.h

    src/lib/Objects/Box/box.cpp
    src/lib/Objects/Box/box.h

    src/lib/Objects/Light/light.cpp
    src/lib/Objects/Light/light.h

    src/lib/utils/utils.cpp
    src/lib/utils/utils.h

    src/lib/image_utils/image_utils.cpp
    src/lib/image_utils/image_utils.h
    src/lib/image_utils/stb_image.h

    src/lib/shader_utils/shader_utils.cpp
    src/lib/shader_utils/shader_utils.h

    src/lib/ObjectLoader/objectLoader.cpp
    src/lib/ObjectLoader/objectLoader.h

    src/lib/Input/input.cpp
    src/lib/Input/input.h

    src/lib/AnimationManager/animationManager.cpp
    src/lib/AnimationManager/animationManager.h
    src/lib/Animations/Animation/animation.cpp
    src/lib/Animations/Animation/animation.h
    src/lib/Animations/TranslateAnimation/translateAnimation.cpp
    src/lib/Animations/TranslateAnimation/translateAnimation.h
    src/lib/Animations/CombinedAnimation/combinedAnimation.cpp
    src/lib/Animations/CombinedAnimation/combinedAnimation.h
    src/lib/Animations/BobbingAnimation/bobbingAnimation.cpp
    src/lib/Animations/BobbingAnimation/bobbingAnimation.cpp
    src/lib/Animations/RotateAnimation/rotateAnimation.cpp
    src/lib/Animations/RotateAnimation/rotateAnimation.cpp
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