#include <glad/glad.h>
#include "curve.h"

Curve::Curve() : MaterialObject() {}

float Curve::getRadius() const {
    return m_radius;
}
void Curve::setRadius(float radius) {
    m_radius = radius;
}

std::vector<glm::vec3> Curve::getPoints() const {
    return m_points;
}
void Curve::setPoints(std::vector<glm::vec3> points) {
    m_points = points;
}

void Curve::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
        m_numPointsLoc = glGetUniformLocation(program, (prefix + ".num_points").c_str());
        m_locationsSet = true;
    }

    glUniform1f(m_radiusLoc, m_radius);
    glUniform1i(m_numPointsLoc, m_points.size());
    for (size_t i = 0; i < m_points.size(); i++) {
        glUniform3f(glGetUniformLocation(program, (prefix + ".points[" + std::to_string(i) + "]").c_str()), m_points[i].x, m_points[i].y, m_points[i].z);
    }
}

ObjectType Curve::getType() const {
    return ObjectType::CURVE;
}
