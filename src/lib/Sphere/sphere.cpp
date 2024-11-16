#include <glad/glad.h>
#include "sphere.h"

Sphere::Sphere() : MaterialObject(), Transform() {}
Sphere::Sphere(glm::vec3 pos) : MaterialObject(), Transform(pos) {}
Sphere::Sphere(glm::vec3 pos, float radius) : Transform(pos), MaterialObject(), m_radius(radius) {}
Sphere::Sphere(glm::vec3 pos, float radius, Material mat) : Transform(pos), MaterialObject(mat), m_radius(radius) {}

float Sphere::getRadius() {
    return m_radius;
}
void Sphere::setRadius(float radius) {
    m_radius = radius;
}

void Sphere::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
}

void Sphere::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform1f(m_radiusLoc, m_radius);
}
