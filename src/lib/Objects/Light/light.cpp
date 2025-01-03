#include <glad/glad.h>
#include "light.h"

Light::Light() : Light::Light(glm::vec3(10., 10., 10.), glm::vec3(1., 1., 1.), 2.5) {}
Light::Light(glm::vec3 pos, glm::vec3 color, float intensity, float attenuationConstant, float attenuationLinear, float attenuationQuadratic) :
    Transform(pos),
    m_color(color),
    m_intensity(intensity),
    m_attenuationConstant(attenuationConstant),
    m_attenuationLinear(attenuationLinear),
    m_attenuationQuadratic(attenuationQuadratic) {}

glm::vec3 Light::getColor() { return m_color; }
void Light::setColor(glm::vec3 color) { m_color = color; }

float Light::getIntensity() { return m_intensity; }
void Light::setIntensity(float intensity) { m_intensity = intensity; }

float Light::getAttenuationConstant() { return m_attenuationConstant; }
void Light::setAttenuationConstant(float constant) { m_attenuationConstant = constant; }

float Light::getAttenuationLinear() { return m_attenuationLinear; }
void Light::setAttenuationLinear(float linear) { m_attenuationLinear = linear; }

float Light::getAttenuationQuadratic() { return m_attenuationQuadratic; }
void Light::setAttenuationQuadratic(float quadratic) { m_attenuationQuadratic = quadratic; }

void Light::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_colorPos = glGetUniformLocation(program, (prefix + ".color").c_str());
        m_intensityPos = glGetUniformLocation(program, (prefix + ".intensity").c_str());
        m_attenuationConstantPos = glGetUniformLocation(program, (prefix + ".attenuation_constant").c_str());
        m_attenuationLinearPos = glGetUniformLocation(program, (prefix + ".attenuation_linear").c_str());
        m_attenuationQuadraticPos = glGetUniformLocation(program, (prefix + ".attenuation_quadratic").c_str());
        m_locationsSet = true;
    }

    Transform::loadShader(program, prefix + ".transform");

    glUniform3f(m_colorPos, m_color.x, m_color.y, m_color.z);
    glUniform1f(m_intensityPos, m_intensity);
    glUniform1f(m_attenuationConstantPos, m_attenuationConstant);
    glUniform1f(m_attenuationLinearPos, m_attenuationLinear);
    glUniform1f(m_attenuationQuadraticPos, m_attenuationQuadratic);
}

ObjectType Light::getType() const {
    return ObjectType::LIGHT;
}
