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

void MaterialObject::setupShader(GLuint program, std::string prefix) {
    Object::setupShader(program, prefix);

    m_materialColorLoc = glGetUniformLocation(program, (prefix + ".base.material.color").c_str());
    m_materialAmbientLoc = glGetUniformLocation(program, (prefix + ".base.material.ambient").c_str());
    m_materialDiffuseLoc = glGetUniformLocation(program, (prefix + ".base.material.diffuse").c_str());
    m_materialSpecularLoc = glGetUniformLocation(program, (prefix + ".base.material.specular").c_str());
    m_materialShininessLoc = glGetUniformLocation(program, (prefix + ".base.material.shininess").c_str());
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
