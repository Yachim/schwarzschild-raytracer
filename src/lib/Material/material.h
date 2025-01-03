#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec4.hpp>

class Material {
public:
    Material();
    Material(glm::vec4 color);
    Material(glm::vec4 color, float ambient, float diffuse, float specular, float shininess);

    glm::vec4 getColor() const;
    void setColor(glm::vec4 color);

    float getAmbient() const;
    void setAmbient(float ambient);

    float getDiffuse() const;
    void setDiffuse(float diffuse);

    float getSpecular() const;
    void setSpecular(float specular);

    float getShininess() const;
    void setShininess(float shininess);

    int getTextureIndex() const;
    void setTextureIndex(int index);

    int getNormalMapIndex() const;
    void setNormalMapIndex(int index);

    bool getInvertUvX() const;
    void setInvertUvX(bool invertUvX);

    bool getInvertUvY() const;
    void setInvertUvY(bool invertUvY);

    bool getSwapUvs() const;
    void setSwapUvs(bool swapUvs);

    bool getDoubleSidedNormals() const;
    void setDoubleSidedNormals(bool doubleSidedNormals);

    bool getFlipNormals() const;
    void setFlipNormals(bool flipNormals);

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
    bool m_doubleSidedNormals = true;
    bool m_flipNormals = false;
};
#endif
