#include "camera.h"
#include <glm/geometric.hpp>
#include <iostream>
#include "../utils/utils.h"

Camera::Camera(): Object() {}
Camera::Camera(glm::vec3 pos): Object(pos) {} 
Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 right): Object(pos) {
    m_forward = forward;
    m_right = right;
    calculateUp();
}

glm::vec3 Camera::getForward() {
    return m_forward;
}
void Camera::setForward(glm::vec3 newForward) {
    m_forward = newForward;
}

glm::vec3 Camera::getRight() {
    return m_right;
}
void Camera::setRight(glm::vec3 newRight) {
    m_right = newRight;
}

glm::vec3 Camera::getUp() {
    return m_up;
}
void Camera::setUp(glm::vec3 newUp) {
    m_up = newUp;
}

void Camera::calculateForward() {
    m_forward = glm::normalize(glm::cross(m_up, m_right));
}
void Camera::calculateRight() {
    m_forward = glm::normalize(glm::cross(m_forward, m_up));
}
void Camera::calculateUp() {
    m_forward = glm::normalize(glm::cross(m_right, m_forward));
}

void Camera::rotateAround(float angle, glm::vec3 point, glm::vec3 axis) {
    m_pos = rotateVector(angle, m_pos, point, axis);
    m_forward = rotateVector(angle, m_forward, point, axis);
    m_right = rotateVector(angle, m_right, point, axis);
    m_up = rotateVector(angle, m_up, point, axis);
}
