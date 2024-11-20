#include <glad/glad.h>
#include "plane.h"

Plane::Plane() : MaterialObject(), Transform() {}
Plane::Plane(glm::vec3 pos) : MaterialObject(), Transform(pos) {}
Plane::Plane(glm::vec3 pos, glm::vec3 normal) : Transform(pos), MaterialObject(), m_normal(normal) {}
Plane::Plane(glm::vec3 pos, glm::vec3 normal, Material mat) : Transform(pos), MaterialObject(mat), m_normal(normal) {}

glm::vec3 Plane::getNormal() {
    return m_normal;
}
void Plane::setNormal(glm::vec3 normal) {
    m_normal = normal;
}

glm::vec2 Plane::getTextureSize() {
    return m_textureSize;
}
void Plane::setTextureSize(glm::vec2 size) {
    m_textureSize = size;
}

void Plane::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_normalLoc = glGetUniformLocation(program, (prefix + ".normal").c_str());
    m_textureSizeLoc = glGetUniformLocation(program, (prefix + ".texture_size").c_str());
}

void Plane::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform3f(m_normalLoc, m_normal.x, m_normal.y, m_normal.z);
    glUniform2f(m_textureSizeLoc, m_textureSize.x, m_textureSize.y);
}
