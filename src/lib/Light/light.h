#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec2.hpp>
#include "../Object/object.h"

class Light: public Object {
public:
    Light();
    Light(glm::vec3 pos, glm::vec3 color, float intensity, 
          float attenuationConstant = 1., 
          float attenuationLinear = 0.09, 
          float attenuationQuadratic = 0.032);

    glm::vec3 getColor();
    void setColor(const glm::vec3 color);

    float getIntensity();
    void setIntensity(float intensity);

    float getAttenuationConstant();
    void setAttenuationConstant(float constant);

    float getAttenuationLinear();
    void setAttenuationLinear(float linear);

    float getAttenuationQuadratic();
    void setAttenuationQuadratic(float quadratic);


private:
    glm::vec3 m_color;
    float m_intensity;
    float m_attenuation_constant;
    float m_attenuation_linear;
    float m_attenuation_quadratic;
};
#endif
