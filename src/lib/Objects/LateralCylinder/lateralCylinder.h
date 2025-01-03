#ifndef LATERAL_CYLINDER_H
#define LATERAL_CYLINDER_H

#include "../MaterialObject/materialObject.h"
#include "../Transform/transform.h"

class LateralCylinder : public MaterialObject, public Transform {
public:
    LateralCylinder();

    float getHeight() const;
    void setHeight(float height);

    float getRadius() const;
    void setRadius(float radius);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    float m_height = 5.;
    float m_radius = 1.;

    bool m_locationsSet = false;
    GLint m_heightLoc;
    GLint m_radiusLoc;
};
#endif