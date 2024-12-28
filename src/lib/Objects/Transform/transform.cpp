#include <glad/glad.h>
#include "transform.h"
#include <glm/gtx/quaternion.hpp>

Transform::Transform() {}

Transform::Transform(glm::vec3 pos) : m_pos(pos) {}

glm::vec3 Transform::getPos() {
    return m_pos;
}
void Transform::setPos(glm::vec3 newPos) {
    m_pos = newPos;
}

glm::mat3 Transform::getAxes() {
    return m_axes;
}
void Transform::setAxes(glm::mat3 newAxes) {
    m_axes = newAxes;
}

void Transform::setAxes(glm::quat rot) {
    m_axes = glm::toMat3(rot);
}

glm::vec3 Transform::getForward() {
    return m_axes[2];
}
void Transform::setForward(glm::vec3 newForward) {
    m_axes[2] = newForward;
}

glm::vec3 Transform::getRight() {
    return m_axes[0];
}
void Transform::setRight(glm::vec3 newRight) {
    m_axes[0] = newRight;
}

glm::vec3 Transform::getUp() {
    return m_axes[1];
}
void Transform::setUp(glm::vec3 newUp) {
    m_axes[1] = newUp;
}

void Transform::calculateForward() {
    m_axes[2] = glm::normalize(glm::cross(m_axes[0], m_axes[1]));
}
void Transform::calculateRight() {
    m_axes[0] = glm::normalize(glm::cross(m_axes[1], m_axes[2]));
}
void Transform::calculateUp() {
    m_axes[1] = glm::normalize(glm::cross(m_axes[2], m_axes[0]));
}

void Transform::setupShader(GLuint program, std::string prefix) {
    Object::setupShader(program, prefix);

    m_posLoc = glGetUniformLocation(program, (prefix + ".pos").c_str());
    m_axesLoc = glGetUniformLocation(program, (prefix + ".axes").c_str());
}
void Transform::loadShader() {
    Object::loadShader();

    glUniform3f(m_posLoc, m_pos.x, m_pos.y, m_pos.z);
    glUniformMatrix3fv(m_axesLoc, 1, GL_FALSE, &m_axes[0].x);
}
