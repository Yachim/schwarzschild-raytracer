#include <glad/glad.h>
#include "plane.h"

Plane::Plane() : MaterialObject(), Transform() {}
Plane::Plane(glm::vec3 pos) : MaterialObject(), Transform(pos) {}

glm::vec2 Plane::getTextureSize() {
    return m_textureSize;
}
void Plane::setTextureSize(glm::vec2 size) {
    m_textureSize = size;
}

glm::vec2 Plane::getTextureOffset() {
    return m_textureOffset;
}
void Plane::setTextureOffset(glm::vec2 offset) {
    m_textureOffset = offset;
}

bool Plane::getRepeatTexture() {
    return m_repeatTexture;
}
void Plane::setRepeatTexture(bool repeatTexture) {
    m_repeatTexture = repeatTexture;
}

void Plane::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_textureSizeLoc = glGetUniformLocation(program, (prefix + ".texture_size").c_str());
    m_textureOffsetLoc = glGetUniformLocation(program, (prefix + ".texture_offset").c_str());
    m_repeatTextureLoc = glGetUniformLocation(program, (prefix + ".repeat_texture").c_str());
}

void Plane::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform2f(m_textureSizeLoc, m_textureSize.x, m_textureSize.y);
    glUniform2f(m_textureOffsetLoc, m_textureOffset.x, m_textureOffset.y);
    glUniform1i(m_repeatTextureLoc, m_repeatTexture);
}
