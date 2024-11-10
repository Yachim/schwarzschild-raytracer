#include "camera.h"
#include <glm/geometric.hpp>
#include <iostream>
#include "../utils/utils.h"

Camera::Camera(): Object() {}
Camera::Camera(glm::vec3 pos): Object(pos) {} 
Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 right): Object(pos) {
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
