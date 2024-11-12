#include "sphere.h"

Sphere::Sphere() : Object() {}
Sphere::Sphere(glm::vec3 pos) : Object(pos) {}
Sphere::Sphere(glm::vec3 pos, float radius) : Object(pos), m_radius(radius) {}
Sphere::Sphere(glm::vec3 pos, float radius, Material mat) : Object(pos), m_radius(radius), m_material(mat) {}

float Sphere::getRadius() {
    return m_radius;
}
void Sphere::setRadius(float radius) {
    m_radius = radius;
}

Material Sphere::getMaterial() {
    return m_material;
}
void Sphere::setMaterial(Material mat) {
    m_material = mat;
}
