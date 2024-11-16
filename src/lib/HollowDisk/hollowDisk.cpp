#include <glad/glad.h>
#include "hollowDisk.h"

HollowDisk::HollowDisk() : Plane() {}
HollowDisk::HollowDisk(glm::vec3 pos) : Plane(pos) {}
HollowDisk::HollowDisk(glm::vec3 pos, glm::vec3 normal, Material mat) : Plane(pos, normal, mat) {}

float HollowDisk::getInnerRadius() {
    return m_innerRadius;
}
void HollowDisk::setInnerRadius(float innerRadius) {
    m_innerRadius = innerRadius;
}

float HollowDisk::getOuterRadius() {
    return m_outerRadius;
}
void HollowDisk::setOuterRadius(float outerRadius) {
    m_outerRadius = outerRadius;
}

void HollowDisk::setupShader(GLuint program, std::string prefix) {
    Plane::setupShader(program, prefix + ".plane");

    m_innerRadiusLoc = glGetUniformLocation(program, (prefix + ".inner_radius").c_str());
    m_outerRadiusLoc = glGetUniformLocation(program, (prefix + ".outer_radius").c_str());
}

void HollowDisk::loadShader() {
    Plane::loadShader();

    glUniform1f(m_innerRadiusLoc, m_innerRadius);
    glUniform1f(m_outerRadiusLoc, m_outerRadius);
}
