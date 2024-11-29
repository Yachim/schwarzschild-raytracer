#include "image_utils.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint loadTexture(const char* texturePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture, and generate mipmaps
    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the image on the y-axis
    unsigned char* data = stbi_load(texturePath, &textureWidth, &textureHeight, &nrChannels, 0);

    if (data) {
        // Determine the format based on the number of channels
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Create the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
    }

    stbi_image_free(data);

    return textureID;
}

GLuint loadTextureArray(const std::vector<std::string>& texturePaths, GLuint program) {
    GLuint textureID;
    glGenTextures(texturePaths.size(), &textureID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

    int maxWidth = 0, maxHeight = 0, maxChannels = 0;

    // First pass: determine maximum dimensions and channels
    for (const auto& path : texturePaths) {
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (data) {
            maxWidth = std::max(maxWidth, width);
            maxHeight = std::max(maxHeight, height);
            maxChannels = std::max(maxChannels, channels);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
    }
    glUniform2f(glGetUniformLocation(program, "max_texture_size"), maxWidth, maxHeight);

    if (maxWidth == 0 || maxHeight == 0 || maxChannels == 0) {
        std::cerr << "Error: No valid textures loaded!" << std::endl;
        return 0;
    }

    GLenum internalFormat = (maxChannels == 4) ? GL_RGBA : GL_RGB;

    // Allocate storage for the texture array
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, maxWidth, maxHeight, texturePaths.size(), 0, internalFormat, GL_UNSIGNED_BYTE, nullptr);

    glUniform1i(glGetUniformLocation(program, "num_textures"), texturePaths.size());
    // Second pass: load and resize textures
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        int width, height, channels;
        unsigned char* data = stbi_load(texturePaths[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            // Resize the texture to match the largest dimensions if necessary
            std::vector<unsigned char> resizedData(maxWidth * maxHeight * maxChannels, 0);

            for (int y = 0; y < maxHeight; ++y) {
                for (int x = 0; x < maxWidth; ++x) {
                    int dstIndex = (y * maxWidth + x) * maxChannels;
                    if (x < width && y < height) {
                        int srcIndex = (y * width + x) * channels;
                        for (int c = 0; c < channels; ++c) {
                            resizedData[dstIndex + c] = data[srcIndex + c];
                        }
                        // Fill missing channels with default value (255 for alpha)
                        for (int c = channels; c < maxChannels; ++c) {
                            resizedData[dstIndex + c] = (c == 3) ? 255 : 0;
                        }
                    }
                }
            }

            GLenum format = (maxChannels == 4) ? GL_RGBA : GL_RGB;
            glUniform2f(glGetUniformLocation(program, ("texture_sizes[" + std::to_string(i) + "]").c_str()), width, height);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, maxWidth, maxHeight, 1, format, GL_UNSIGNED_BYTE, resizedData.data());
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load texture: " << texturePaths[i] << std::endl;
        }
    }

    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}