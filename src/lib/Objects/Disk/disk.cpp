#include <glad/glad.h>
#include "disk.h"

Disk::Disk() : Plane() {}
Disk::Disk(glm::vec3 pos) : Plane(pos) {}

float Disk::getRadius() const {
    return m_radius;
}

void Disk::setRadius(float radius) {
    m_radius = radius;
}

void Disk::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
        m_locationsSet = true;
    }

    Plane::loadShader(program, prefix + ".plane");

    glUniform1f(m_radiusLoc, m_radius);
}

ObjectType Disk::getType() const {
    return ObjectType::DISK;
}
