// TODO: loading
// TODO: clocks
// FIXME: accretion disk casts shadow on the moon even if moon is in front of the accretion disk
// FIXME: accretion disc texture seam
// FIXME: cylinder visual bug (look from above when close)
// FIXME: orthographic camera background
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include "lib/Camera/camera.h"
#include "lib/Sphere/sphere.h"
#include "lib/Light/light.h"
#include "lib/utils/utils.h"
#include "lib/image_utils/image_utils.h"
#include "lib/Input/input.h"
#include "lib/HollowDisk/hollowDisk.h"

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

// 0 for 2k, 1 for 8k
#define BACKGROUND_TEXTURE_QUALITY 0
#if BACKGROUND_TEXTURE_QUALITY == 0
#define BACKGROUND_TEXTURE_PATH "assets/textures/background/2k.jpg"
#elif BACKGROUND_TEXTURE_QUALITY == 1
#define BACKGROUND_TEXTURE_PATH "assets/textures/background/8k.jpg"
#endif

#pragma region shader utils
std::string loadShaderSource(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}
#pragma endregion

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
        "assets/textures/2k_earth_daymap.jpg",
        "assets/textures/2k_saturn_ring_alpha.png",
        "assets/textures/2k_earth_normal_map.jpg"
    };

    GLuint textureArrayID = loadTextureArray(texturePaths, shaderProgram);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textures"), 1);
#pragma endregion

#pragma region objects
    Camera cam(glm::vec3(0., 2., 15.), -glm::normalize(glm::vec3(0., 2., 15.)), glm::vec3(1., 0., 0.));

    Sphere sphere(glm::vec3(-10., 0., 0.));
    sphere.setMaterialColor(glm::vec4(1., 0., 0., 1.));
    sphere.setMaterialTextureIndex(0);
    sphere.setMaterialNormalMapIndex(2);

    HollowDisk accretionDisk;
    accretionDisk.setMaterialColor(glm::vec4(1., 0.5, 0.1, 1.));
    accretionDisk.setMaterialTextureIndex(1);

    Light light;

    glUseProgram(shaderProgram);

    cam.setupShader(shaderProgram);
    cam.loadShader();

    glUniform1i(glGetUniformLocation(shaderProgram, "num_lights"), 1);
    light.setupShader(shaderProgram, "lights[0]");
    light.loadShader();

    glUniform1i(glGetUniformLocation(shaderProgram, "num_spheres"), 1);
    sphere.setupShader(shaderProgram, "spheres[0]");
    sphere.loadShader();

    glUniform1i(glGetUniformLocation(shaderProgram, "num_hollow_disks"), 1);
    accretionDisk.setupShader(shaderProgram, "hollow_disks[0]");
    accretionDisk.loadShader();

    glUniform1i(glGetUniformLocation(shaderProgram, "num_objects"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[0].type"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[0].index"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[1].type"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[1].index"), 0);
#pragma endregion

#pragma region input
    Input* input = Input::getInstance();
    glfwSetKeyCallback(window, Input::keyCallback);
    glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
    glfwSetCursorPosCallback(window, Input::cursorPosCallback);
    glfwSetScrollCallback(window, Input::scrollCallback);
#pragma endregion

    double hyperbolicTrajectoryStartTime = -(1. + HYPERBOLIC_TRAJECTORY_DURATION);

    float speed = MOVE_SPEED;
    double prevTime = 0.;
    glm::vec2 prevMouse = input->getMouse();
    GLint flatRaytraceLoc = glGetUniformLocation(shaderProgram, "flat_raytrace");
    GLint flatPercentage = glGetUniformLocation(shaderProgram, "flat_percentage");
    int flatRaytrace = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (input->isPressed(GLFW_KEY_ESCAPE)) break;

        double windowTime = glfwGetTime();
        double dt = windowTime - prevTime;
        glm::vec2 mouse = input->getMouse();
        glm::vec2 deltaMouse = mouse - prevMouse;

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        int height, width;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        if (input->isPressed(GLFW_KEY_P)) {
            cam.setOrthographic(false);
            cam.loadShader();
        }
        else if (input->isPressed(GLFW_KEY_O)) {
            cam.setOrthographic(true);
            cam.loadShader();
        }

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
            if (cam.getOrthographic()) {
                float orthographicWidth = cam.getOrthographicWidth();
                orthographicWidth += ZOOM_SENSITIVITY * (float)dt * deltaMouse.y;
                if (orthographicWidth < 0.) orthographicWidth = 0.;
                cam.setOrthographicWidth(orthographicWidth);
            }
            else {
                float fov = cam.getFov();
                fov += ZOOM_SENSITIVITY * (float)dt * deltaMouse.y;
                if (fov < MIN_FOV) fov = MIN_FOV;
                if (fov > MAX_FOV) fov = MAX_FOV;
                cam.setFov(fov);
            }
        }
#pragma endregion

        if (input->isPressed(GLFW_KEY_F) && !input->isPressed(GLFW_KEY_F, GLFW_MOD_ALT) && !input->isPressed(GLFW_KEY_F, GLFW_MOD_CAPS_LOCK)) {
            if (cam.getOrthographic()) {
                cam.setOrthographicWidth(DEFAULT_ORTHOGRAPHIC_WIDTH);
            }
            else {
                cam.setFov(DEFAULT_FOV);
            }
        }

        double hyperbolicTrajectoryDt = windowTime - hyperbolicTrajectoryStartTime;
        if (input->isPressed(GLFW_KEY_H) && (hyperbolicTrajectoryDt < 0. || hyperbolicTrajectoryDt > HYPERBOLIC_TRAJECTORY_DURATION)) {
            hyperbolicTrajectoryStartTime = windowTime;
        }
        if (hyperbolicTrajectoryDt >= 0. && hyperbolicTrajectoryDt <= HYPERBOLIC_TRAJECTORY_DURATION) {
            cam.hyperbolicTrajectory(30.f, 10.f, (float)hyperbolicTrajectoryDt / HYPERBOLIC_TRAJECTORY_DURATION);
        }

        if (input->isPressed(GLFW_KEY_1)) flatRaytrace = 0;
        else if (input->isPressed(GLFW_KEY_2)) flatRaytrace = 1;
        else if (input->isPressed(GLFW_KEY_3)) flatRaytrace = 2;
        else if (input->isPressed(GLFW_KEY_4)) flatRaytrace = 3;
        glUniform1i(flatRaytraceLoc, flatRaytrace);

        if (input->isPressed(GLFW_KEY_LEFT_ALT) && input->isLClicked()) {
            if (flatRaytrace == 2) glUniform1f(flatPercentage, mouse.x / width);
            if (flatRaytrace == 3) glUniform1f(flatPercentage, 1. - mouse.y / height);
        }

        if (input->isPressed(GLFW_KEY_L)) cam.lookAt();

#pragma region uniforms
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), (float)glfwGetTime());
        glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), width, height);

        cam.loadShader();
#pragma endregion

        glfwSwapBuffers(window);
        prevTime = windowTime;
        prevMouse = mouse;
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
