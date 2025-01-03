#ifndef MATERIAL_OBJECT_H
#define MATERIAL_OBJECT_H

#include "../Object/object.h"
#include "../../Material/material.h"

class MaterialObject : public virtual Object {
public:
    MaterialObject();
    MaterialObject(Material mat);

    Material getMaterial();
    void setMaterial(Material mat);

    void loadShader(GLuint program, std::string prefix) override;

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

    int getMaterialNormalMapIndex();
    void setMaterialNormalMapIndex(int index);

    bool getMaterialInvertUvX();
    void setMaterialInvertUvX(bool invertUvX);

    bool getMaterialInvertUvY();
    void setMaterialInvertUvY(bool invertUvY);

    bool getMaterialSwapUvs();
    void setMaterialSwapUvs(bool swapUvs);

    bool getMaterialDoubleSidedNormals();
    void setMaterialDoubleSidedNormals(bool doubleSidedNormals);

    bool getMaterialFlipNormals();
    void setMaterialFlipNormals(bool flipNormals);

protected:
    Material m_material = Material();

private:
    // uniform locations
    bool m_locationsSet = false;
    GLint m_materialColorLoc;
    GLint m_materialAmbientLoc;
    GLint m_materialDiffuseLoc;
    GLint m_materialSpecularLoc;
    GLint m_materialShininessLoc;
    GLint m_materialTextureIndexLoc;
    GLint m_materialNormalMapIndexLoc;
    GLint m_materialInvertUvXLoc;
    GLint m_materialInvertUvYLoc;
    GLint m_materialSwapUvsLoc;
    GLint m_materialDoubleSidedNormalsLoc;
    GLint m_materialFlipNormalsLoc;
};
#endif
