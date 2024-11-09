#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/vec3.hpp>
#include "lib/Camera/camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const uint WIDTH = 1280;
const uint HEIGHT = 720;

const float orbitingSpeed = 0.5;

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
    unsigned char *data = stbi_load("assets/textures/background.jpg", &width, &height, &nrChannels, 0);
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

    Camera cam(glm::vec3(0.f, 0.f, 15.f));

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "background_texture"), 0);

    // Setting up lights
    glUniform1i(glGetUniformLocation(shaderProgram, "num_lights"), 1);
    // For each light (i from 0 to num_lights-1):
    glUniform3f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].position").c_str()), 10.0f, 10.0f, 10.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].color").c_str()), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].intensity").c_str()), 2.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].attenuation_constant").c_str()), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].attenuation_linear").c_str()), 0.09f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + std::to_string(0) + "].attenuation_quadratic").c_str()), 0.032f);

    // Setting up additional spheres (not counting black hole)
    glUniform1i(glGetUniformLocation(shaderProgram, "num_spheres"), 2);  // For 2 additional spheres

    // Green sphere
    int i = 0;
    glUniform3f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].center").c_str()), 0.0f, 0.0f, 10.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].radius").c_str()), 1.0f);
    glUniform1i(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].opaque").c_str()), 1);
    glUniform4f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.color").c_str()), 0.0f, 1.0f, 0.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.ambient").c_str()), 0.1f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.diffuse").c_str()), 0.9f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.specular").c_str()), 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.shininess").c_str()), 32.0f);

    // Blue sphere
    i = 1;
    glUniform3f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].center").c_str()), 7.5f, 0.0f, 7.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].radius").c_str()), 2.0f);
    glUniform1i(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].opaque").c_str()), 1);
    glUniform4f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.color").c_str()), 0.0f, 0.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.ambient").c_str()), 0.1f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.diffuse").c_str()), 0.9f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.specular").c_str()), 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, ("spheres[" + std::to_string(i) + "].material.shininess").c_str()), 32.0f);

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

        cam.rotateAround(orbitingSpeed * dt);
        glm::vec3 camPos = cam.getPos();
        glm::vec3 camForward = cam.getForward();
        glm::vec3 camRight = cam.getRight();
        glm::vec3 camUp = cam.getUp();

        glUniform3f(glGetUniformLocation(shaderProgram, "cam_pos"), camPos.x, camPos.y, camPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "cam_forward"), camForward.x, camForward.y, camForward.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "cam_right"), camRight.x, camRight.y, camRight.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "cam_up"), camUp.x, camUp.y, camUp.z);

        glfwSwapBuffers(window);
        prevTime = time;
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
