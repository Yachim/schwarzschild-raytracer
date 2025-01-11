// TODO: loading
// TODO: clocks
// FIXME: orthographic camera background
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include "lib/Objects/Camera/camera.h"
#include "lib/Objects/Object/object.h"
#include "lib/Objects/Sphere/sphere.h"
#include "lib/Objects/Disk/disk.h"
#include "lib/Objects/HollowDisk/hollowDisk.h"
#include "lib/Objects/LateralCylinder/lateralCylinder.h"
#include "lib/Objects/Rectangle/rectangle.h"
#include "lib/Objects/Box/box.h"
#include "lib/utils/utils.h"
#include "lib/image_utils/image_utils.h"
#include "lib/Input/input.h"
#include "lib/shader_utils/shader_utils.h"
#include "lib/ObjectLoader/objectLoader.h"
#include <unistd.h>
#include <sys/select.h>
#include "lib/AnimationManager/animationManager.h"
#include "lib/Animations/BobbingAnimation/bobbingAnimation.h"
#include "lib/Animations/RotateAnimation/rotateAnimation.h"
#include "lib/Animations/TrajectoryAnimation/trajectoryAnimation.h"

const uint DEFAULT_WIDTH = 1280;
const uint DEFAULT_HEIGHT = 720;

const float MOVE_SPEED_INCREASE_SPEED = 0.25;
const float MOVE_SPEED = 5.;
const float SENSITIVITY = 0.5;
const float ZOOM_SENSITIVITY = 10.;

const float MIN_FOV = 10.;
const float MAX_FOV = 120.;

// used for clamping
const float MIN_ANGLE = 10. * M_PI / 180.;
const float MAX_ANGLE = 170. * M_PI / 180.;

std::string readStdin() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout) > 0) {
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    return "";
}

// 0 for 2k, 1 for 8k
#define BACKGROUND_TEXTURE_QUALITY 0
#if BACKGROUND_TEXTURE_QUALITY == 0
#define BACKGROUND_TEXTURE_PATH "assets/textures/background/2k.jpg"
#elif BACKGROUND_TEXTURE_QUALITY == 1
#define BACKGROUND_TEXTURE_PATH "assets/textures/background/8k.jpg"
#endif

// noise optimization, set to < 0 to deactivate
#define PERCENT_BLACK 0.75

#define MAX_STEPS 100
#define MAX_REVOLUTIONS 2
#define MAX_TEST_RAY_ANGLE 2. * float(MAX_REVOLUTIONS) * M_PI
#define TEST_RAY_OFFSET 1.

float ddu(float u) {
    return -u * (1. - 1.5 * u);
}

// returns (Δu_{i+1}, Δu'_{i+1})
glm::vec2 rk4_step(float u_i, float du_i, float delta_phi) {
    float k1 = du_i;
    float l1 = ddu(u_i);

    float k2 = du_i + 0.5 * l1 * delta_phi;
    float l2 = ddu(u_i + 0.5 * k1 * delta_phi);

    float k3 = du_i + 0.5 * l2 * delta_phi;
    float l3 = ddu(u_i + 0.5 * k2 * delta_phi);

    float k4 = du_i + l3 * delta_phi;
    float l4 = ddu(u_i + k3 * delta_phi);

    return glm::vec2(delta_phi / 6. * (k1 + 2. * k2 + 2. * k3 + k4), delta_phi / 6. * (l1 + 2. * l2 + 2. * l3 + l4));
}

std::vector<glm::vec3> calculateTestRayPoints(Camera& cam) {
    glm::vec3 dir = cam.getForward();
    glm::vec3 origin = cam.getPos() + dir * float(TEST_RAY_OFFSET);

    glm::vec3 normal_vec = glm::normalize(origin);
    glm::vec3 tangent_vec = glm::normalize(glm::cross(glm::cross(normal_vec, dir), normal_vec));

    float u = 1. / glm::length(origin);
    float du = -u * dot(dir, normal_vec) / dot(dir, tangent_vec);

    if (abs(glm::dot(dir, normal_vec)) >= 1. - 0.000001) { // if radial trajectory
        return { origin, origin + dir };
    }

    std::vector<glm::vec3> out = { origin };

    float phi = 0.;
    for (size_t i = 0; i < MAX_STEPS; i++) {
        float step_size = (MAX_TEST_RAY_ANGLE - phi) / float(MAX_STEPS - i);
        phi += step_size;

        glm::vec2 rk4_result = rk4_step(u, du, step_size);
        u += rk4_result.x;
        if (u < 0. || u > 1.) break;
        du += rk4_result.y;

        out.push_back((float(cos(phi)) * normal_vec + float(sin(phi)) * tangent_vec) / u);
    }

    return out;
}

int main(int, char**) {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Schwarzschild Raytracer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Couldn't load OpenGL" << std::endl;
        glfwTerminate();
        return -1;
    }

#pragma region quad
    float vertices[] = {
        // positions        // texture coords
         1.f,  1.f, 0.0f,    1.f,  1.f,   // top right
         1.f, -1.f, 0.0f,    1.f, -1.f,   // bottom right
        -1.f, -1.f, 0.0f,   -1.f, -1.f,   // bottom left
        -1.f,  1.f, 0.0f,   -1.f,  1.f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#pragma endregion

#pragma region shader
    std::string vertexShaderSource = loadShaderSource("assets/shaders/full_screen_quad.vert");
    std::string fragmentShaderSource = loadShaderSource("assets/shaders/black_hole.frag");
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) return -1;

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
    if (!fragmentShader) return -1;

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#pragma endregion

#pragma region texture
    glUseProgram(shaderProgram);
    loadTexture(BACKGROUND_TEXTURE_PATH);
    glUniform1i(glGetUniformLocation(shaderProgram, "background_texture"), 0);

    std::vector<std::string> texturePaths = {
        "assets/textures/uv_checker.jpg",
        "assets/textures/cubemap.png"
    };

    GLuint textureArrayID = loadTextureArray(texturePaths, shaderProgram);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textures"), 1);
#pragma endregion

#pragma region objects
    Camera cam(glm::vec3(0., 2., 15.), -glm::normalize(glm::vec3(0., 2., 15.)), glm::vec3(1., 0., 0.));

    ObjectLoader* objectLoader = ObjectLoader::getInstance();

    Material mat1;
    mat1.setTextureIndex(0);

    glm::vec3 spherePos = glm::vec3(-10., 0., 0.);
    Sphere sphere(spherePos);
    sphere.setMaterial(&mat1);
    objectLoader->addObject(&sphere);

    Disk disk;
    disk.setRadius(2.);
    disk.setPos(glm::vec3(0., 0., -10.));
    disk.setAxes(glm::angleAxis((float)M_PI / 4.f, glm::normalize(glm::vec3(1.f, 1.f, 1.f))));
    disk.setMaterial(&mat1);
    objectLoader->addObject(&disk);

    HollowDisk accretionDisk;
    accretionDisk.setMaterial(&mat1);
    objectLoader->addObject(&accretionDisk);

    LateralCylinder cyl;
    cyl.setPos(glm::vec3(0., 10., 0.));
    cyl.setHeight(5.);
    cyl.setRadius(2.);
    cyl.setMaterial(&mat1);
    objectLoader->addObject(&cyl);

    Rectangle rect;
    rect.setPos(glm::vec3(0., 0., 10.));
    rect.setWidth(3.);
    rect.setHeight(2.);
    rect.setMaterial(&mat1);
    objectLoader->addObject(&rect);

    Material mat2;
    mat2.setTextureIndex(1);

    Box box;
    box.setPos(glm::vec3(10., 0., 0.));
    box.setMaterial(&mat2);
    objectLoader->addObject(&box);

    Light light;
    light.setIntensity(8.);
    objectLoader->addLight(&light);

    glUseProgram(shaderProgram);

    cam.loadShader(shaderProgram);

    objectLoader->load(shaderProgram);
#pragma endregion

#pragma region input
    Input* input = Input::getInstance();
    glfwSetKeyCallback(window, Input::keyCallback);
    glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
    glfwSetCursorPosCallback(window, Input::cursorPosCallback);
    glfwSetScrollCallback(window, Input::scrollCallback);
#pragma endregion

    AnimationManager* animationManager = AnimationManager::getInstance();

    BobbingAnimation bobbingAnimation(3., 2., &sphere);
    bobbingAnimation.setPoints(sphere.getPos(), glm::vec3(0., 1., 0.), 0.5);
    animationManager->addAnimation(&bobbingAnimation);

    RotateAnimation sphereRotateAnimation(EaseType::LINEAR, 0., 5., &sphere);
    sphereRotateAnimation.setRepeating(true);
    animationManager->addAnimation(&sphereRotateAnimation);

    RotateAnimation boxRotateAnimation(sphereRotateAnimation);
    boxRotateAnimation.setObject(&box);
    animationManager->addAnimation(&boxRotateAnimation);

    TrajectoryAnimation cameraHyperbolic(EaseType::EASE_IN_OUT, 3., 5., &cam);
    cameraHyperbolic.m_func = [](double t) {
        float closestDistanceSquared = pow(10., 2.);
        float a = -closestDistanceSquared / (-30. + 2 * 10.);
        float c = 10. + a;
        float b = sqrt(closestDistanceSquared + 2. * a * 10.);

        float x = -30. + 2. * t * 30.;
        float y = c - a * sqrt(1 + pow(x / b, 2.));

        return x * glm::vec3(0., 0., -1.) + y * glm::vec3(cos(M_PI / 10.), sin(M_PI / 10.), 0.);
        };

    float speed = MOVE_SPEED;
    double prevTime = 0.;
    glm::vec2 prevMouse = input->getMouse();
    GLint raytraceTypeLoc = glGetUniformLocation(shaderProgram, "raytrace_type");
    GLint curvedPercentageLoc = glGetUniformLocation(shaderProgram, "curved_percentage");
    GLint crosshairLoc = glGetUniformLocation(shaderProgram, "crosshair");
    GLint timeLoc = glGetUniformLocation(shaderProgram, "time");
    GLint resolutionLoc = glGetUniformLocation(shaderProgram, "resolution");
    glUniform1f(glGetUniformLocation(shaderProgram, "percent_black"), PERCENT_BLACK);
    glUniform1i(glGetUniformLocation(shaderProgram, "max_steps"), MAX_STEPS);
    glUniform1f(glGetUniformLocation(shaderProgram, "max_revolutions"), MAX_REVOLUTIONS);
    GLint numTestRayCurvedPointsLoc = glGetUniformLocation(shaderProgram, "num_test_ray_curved_points");
    GLint testRayFlatOriginLoc = glGetUniformLocation(shaderProgram, "test_ray_flat_origin");
    GLint testRayFlatDirLoc = glGetUniformLocation(shaderProgram, "test_ray_flat_dir");
    GLint testRayVisibleLoc = glGetUniformLocation(shaderProgram, "test_ray_visible");
    int raytraceType = RaytraceType::FLAT;
    while (!glfwWindowShouldClose(window)) {
        std::string consoleInput = readStdin();
        if (consoleInput != "") {
            std::cout << consoleInput << std::endl;
        }

        glfwPollEvents();

        if (input->isPressed(GLFW_KEY_ESCAPE)) break;

        double windowTime = glfwGetTime();
        double dt = windowTime - prevTime;
        glm::vec2 mouse = input->getMouse();
        glm::vec2 deltaMouse = mouse - prevMouse;

        animationManager->update(windowTime);
        objectLoader->load(shaderProgram);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        int height, width;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

#pragma region movement
        glm::vec3 camPos = cam.getPos();
        glm::vec3 camForward = cam.getForward();
        glm::vec3 camRight = cam.getRight();
        glm::vec3 camUp = cam.getUp();

        glm::vec3 moveDirection = input->getAxis3D();
        moveDirection = moveDirection.x * camRight + moveDirection.y * camUp + moveDirection.z * camForward;

        float moveAmount = glm::length(moveDirection);
        if (moveAmount > 0.) moveDirection /= moveAmount;
        camPos += moveDirection * speed * (float)dt;
        cam.setPos(camPos);

        if (input->isPressed(GLFW_KEY_LEFT_SHIFT)) {
            speed *= 1 + MOVE_SPEED_INCREASE_SPEED * (float)dt;
        }
        else if (input->isPressed(GLFW_KEY_LEFT_CONTROL)) {
            speed *= 1 - MOVE_SPEED_INCREASE_SPEED * (float)dt;
        }

        if (input->isRClicked()) {
            glm::vec2 rotation = -SENSITIVITY * (float)dt * deltaMouse;

            // rotation x-axis
            camForward = rotateVector(rotation.x, camForward);
            camRight = rotateVector(rotation.x, camRight);
            camUp = rotateVector(rotation.x, camUp);

            float angle = acos(-camForward.y);
            float newAngle = glm::clamp(angle + rotation.y, MIN_ANGLE, MAX_ANGLE);
            float dAngle = newAngle - angle;

            // rotation y-axis
            camForward = rotateVector(dAngle, camForward, camRight);
            camUp = rotateVector(dAngle, camUp, camRight);

            cam.setForward(camForward);
            cam.setRight(camRight);
            cam.setUp(camUp);
        }
        else if (!input->isPressed(GLFW_KEY_LEFT_ALT) && input->isLClicked()) {
            float fov = cam.getFov();
            fov += ZOOM_SENSITIVITY * (float)dt * deltaMouse.y;
            if (fov < MIN_FOV) fov = MIN_FOV;
            if (fov > MAX_FOV) fov = MAX_FOV;
            cam.setFov(fov);
        }
#pragma endregion

        if (input->isPressed(GLFW_KEY_R)) {
            std::vector<glm::vec3> points = calculateTestRayPoints(cam);
            glUniform1i(numTestRayCurvedPointsLoc, points.size());
            for (size_t i = 0; i < points.size(); i++) {
                glUniform3f(glGetUniformLocation(shaderProgram, ("test_ray_curved_points[" + std::to_string(i) + "]").c_str()), points[i].x, points[i].y, points[i].z);
            }

            glm::vec3 dir = cam.getForward();
            glm::vec3 origin = cam.getPos() + dir * float(TEST_RAY_OFFSET);
            glUniform3f(testRayFlatOriginLoc, origin.x, origin.y, origin.z);
            glUniform3f(testRayFlatDirLoc, dir.x, dir.y, dir.z);

            glUniform1i(testRayVisibleLoc, true);
        }
        if (input->isPressed(GLFW_KEY_T)) {
            glUniform1i(testRayVisibleLoc, false);
        }

        if (input->isPressed(GLFW_KEY_C)) {
            glUniform1i(crosshairLoc, 1);
        }
        else {
            glUniform1i(crosshairLoc, 0);
        }

        if (input->isPressed(GLFW_KEY_F) && !input->isPressed(GLFW_KEY_F, GLFW_MOD_ALT) && !input->isPressed(GLFW_KEY_F, GLFW_MOD_CAPS_LOCK)) {
            cam.setFov(DEFAULT_FOV);
        }

        if (cameraHyperbolic.isPlaying()) {
            cam.lookAt();
        }
        else if (input->isPressed(GLFW_KEY_H)) {
            animationManager->play(&cameraHyperbolic);
        }

        if (input->isPressed(GLFW_KEY_1)) raytraceType = RaytraceType::CURVED;
        else if (input->isPressed(GLFW_KEY_2)) raytraceType = RaytraceType::FLAT;
        else if (input->isPressed(GLFW_KEY_3)) raytraceType = RaytraceType::HALF_WIDTH;
        else if (input->isPressed(GLFW_KEY_4)) raytraceType = RaytraceType::HALF_HEIGHT;
        glUniform1i(raytraceTypeLoc, raytraceType);

        if (input->isPressed(GLFW_KEY_LEFT_ALT) && input->isLClicked()) {
            if (raytraceType == RaytraceType::HALF_WIDTH) glUniform1f(curvedPercentageLoc, mouse.x / width);
            if (raytraceType == RaytraceType::HALF_HEIGHT) glUniform1f(curvedPercentageLoc, 1. - mouse.y / height);
        }

        if (input->isPressed(GLFW_KEY_L)) cam.lookAt();

#pragma region uniforms
        glUniform1f(timeLoc, (float)glfwGetTime());
        glUniform2f(resolutionLoc, width, height);

        cam.loadShader(shaderProgram);
#pragma endregion

        glfwSwapBuffers(window);
        prevTime = windowTime;
        prevMouse = mouse;
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
