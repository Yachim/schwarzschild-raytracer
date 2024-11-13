#ifndef LIGHT_H
#define LIGHT_H

#include "../Transform/transform.h"

class Light : public Transform {
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

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    glm::vec3 m_color;
    float m_intensity;
    float m_attenuationConstant;
    float m_attenuationLinear;
    float m_attenuationQuadratic;

    GLuint m_colorPos;
    GLuint m_intensityPos;
    GLuint m_attenuationConstantPos;
    GLuint m_attenuationLinearPos;
    GLuint m_attenuationQuadraticPos;
};
#endif
