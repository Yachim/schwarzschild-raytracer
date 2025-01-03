#include <glad/glad.h>
#include "sphere.h"

Sphere::Sphere() : MaterialObject(), Transform() {}
Sphere::Sphere(glm::vec3 pos) : MaterialObject(), Transform(pos) {}
Sphere::Sphere(glm::vec3 pos, float radius) : Transform(pos), MaterialObject(), m_radius(radius) {}
Sphere::Sphere(glm::vec3 pos, float radius, Material mat) : Transform(pos), MaterialObject(mat), m_radius(radius) {}

float Sphere::getRadius() const {
    return m_radius;
}
void Sphere::setRadius(float radius) {
    m_radius = radius;
}

void Sphere::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_radiusLoc = glGetUniformLocation(program, (prefix + ".radius").c_str());
        m_locationsSet = true;
    }

    Transform::loadShader(program, prefix + ".transform");
    MaterialObject::loadShader(program, prefix + ".material");


    glUniform1f(m_radiusLoc, m_radius);
}

ObjectType Sphere::getType() const {
    return ObjectType::SPHERE;
}
