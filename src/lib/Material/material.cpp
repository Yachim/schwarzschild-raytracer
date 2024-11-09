#include "material.h"

Material::Material() {}
Material::Material(glm::vec4 color): m_color(color) {}
Material::Material(glm::vec4 color, float ambient, float diffuse, float specular, float shininess):
    m_color(color), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_shininess(m_shininess) {}

glm::vec4 Material::getColor() {
    return m_color;
}
void Material::setColor(glm::vec4 color) {
    m_color = color;
}

float Material::getAmbient() {
    return m_ambient;
}
void Material::setAmbient(float ambient) {
    m_ambient = ambient;
}

float Material::getDiffuse() {
    return m_diffuse;
}
void Material::setDiffuse(float diffuse) {
    m_diffuse = diffuse;
}

float Material::getSpecular() {
    return m_specular;
}
void Material::setSpecular(float specular) {
    m_specular = specular;
}

float Material::getShininess() {
    return m_shininess;
}
void Material::setShininess(float shininess) {
    m_shininess = shininess;
}
