#include <glad/glad.h>
#include "lateralCylinder.h"
#include <vector>

LateralCylinder::LateralCylinder() : MaterialObject(), Transform() {}

float LateralCylinder::getHeight() const {
    return m_height;
}
void LateralCylinder::setHeight(float height) {
    m_height = height;
}

float LateralCylinder::getRadius() const {
    return m_radius;
}
void LateralCylinder::setRadius(float radius) {
    m_radius = radius;
}

void LateralCylinder::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_heightLoc = glGetUniformLocation(program, (prefix + ".height").c_str());
        m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
        m_locationsSet = true;
    }

    MaterialObject::loadShader(program, prefix + ".material");
    Transform::loadShader(program, prefix + ".transform");

    glUniform1f(m_heightLoc, m_height);
    glUniform1f(m_radiusLoc, m_radius);
}

ObjectType LateralCylinder::getType() const {
    return ObjectType::LATERAL_CYLINDER;
}
