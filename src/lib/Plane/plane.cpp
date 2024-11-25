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

void Plane::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_textureSizeLoc = glGetUniformLocation(program, (prefix + ".texture_size").c_str());
}

void Plane::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform2f(m_textureSizeLoc, m_textureSize.x, m_textureSize.y);
}
