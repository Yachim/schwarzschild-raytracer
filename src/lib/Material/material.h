#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec4.hpp>

class Material {
public:
    Material();
    Material(glm::vec4 color);
    Material(glm::vec4 color, float ambient, float diffuse, float specular, float shininess);

    glm::vec4 getColor();
    void setColor(glm::vec4 color);

    float getAmbient();
    void setAmbient(float ambient);

    float getDiffuse();
    void setDiffuse(float diffuse);

    float getSpecular();
    void setSpecular(float specular);

    float getShininess();
    void setShininess(float shininess);

private:
    glm::vec4 m_color{ 0.5, 0., 0.5, 1. };
    float m_ambient = 0.1;
    float m_diffuse = 0.9;
    float m_specular = 0.5;
    float m_shininess = 32.;
};
#endif
