#ifndef BOX_H
#define BOX_H

#include "../MaterialObject/materialObject.h"
#include "../Transform/transform.h"

class Box : public MaterialObject, public Transform {
public:
    Box();
    Box(glm::vec3 pos);

    float getWidth();
    void setWidth(float width);

    float getDepth();
    void setDepth(float depth);

    float getHeight();
    void setHeight(float height);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    float m_width = 1.;
    float m_depth = 1.;
    float m_height = 1.;

    bool m_locationsSet = false;
    GLint m_widthLoc;
    GLint m_depthLoc;
    GLint m_heightLoc;
};
#endif