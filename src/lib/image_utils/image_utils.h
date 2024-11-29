#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <glad/glad.h>
#include <vector>
#include <string>

GLuint loadTexture(const char* texturePath);
GLuint loadTextureArray(const std::vector<std::string>& texturePaths, GLuint program);

#endif