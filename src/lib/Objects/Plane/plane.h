#ifndef PLANE_H
#define PLANE_H

#include "../MaterialObject/materialObject.h"
#include "../Transform/transform.h"
#include <glm/vec2.hpp>

class Plane : public MaterialObject, public Transform {
public:
    Plane();
    Plane(glm::vec3 pos);

    glm::vec2 getTextureSize();
    void setTextureSize(glm::vec2 size);

    glm::vec2 getTextureOffset();
    void setTextureOffset(glm::vec2 offset);

    bool getRepeatTexture();
    void setRepeatTexture(bool repeatTexture);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType()  const override;

private:
    glm::vec2 m_textureSize{ 1., 1. };
    glm::vec2 m_textureOffset{ 0., 0. };
    bool m_repeatTexture = true;

    bool m_locationsSet = false;
    GLint m_textureSizeLoc;
    GLint m_textureOffsetLoc;
    GLint m_repeatTextureLoc;
};
#endif