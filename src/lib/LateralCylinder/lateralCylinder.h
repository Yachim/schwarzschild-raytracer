#ifndef LATERAL_CYLINDER_H
#define LATERAL_CYLINDER_H

#include "../MaterialObject/materialObject.h"
#include "../Material/material.h"
#include "../Transform/transform.h"

class LateralCylinder : public MaterialObject, public Transform {
public:
    LateralCylinder();

    float getHeight();
    void setHeight(float height);

    float getRadius();
    void setRadius(float radius);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    float m_height = 5.;
    float m_radius = 1.;

    GLint m_heightLoc;
    GLint m_radiusLoc;
};
#endif