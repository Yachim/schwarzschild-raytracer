#include <glad/glad.h>
#include "disk.h"

Disk::Disk() : Plane() {}
Disk::Disk(glm::vec3 pos) : Plane(pos) {}
Disk::Disk(glm::vec3 pos, glm::vec3 normal, Material mat) : Plane(pos, normal, mat) {}

float Disk::getRadius() {
    return m_radius;
}

void Disk::setRadius(float radius) {
    m_radius = radius;
}

void Disk::setupShader(GLuint program, std::string prefix) {
    Plane::setupShader(program, prefix + ".plane");

    m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
}

void Disk::loadShader() {
    Plane::loadShader();

    glUniform1f(m_radiusLoc, m_radius);
}
