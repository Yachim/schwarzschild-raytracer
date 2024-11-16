#include <glad/glad.h>
#include "transform.h"

Transform::Transform() {}

Transform::Transform(glm::vec3 pos) : m_pos(pos) {}

glm::vec3 Transform::getPos() {
    return m_pos;
}

void Transform::setPos(glm::vec3 newPos) {
    m_pos = newPos;
}

void Transform::setupShader(GLuint program, std::string prefix) {
    Object::setupShader(program, prefix);

    m_posLoc = glGetUniformLocation(program, (prefix + ".transform.pos").c_str());
}
void Transform::loadShader() {
    Object::loadShader();

    glUniform3f(m_posLoc, m_pos.x, m_pos.y, m_pos.z);
}
