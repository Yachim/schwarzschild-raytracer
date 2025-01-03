#ifndef LIGHT_H
#define LIGHT_H

#include "../Transform/transform.h"
#include <iostream>

class Light : public Transform {
public:
    Light();
    Light(glm::vec3 pos, glm::vec3 color, float intensity,
        float attenuationConstant = 1.,
        float attenuationLinear = 0.09,
        float attenuationQuadratic = 0.032);

    glm::vec3 getColor() const;
    void setColor(const glm::vec3 color);

    float getIntensity() const;
    void setIntensity(float intensity);

    float getAttenuationConstant() const;
    void setAttenuationConstant(float constant);

    float getAttenuationLinear() const;
    void setAttenuationLinear(float linear);

    float getAttenuationQuadratic() const;
    void setAttenuationQuadratic(float quadratic);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    glm::vec3 m_color;
    float m_intensity;
    float m_attenuationConstant;
    float m_attenuationLinear;
    float m_attenuationQuadratic;

    bool m_locationsSet = false;
    GLuint m_colorPos;
    GLuint m_intensityPos;
    GLuint m_attenuationConstantPos;
    GLuint m_attenuationLinearPos;
    GLuint m_attenuationQuadraticPos;
};
#endif
