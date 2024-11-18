// TODO: loading
// TODO: disk, hollow disk classes
// TODO: clocks
// FIXME: lateral cylinder casting shadow (e.g. on accretion disk) from below
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include "lib/Camera/camera.h"
#include "lib/Sphere/sphere.h"
#include "lib/Material/material.h"
#include "lib/Light/light.h"
#include "lib/utils/utils.h"
#include "lib/Input/input.h"
#include "lib/HollowDisk/hollowDisk.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const uint DEFAULT_WIDTH = 1280;
const uint DEFAULT_HEIGHT = 720;

// the higher, the more fields
// renders only half
// set <= 0 to deactivate
const float CHECKERBOARD_DETAIL = 0.;

const float MOVE_SPEED_INCREASE_SPEED = 0.25;
const float MOVE_SPEED = 5.;
const float SENSITIVITY = 0.5;
const float ZOOM_SENSITIVITY = 10.;

const float MIN_FOV = 10.;
const float MAX_FOV = 120.;

// 0 for 2k, 1 for 8k
#define TEXTURE_QUALITY 0
#if TEXTURE_QUALITY == 0
#define TEXTURE_PATH "assets/textures/background_2k.jpg"
#elif TEXTURE_QUALITY == 1
#define TEXTURE_PATH "assets/textures/background_8k.jpg"
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
    unsigned int background_texture;
    glGenTextures(1, &background_texture);
    glBindTexture(GL_TEXTURE_2D, background_texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(TEXTURE_PATH, &textureWidth, &textureHeight, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
#pragma endregion

    Camera cam(glm::vec3(0., 2., 15.), -glm::normalize(glm::vec3(0., 2., 15.)), glm::vec3(1., 0., 0.));

    Sphere sphere(glm::vec3(-10., 0., 0.));
    sphere.setMaterialColor(glm::vec4(1., 0., 0., 1.));

    HollowDisk accretionDisk;
    accretionDisk.setMaterialColor(glm::vec4(1., 0.5, 0.1, 1.));

    Light light{};

#pragma region uniforms
    glUseProgram(shaderProgram);

    cam.setupShader(shaderProgram);
    cam.loadShader();

    glUniform1i(glGetUniformLocation(shaderProgram, "background_texture"), 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "num_lights"), 1);
    light.setupShader(shaderProgram, "lights[0]");
    light.loadShader();

    glUniform1i(glGetUniformLocation(shaderProgram, "num_spheres"), 1);
    sphere.setupShader(shaderProgram, "spheres[0]");
    sphere.loadShader();
    glUniform1i(glGetUniformLocation(shaderProgram, "spheres[0].material.opaque"), 1); // TODO:  

    glUniform1i(glGetUniformLocation(shaderProgram, "num_hollow_disks"), 1);
    accretionDisk.setupShader(shaderProgram, "hollow_disks[0]");
    accretionDisk.loadShader();
    glUniform1i(glGetUniformLocation(shaderProgram, "hollow_disks[0].plane.material.opaque"), 1); // TODO:  

    glUniform1i(glGetUniformLocation(shaderProgram, "num_objects"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[0].type"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[0].index"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[1].type"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "objects[1].index"), 0);

    glUniform1f(glGetUniformLocation(shaderProgram, "checkerboard_detail"), CHECKERBOARD_DETAIL);
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, background_texture);

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
            // rotation x-axis
            camForward = rotateVector(-SENSITIVITY * (float)dt * deltaMouse.x, camForward, glm::vec3(0., 1., 0.));
            camRight = rotateVector(-SENSITIVITY * (float)dt * deltaMouse.x, camRight, glm::vec3(0., 1., 0.));
            camUp = rotateVector(-SENSITIVITY * (float)dt * deltaMouse.x, camUp, glm::vec3(0., 1., 0.));

            // rotation y-axis FIXME: clamp this
            camForward = rotateVector(-SENSITIVITY * (float)dt * deltaMouse.y, camForward, camRight);
            camUp = rotateVector(-SENSITIVITY * (float)dt * deltaMouse.y, camUp, camRight);

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

        if (input->isPressed(GLFW_KEY_F) && !input->isPressed(GLFW_KEY_F, GLFW_MOD_ALT) && !input->isPressed(GLFW_KEY_F, GLFW_MOD_CAPS_LOCK)) cam.setFov(DEFAULT_FOV);

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
