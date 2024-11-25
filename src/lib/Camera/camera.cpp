#include <glad/glad.h>
#include "camera.h"
#include "../utils/utils.h"

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
float Camera::getFov() {
    return m_fov;
}

void Camera::hyperbolicTrajectory(float initialDistance, float closestDistance, float time) {
    float closestDistanceSquared = pow(closestDistance, 2.);
    float a = -closestDistanceSquared / (-initialDistance + 2 * closestDistance);
    float c = closestDistance + a;
    float b = sqrt(closestDistanceSquared + 2. * a * closestDistance);

    float easedTime = (1. - cos(time * M_PI)) / 2.;

    float x = -initialDistance + 2. * easedTime * initialDistance;
    float y = c - a * sqrt(1 + pow(x / b, 2.));

    m_pos = x * HYPERBOLIC_TRAJECTORY_BASE_X + y * HYPERBOLIC_TRAJECTORY_BASE_Y;
    lookAt();
}

void Camera::lookAt(glm::vec3 point) {
    m_axes[2] = glm::normalize(point - m_pos);
    m_axes[0] = glm::normalize(glm::cross(m_axes[2], glm::vec3(0., 1., 0.)));
    m_axes[1] = glm::normalize(glm::cross(m_axes[0], m_axes[2]));
}

void Camera::setupShader(GLuint program) {
    Transform::setupShader(program, "cam.transform");

    m_fovLoc = glGetUniformLocation(program, "cam.fov");
}

void Camera::loadShader() {
    Transform::loadShader();

    glUniform1f(m_fovLoc, m_fov);
}
