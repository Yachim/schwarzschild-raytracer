// TODO: two sided normals
// TODO: loading
// TODO: disk, hollow disk classes
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const uint WIDTH = 1280;
const uint HEIGHT = 720;

// the higher, the more fields
// renders only half
// set < 0 to deactivate
const float CHECKERBOARD_DETAIL = 200.;

const float DOUBLE_CLICK_TRESHOLD = 0.5; // max time since last click in seconds
const float MOVE_SPEED = 5.;
const float SENSITIVITY = 100.;
const float ZOOM_SENSITIVITY = 5000.;

const float DEFAULT_FOV = 90.;
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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Schwarzschild Raytracer", NULL, NULL);
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
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(TEXTURE_PATH, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    #pragma endregion

    Camera cam(glm::vec3(0., 1., 15.));
    Sphere sphere(glm::vec3(-10., 0., 0.));
    Material sphereMat = sphere.getMaterial();
    sphereMat.setColor(glm::vec4(1., 0., 0., 1.));
    Light light{};

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "background_texture"), 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "num_lights"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].pos").c_str()), light.getPos().x, light.getPos().y, light.getPos().z);
    glUniform3f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].color").c_str()), light.getColor().x, light.getColor().y, light.getColor().z);
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].intensity").c_str()), light.getIntensity());
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].attenuation_constant").c_str()), light.getAttenuationConstant());
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].attenuation_linear").c_str()), light.getAttenuationLinear());
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].attenuation_quadratic").c_str()), light.getAttenuationQuadratic());

    glUniform1i(glGetUniformLocation(shaderProgram, "num_spheres"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.pos").c_str()), sphere.getPos().x, sphere.getPos().y, sphere.getPos().z);
    glUniform1i(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.opaque").c_str()), 1); // TODO:  
    glUniform4f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.material.color").c_str()), sphereMat.getColor().x, sphereMat.getColor().y, sphereMat.getColor().z, sphereMat.getColor().w);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.material.ambient").c_str()), sphereMat.getAmbient());
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.material.diffuse").c_str()), sphereMat.getDiffuse());
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.material.specular").c_str()), sphereMat.getSpecular());
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].base.material.shininess").c_str()), sphereMat.getShininess());
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(0) + "].radius").c_str()), sphere.getRadius());

    glUniform1i(glGetUniformLocation(shaderProgram, "num_hollow_disks"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.pos").c_str()), 0., 0., 0.);
    glUniform1i(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.opaque").c_str()), 1); // TODO:
    glUniform4f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.material.color").c_str()), 1., 0.5, 0.1, 1.);
    glUniform1f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.material.ambient").c_str()), sphereMat.getAmbient());
    glUniform1f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.material.diffuse").c_str()), sphereMat.getDiffuse());
    glUniform1f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.material.specular").c_str()), sphereMat.getSpecular());
    glUniform1f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.base.material.shininess").c_str()), sphereMat.getShininess());
    glUniform1f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].inner_radius").c_str()), 2.5);
    glUniform1f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].outer_radius").c_str()), 5.);
    glUniform3f(glGetUniformLocation(shaderProgram, ("hollow_disks[" + std::to_string(0) + "].plane.normal").c_str()), 0., 1., 0.);

    glUniform1i(glGetUniformLocation(shaderProgram, "num_objects"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, ("objects[" + std::to_string(0) + "].type").c_str()), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, ("objects[" + std::to_string(0) + "].index").c_str()), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, ("objects[" + std::to_string(1) + "].type").c_str()), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, ("objects[" + std::to_string(1) + "].index").c_str()), 0);

    glUniform1f(glGetUniformLocation(shaderProgram, "checkerboard_detail"), CHECKERBOARD_DETAIL);
    float fov = DEFAULT_FOV;

    double prevMouseX, prevMouseY;
    glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
    glm::vec2 prevMouse = glm::vec2(prevMouseX, prevMouseY);
    double prevLClickTime = -1.;
    double prevRClickTime = -1.;

    double prevTime = 0.;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double time = glfwGetTime();
        double dt = time - prevTime;

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, background_texture);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glUniform1f(glGetUniformLocation(shaderProgram, "time"), (float)glfwGetTime());
        glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), width, height);

        glm::vec3 camPos = cam.getPos();
        glm::vec3 camForward = cam.getForward();
        glm::vec3 camRight = cam.getRight();
        glm::vec3 camUp = cam.getUp();

        glUniform3f(glGetUniformLocation(shaderProgram, "cam_pos"), camPos.x, camPos.y, camPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "cam_forward"), camForward.x, camForward.y, camForward.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "cam_right"), camRight.x, camRight.y, camRight.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "cam_up"), camUp.x, camUp.y, camUp.z);

        #pragma region input
        int stateW = glfwGetKey(window, GLFW_KEY_W);
        int stateA = glfwGetKey(window, GLFW_KEY_A);
        int stateS = glfwGetKey(window, GLFW_KEY_S);
        int stateD = glfwGetKey(window, GLFW_KEY_D);
        int stateE = glfwGetKey(window, GLFW_KEY_E);
        int stateQ = glfwGetKey(window, GLFW_KEY_Q);
        int stateF = glfwGetKey(window, GLFW_KEY_F);

        glm::vec3 axis = glm::vec3(0., 0., 0.);
        if (stateW == GLFW_PRESS) {
            axis += camForward;
        }
        if (stateA == GLFW_PRESS) {
            axis -= camRight;
        }
        if (stateS == GLFW_PRESS) {
            axis -= camForward;
        }
        if (stateD == GLFW_PRESS) {
            axis += camRight;
        }
        if (stateE == GLFW_PRESS) {
            axis += camUp;
        }
        if (stateQ == GLFW_PRESS) {
            axis -= camUp;
        }
        if (stateF == GLFW_PRESS) {
            fov = DEFAULT_FOV;
        }
        float axisLength = glm::length(axis);
        if (axisLength > 0.) axis /= axisLength;
        camPos += axis * MOVE_SPEED * (float)dt;
        cam.setPos(camPos);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glm::vec2 mouse = glm::vec2(mouseX, mouseY);
        glm::vec2 deltaMouse = mouse - prevMouse;
        deltaMouse.x /= width;
        deltaMouse.y /= height;

        int stateMouseRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        int stateMouseLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (stateMouseRight == GLFW_PRESS) {
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
        else if (stateMouseLeft == GLFW_PRESS) {
            fov += ZOOM_SENSITIVITY * (float)dt * deltaMouse.y;
            if (fov < MIN_FOV) fov = MIN_FOV;
            if (fov > MAX_FOV) fov = MAX_FOV;
            glUniform1f(glGetUniformLocation(shaderProgram, "cam_fov"), fov);
        }

        #pragma endregion

        glfwSwapBuffers(window);
        prevTime = time;
        prevMouse = mouse;
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
