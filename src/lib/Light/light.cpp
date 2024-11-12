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
