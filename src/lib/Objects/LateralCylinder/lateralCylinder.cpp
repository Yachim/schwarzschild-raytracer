#include <glad/glad.h>
#include "lateralCylinder.h"

LateralCylinder::LateralCylinder() : MaterialObject(), Transform() {}

float LateralCylinder::getHeight() {
    return m_height;
}
void LateralCylinder::setHeight(float height) {
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

    glUniform1f(m_heightLoc, m_height);
    glUniform1f(m_radiusLoc, m_radius);
}
