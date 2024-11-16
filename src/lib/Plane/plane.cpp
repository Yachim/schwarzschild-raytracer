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

void Plane::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_normalLoc = glGetUniformLocation(program, (prefix + ".normal").c_str());
}

void Plane::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform3f(m_normalLoc, m_normal.x, m_normal.y, m_normal.z);
}
