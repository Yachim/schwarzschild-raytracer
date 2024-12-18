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

    int getTextureIndex();
    void setTextureIndex(int index);

    int getNormalMapIndex();
    void setNormalMapIndex(int index);

    bool getInvertUvX();
    void setInvertUvX(bool invertUvX);

    bool getInvertUvY();
    void setInvertUvY(bool invertUvY);

    bool getSwapUvs();
    void setSwapUvs(bool swapUvs);

private:
    glm::vec4 m_color{ 0.5, 0., 0.5, 1. };
    float m_ambient = 0.1;
    float m_diffuse = 0.9;
    float m_specular = 0.5;
    float m_shininess = 32.;
    int m_textureIndex = -1;
    int m_normalMapIndex = -1;
    bool m_invertUvX = false;
    bool m_invertUvY = false;
    bool m_swapUvs = false;
};
#endif
