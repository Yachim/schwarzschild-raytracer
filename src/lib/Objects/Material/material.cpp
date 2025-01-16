#include <glad/glad.h>
#include "material.h"

Material::Material() {}
Material::Material(glm::vec4 color) : m_color(color) {}
Material::Material(glm::vec4 color, float ambient, float diffuse, float specular, float shininess) :
    m_color(color), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_shininess(m_shininess) {}

glm::vec4 Material::getColor() const {
    return m_color;
}
void Material::setColor(glm::vec4 color) {
    m_color = color;
}

float Material::getAmbient() const {
    return m_ambient;
}
void Material::setAmbient(float ambient) {
    m_ambient = ambient;
}

float Material::getDiffuse() const {
    return m_diffuse;
}
void Material::setDiffuse(float diffuse) {
    m_diffuse = diffuse;
}

float Material::getSpecular() const {
    return m_specular;
}
void Material::setSpecular(float specular) {
    m_specular = specular;
}

float Material::getShininess() const {
    return m_shininess;
}
void Material::setShininess(float shininess) {
    m_shininess = shininess;
}

int Material::getTextureIndex() const {
    return m_textureIndex;
}
void Material::setTextureIndex(int index) {
    m_textureIndex = index;
}

int Material::getNormalMapIndex() const {
    return m_normalMapIndex;
}
void Material::setNormalMapIndex(int index) {
    m_normalMapIndex = index;
}

float Material::getTextureOpacity() const {
    return m_textureOpacity;
}
void Material::setTextureOpacity(float opacity) {
    m_textureOpacity = opacity;
}

bool Material::getInvertUvX() const {
    return m_invertUvX;
}
void Material::setInvertUvX(bool invertUvX) {
    m_invertUvX = invertUvX;
}

bool Material::getInvertUvY() const {
    return m_invertUvY;
}
void Material::setInvertUvY(bool invertUvY) {
    m_invertUvY = invertUvY;
}

bool Material::getSwapUvs() const {
    return m_swapUvs;
}
void Material::setSwapUvs(bool swapUvs) {
    m_swapUvs = swapUvs;
}

bool Material::getDoubleSidedNormals() const {
    return m_doubleSidedNormals;
}
void Material::setDoubleSidedNormals(bool doubleSidedNormals) {
    m_doubleSidedNormals = doubleSidedNormals;
}

bool Material::getFlipNormals() const {
    return m_flipNormals;
}
void Material::setFlipNormals(bool flipNormals) {
    m_flipNormals = flipNormals;
}

void Material::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_materialColorLoc = glGetUniformLocation(program, (prefix + ".color").c_str());
        m_materialAmbientLoc = glGetUniformLocation(program, (prefix + ".ambient").c_str());
        m_materialDiffuseLoc = glGetUniformLocation(program, (prefix + ".diffuse").c_str());
        m_materialSpecularLoc = glGetUniformLocation(program, (prefix + ".specular").c_str());
        m_materialShininessLoc = glGetUniformLocation(program, (prefix + ".shininess").c_str());
        m_materialTextureIndexLoc = glGetUniformLocation(program, (prefix + ".texture_index").c_str());
        m_materialNormalMapIndexLoc = glGetUniformLocation(program, (prefix + ".normal_map_index").c_str());
        m_materialTextureOpacityLoc = glGetUniformLocation(program, (prefix + ".texture_opacity").c_str());
        m_materialInvertUvXLoc = glGetUniformLocation(program, (prefix + ".invert_uv_x").c_str());
        m_materialInvertUvYLoc = glGetUniformLocation(program, (prefix + ".invert_uv_y").c_str());
        m_materialSwapUvsLoc = glGetUniformLocation(program, (prefix + ".swap_uvs").c_str());
        m_materialDoubleSidedNormalsLoc = glGetUniformLocation(program, (prefix + ".double_sided_normals").c_str());
        m_materialFlipNormalsLoc = glGetUniformLocation(program, (prefix + ".flip_normals").c_str());
        m_locationsSet = true;
    }

    Object::loadShader(program, prefix);

    glUniform4f(m_materialColorLoc, m_color.x, m_color.y, m_color.z, m_color.w);
    glUniform1f(m_materialAmbientLoc, m_ambient);
    glUniform1f(m_materialDiffuseLoc, m_diffuse);
    glUniform1f(m_materialSpecularLoc, m_specular);
    glUniform1f(m_materialShininessLoc, m_shininess);
    glUniform1i(m_materialTextureIndexLoc, m_textureIndex);
    glUniform1i(m_materialNormalMapIndexLoc, m_normalMapIndex);
    glUniform1i(m_materialTextureOpacityLoc, m_textureOpacity);
    glUniform1i(m_materialInvertUvXLoc, m_invertUvX);
    glUniform1i(m_materialInvertUvYLoc, m_invertUvX);
    glUniform1i(m_materialSwapUvsLoc, m_swapUvs);
    glUniform1i(m_materialDoubleSidedNormalsLoc, m_doubleSidedNormals);
    glUniform1i(m_materialFlipNormalsLoc, m_flipNormals);
}

ObjectType Material::getType() const {
    return ObjectType::MATERIAL;
}
