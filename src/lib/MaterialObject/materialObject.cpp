#include <glad/glad.h>
#include "materialObject.h"

MaterialObject::MaterialObject() {}
MaterialObject::MaterialObject(Material mat) : m_material(mat) {}

Material MaterialObject::getMaterial() {
    return m_material;
}
void MaterialObject::setMaterial(Material mat) {
    m_material = mat;
}

glm::vec4 MaterialObject::getMaterialColor() {
    return m_material.getColor();
}
void MaterialObject::setMaterialColor(glm::vec4 color) {
    m_material.setColor(color);
}

float MaterialObject::getMaterialAmbient() {
    return m_material.getAmbient();
}
void MaterialObject::setMaterialAmbient(float ambient) {
    m_material.setAmbient(ambient);
}

float MaterialObject::getMaterialDiffuse() {
    return m_material.getDiffuse();
}
void MaterialObject::setMaterialDiffuse(float diffuse) {
    m_material.setDiffuse(diffuse);
}

float MaterialObject::getMaterialSpecular() {
    return m_material.getSpecular();
}
void MaterialObject::setMaterialSpecular(float specular) {
    m_material.setSpecular(specular);
}

float MaterialObject::getMaterialShininess() {
    return m_material.getShininess();
}
void MaterialObject::setMaterialShininess(float shininess) {
    m_material.setShininess(shininess);
}

void MaterialObject::setupShader(GLuint program, std::string prefix) {
    Object::setupShader(program, prefix);

    m_materialColorLoc = glGetUniformLocation(program, (prefix + ".color").c_str());
    m_materialAmbientLoc = glGetUniformLocation(program, (prefix + ".ambient").c_str());
    m_materialDiffuseLoc = glGetUniformLocation(program, (prefix + ".diffuse").c_str());
    m_materialSpecularLoc = glGetUniformLocation(program, (prefix + ".specular").c_str());
    m_materialShininessLoc = glGetUniformLocation(program, (prefix + ".shininess").c_str());
}

void MaterialObject::loadShader() {
    Object::loadShader();

    glm::vec4 matColor = m_material.getColor();
    glUniform4f(m_materialColorLoc, matColor.x, matColor.y, matColor.z, matColor.w);
    glUniform1f(m_materialAmbientLoc, m_material.getAmbient());
    glUniform1f(m_materialDiffuseLoc, m_material.getDiffuse());
    glUniform1f(m_materialSpecularLoc, m_material.getSpecular());
    glUniform1f(m_materialShininessLoc, m_material.getShininess());
}
