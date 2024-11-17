#include <glad/glad.h>
#include "lateralCylinder.h"

LateralCylinder::LateralCylinder() : MaterialObject(), Transform() {}

glm::vec3 LateralCylinder::getHeight() {
    return m_height;
}
void LateralCylinder::setHeight(glm::vec3 height) {
    m_height = height;
}

float LateralCylinder::getRadius() {
    return m_radius;
}
void LateralCylinder::setRadius(float radius) {
    m_radius = radius;
}

void LateralCylinder::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_heightLoc = glGetUniformLocation(program, (prefix + ".height").c_str());
    m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
}

void LateralCylinder::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform3f(m_heightLoc, m_height.x, m_height.y, m_height.z);
    glUniform1f(m_radiusLoc, m_radius);
}
