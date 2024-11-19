#ifndef MATERIAL_OBJECT_H
#define MATERIAL_OBJECT_H

#include "../Object/object.h"
#include "../Material/material.h"

class MaterialObject : public Object {
public:
    MaterialObject();
    MaterialObject(Material mat);

    Material getMaterial();
    void setMaterial(Material mat);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

    glm::vec4 getMaterialColor();
    void setMaterialColor(glm::vec4 color);

    float getMaterialAmbient();
    void setMaterialAmbient(float ambient);

    float getMaterialDiffuse();
    void setMaterialDiffuse(float diffuse);

    float getMaterialSpecular();
    void setMaterialSpecular(float specular);

    float getMaterialShininess();
    void setMaterialShininess(float shininess);

    int getMaterialTextureIndex();
    void setMaterialTextureIndex(int index);

    bool getMaterialInvertUvX();
    void setMaterialInvertUvX(bool invertUvX);

    bool getMaterialInvertUvY();
    void setMaterialInvertUvY(bool invertUvY);

    bool getMaterialSwapUvs();
    void setMaterialSwapUvs(bool swapUvs);

protected:
    Material m_material = Material();

    // uniform locations
    GLint m_materialColorLoc;
    GLint m_materialAmbientLoc;
    GLint m_materialDiffuseLoc;
    GLint m_materialSpecularLoc;
    GLint m_materialShininessLoc;
    GLint m_materialTextureIndexLoc;
    GLint m_materialInvertUvXLoc;
    GLint m_materialInvertUvYLoc;
    GLint m_materialSwapUvsLoc;
};
#endif
