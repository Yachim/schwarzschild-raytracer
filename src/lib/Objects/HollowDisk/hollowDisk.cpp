#include <glad/glad.h>
#include "hollowDisk.h"

HollowDisk::HollowDisk() : Plane() {}
HollowDisk::HollowDisk(glm::vec3 pos) : Plane(pos) {}

float HollowDisk::getInnerRadius() const {
    return m_innerRadius;
}
void HollowDisk::setInnerRadius(float innerRadius) {
    m_innerRadius = innerRadius;
}

float HollowDisk::getOuterRadius() const {
    return m_outerRadius;
}
void HollowDisk::setOuterRadius(float outerRadius) {
    m_outerRadius = outerRadius;
}

void HollowDisk::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_innerRadiusLoc = glGetUniformLocation(program, (prefix + ".inner_radius").c_str());
        m_outerRadiusLoc = glGetUniformLocation(program, (prefix + ".outer_radius").c_str());
        m_locationsSet = true;
    }

    Plane::loadShader(program, prefix + ".plane");

    glUniform1f(m_innerRadiusLoc, m_innerRadius);
    glUniform1f(m_outerRadiusLoc, m_outerRadius);
}

ObjectType HollowDisk::getType() const {
    return ObjectType::HOLLOW_DISK;
}
