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

int MaterialObject::getMaterialTextureIndex() {
    return m_material.getTextureIndex();
}
void MaterialObject::setMaterialTextureIndex(int index) {
    m_material.setTextureIndex(index);
}

int MaterialObject::getMaterialNormalMapIndex() {
    return m_material.getNormalMapIndex();
}
void MaterialObject::setMaterialNormalMapIndex(int index) {
    m_material.setNormalMapIndex(index);
}

bool MaterialObject::getMaterialInvertUvX() {
    return m_material.getInvertUvX();
}
void MaterialObject::setMaterialInvertUvX(bool invertUvX) {
    m_material.setInvertUvX(invertUvX);
}

bool MaterialObject::getMaterialInvertUvY() {
    return m_material.getInvertUvY();
}
void MaterialObject::setMaterialInvertUvY(bool invertUvY) {
    m_material.setInvertUvY(invertUvY);
}

bool MaterialObject::getMaterialSwapUvs() {
    return m_material.getSwapUvs();
}
void MaterialObject::setMaterialSwapUvs(bool swapUvs) {
    m_material.setSwapUvs(swapUvs);
}

bool MaterialObject::getMaterialDoubleSidedNormals() {
    return m_material.getDoubleSidedNormals();
}
void MaterialObject::setMaterialDoubleSidedNormals(bool doubleSidedNormals) {
    m_material.setDoubleSidedNormals(doubleSidedNormals);
}

bool MaterialObject::getMaterialFlipNormals() {
    return m_material.getFlipNormals();
}
void MaterialObject::setMaterialFlipNormals(bool flipNormals) {
    m_material.setFlipNormals(flipNormals);
}


void MaterialObject::setupShader(GLuint program, std::string prefix) {
    Object::setupShader(program, prefix);

    m_materialColorLoc = glGetUniformLocation(program, (prefix + ".color").c_str());
    m_materialAmbientLoc = glGetUniformLocation(program, (prefix + ".ambient").c_str());
    m_materialDiffuseLoc = glGetUniformLocation(program, (prefix + ".diffuse").c_str());
    m_materialSpecularLoc = glGetUniformLocation(program, (prefix + ".specular").c_str());
    m_materialShininessLoc = glGetUniformLocation(program, (prefix + ".shininess").c_str());
    m_materialTextureIndexLoc = glGetUniformLocation(program, (prefix + ".texture_index").c_str());
    m_materialNormalMapIndexLoc = glGetUniformLocation(program, (prefix + ".normal_map_index").c_str());
    m_materialInvertUvXLoc = glGetUniformLocation(program, (prefix + ".invert_uv_x").c_str());
    m_materialInvertUvYLoc = glGetUniformLocation(program, (prefix + ".invert_uv_y").c_str());
    m_materialSwapUvsLoc = glGetUniformLocation(program, (prefix + ".swap_uvs").c_str());
    m_materialDoubleSidedNormalsLoc = glGetUniformLocation(program, (prefix + ".double_sided_normals").c_str());
    m_materialFlipNormalsLoc = glGetUniformLocation(program, (prefix + ".flip_normals").c_str());
}

void MaterialObject::loadShader() {
    Object::loadShader();

    glm::vec4 matColor = m_material.getColor();
    glUniform4f(m_materialColorLoc, matColor.x, matColor.y, matColor.z, matColor.w);
    glUniform1f(m_materialAmbientLoc, m_material.getAmbient());
    glUniform1f(m_materialDiffuseLoc, m_material.getDiffuse());
    glUniform1f(m_materialSpecularLoc, m_material.getSpecular());
    glUniform1f(m_materialShininessLoc, m_material.getShininess());
    glUniform1i(m_materialTextureIndexLoc, m_material.getTextureIndex());
    glUniform1i(m_materialNormalMapIndexLoc, m_material.getNormalMapIndex());
    glUniform1i(m_materialInvertUvXLoc, m_material.getInvertUvX());
    glUniform1i(m_materialInvertUvYLoc, m_material.getInvertUvY());
    glUniform1i(m_materialSwapUvsLoc, m_material.getSwapUvs());
    glUniform1i(m_materialDoubleSidedNormalsLoc, m_material.getDoubleSidedNormals());
    glUniform1i(m_materialFlipNormalsLoc, m_material.getFlipNormals());
}
