#ifndef LATERAL_CYLINDER_H
#define LATERAL_CYLINDER_H

#include "../MaterialObject/materialObject.h"
#include "../Material/material.h"
#include "../Transform/transform.h"

class LateralCylinder : public MaterialObject, public Transform {
public:
    LateralCylinder();

    glm::vec3 getHeight();
    void setHeight(glm::vec3 height);

    float getRadius();
    void setRadius(float radius);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    glm::vec3 m_height{ 0., 5., 0. };
    float m_radius = 1.;

    GLint m_heightLoc;
    GLint m_radiusLoc;
};
#endif