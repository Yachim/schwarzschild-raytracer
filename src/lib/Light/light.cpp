#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "light.h"

Light::Light() : Light::Light(glm::vec3(10., 10., 10.), glm::vec3(1., 1., 1.), 2.5) {}
Light::Light(glm::vec3 pos, glm::vec3 color, float intensity, float attenuationConstant, float attenuationLinear, float attenuationQuadratic) :
    Object(pos),
    m_color(color),
    m_intensity(intensity),
    m_attenuation_constant(attenuationConstant),
    m_attenuation_linear(attenuationLinear),
    m_attenuation_quadratic(attenuationQuadratic) {}

glm::vec3 Light::getColor() { return m_color; }
void Light::setColor(glm::vec3 color) { m_color = color; }

float Light::getIntensity() { return m_intensity; }
void Light::setIntensity(float intensity) { m_intensity = intensity; }

float Light::getAttenuationConstant() { return m_attenuation_constant; }
void Light::setAttenuationConstant(float constant) { m_attenuation_constant = constant; }

float Light::getAttenuationLinear() { return m_attenuation_linear; }
void Light::setAttenuationLinear(float linear) { m_attenuation_linear = linear; }

float Light::getAttenuationQuadratic() { return m_attenuation_quadratic; }
void Light::setAttenuationQuadratic(float quadratic) { m_attenuation_quadratic = quadratic; }

void Light::setupShader(GLuint program, int i) {
    m_posLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.pos").c_str());
    m_opaqueLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.opaque").c_str());
    m_materialColorLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.color").c_str());
    m_materialAmbientLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.ambient").c_str());
    m_materialDiffuseLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.diffuse").c_str());
    m_materialSpecularLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.specular").c_str());
    m_materialShininessLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].base.material.shininess").c_str());
    m_radiusLoc = glGetUniformLocation(program, ("spheres[" + std::to_string(i) + "].radius").c_str());
}

void Light::loadShader() {
    glUniform3f(m_posLoc, m_pos.x, m_pos.y, m_pos.z);
    glUniform1i(m_opaqueLoc, 1); // TODO:  
    glUniform4f(m_materialColorLoc, m_material.getColor().x, m_material.getColor().y, m_material.getColor().z, m_material.getColor().w);
    glUniform1f(m_materialAmbientLoc, m_material.getAmbient());
    glUniform1f(m_materialDiffuseLoc, m_material.getDiffuse());
    glUniform1f(m_materialSpecularLoc, m_material.getSpecular());
    glUniform1f(m_materialShininessLoc, m_material.getShininess());
    glUniform1f(m_radiusLoc, m_radius);
}
