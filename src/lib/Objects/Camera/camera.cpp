#include <glad/glad.h>
#include "camera.h"
#include "../../utils/utils.h"

Camera::Camera() : Transform() {}
Camera::Camera(glm::vec3 pos) : Transform(pos) {}
Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 right) : Transform(pos) {
    m_axes[0] = glm::normalize(right);
    m_axes[2] = glm::normalize(forward);
    m_axes[1] = glm::normalize(glm::cross(right, forward));
}

void Camera::setFov(float fov) {
    m_fov = fov;
}
float Camera::getFov() const {
    return m_fov;
}

void Camera::lookAt(glm::vec3 point) {
    m_axes[2] = glm::normalize(point - m_pos);
    m_axes[0] = glm::normalize(glm::cross(m_axes[2], glm::vec3(0., 1., 0.)));
    m_axes[1] = glm::normalize(glm::cross(m_axes[0], m_axes[2]));
}

void Camera::loadShader(GLuint program) {
    if (!m_locationsSet) {
        m_fovLoc = glGetUniformLocation(program, "cam.fov");
        m_locationsSet = true;
    }

    Transform::loadShader(program, "cam.transform");

    glUniform1f(m_fovLoc, m_fov);
}

ObjectType Camera::getType() const {
    return ObjectType::CAMERA;
}
