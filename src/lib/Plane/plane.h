#ifndef PLANE_H
#define PLANE_H

#include "../MaterialObject/materialObject.h"
#include "../Material/material.h"
#include "../Transform/transform.h"
#include <glm/vec2.hpp>

class Plane : public MaterialObject, public Transform {
public:
    Plane();
    Plane(glm::vec3 pos);
    Plane(glm::vec3 pos, glm::vec3 normal);
    Plane(glm::vec3 pos, glm::vec3 normal, Material mat);

    glm::vec3 getNormal();
    void setNormal(glm::vec3 normal);

    glm::vec2 getTextureSize();
    void setTextureSize(glm::vec2 size);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    glm::vec3 m_normal{ 0., 1., 0. };
    glm::vec2 m_textureSize{ 1., 1. };

    GLint m_normalLoc;
    GLint m_textureSizeLoc;
};
#endif