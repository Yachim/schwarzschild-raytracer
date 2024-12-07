#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <string>
#include <GLFW/glfw3.h>

std::string loadShaderSource(const char* filePath);
GLuint compileShader(GLenum type, const char* source);

#endif