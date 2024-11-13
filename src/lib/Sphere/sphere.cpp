#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "sphere.h"
#include <iostream>

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

void Sphere::setupShader(GLuint program, int i) {
    m_posLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.pos").c_str());
    m_opaqueLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.opaque").c_str());
    m_materialColorLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.color").c_str());
    m_materialAmbientLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.ambient").c_str());
    m_materialDiffuseLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.diffuse").c_str());
    m_materialSpecularLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.specular").c_str());
    m_materialShininessLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.shininess").c_str());
    m_radiusLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].radius").c_str());
}

void Sphere::loadShader() {
    glUniform3f(m_posLoc, m_pos.x, m_pos.y, m_pos.z);
    glUniform1i(m_opaqueLoc, 1); // TODO:  
    glUniform4f(m_materialColorLoc, m_material.getColor().x, m_material.getColor().y, m_material.getColor().z, m_material.getColor().w);
    glUniform1f(m_materialAmbientLoc, m_material.getAmbient());
    glUniform1f(m_materialDiffuseLoc, m_material.getDiffuse());
    glUniform1f(m_materialSpecularLoc, m_material.getSpecular());
    glUniform1f(m_materialShininessLoc, m_material.getShininess());
    glUniform1f(m_radiusLoc, m_radius);
}
