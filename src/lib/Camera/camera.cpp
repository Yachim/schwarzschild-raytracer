#include <glad/glad.h>
#include "camera.h"
#include <glm/geometric.hpp>
#include <iostream>
#include "../utils/utils.h"

Camera::Camera() : Transform() {}
Camera::Camera(glm::vec3 pos) : Transform(pos) {}
Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 right) : Transform(pos) {
    m_forward = glm::normalize(forward);
    m_right = glm::normalize(right);
    calculateUp();
}

glm::vec3 Camera::getForward() {
    return m_forward;
}
void Camera::setForward(glm::vec3 newForward) {
    m_forward = glm::normalize(newForward);
}

glm::vec3 Camera::getRight() {
    return m_right;
}
void Camera::setRight(glm::vec3 newRight) {
    m_right = glm::normalize(newRight);
}

glm::vec3 Camera::getUp() {
    return m_up;
}
void Camera::setUp(glm::vec3 newUp) {
    m_up = glm::normalize(newUp);
}

void Camera::calculateForward() {
    m_forward = glm::normalize(glm::cross(m_up, m_right));
}
void Camera::calculateRight() {
    m_right = glm::normalize(glm::cross(m_forward, m_up));
}
void Camera::calculateUp() {
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Camera::setFov(float fov) {
    m_fov = fov;
}
float Camera::getFov() {
    return m_fov;
}

void Camera::setupShader(GLuint program) {
    Transform::setupShader(program, "cam");

    m_forwardLoc = glGetUniformLocation(program, "cam.forward");
    m_rightLoc = glGetUniformLocation(program, "cam.right");
    m_upLoc = glGetUniformLocation(program, "cam.up");
    m_fovLoc = glGetUniformLocation(program, "cam.fov");
}

void Camera::loadShader() {
    Transform::loadShader();

    glUniform3f(m_forwardLoc, m_forward.x, m_forward.y, m_forward.z);
    glUniform3f(m_rightLoc, m_right.x, m_right.y, m_right.z);
    glUniform3f(m_upLoc, m_up.x, m_up.y, m_up.z);
    glUniform1f(m_fovLoc, m_fov);
}
