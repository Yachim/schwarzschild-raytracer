#ifndef HOLLOW_DISK_H
#define HOLLOW_DISK_H

#include "../Plane/plane.h"
#include <glm/vec3.hpp>

class HollowDisk : public Plane {
public:
    HollowDisk();
    HollowDisk(glm::vec3 pos);

    float getInnerRadius() const;
    void setInnerRadius(float innerRadius);

    float getOuterRadius() const;
    void setOuterRadius(float outerRadius);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    float m_innerRadius = 2.5;
    float m_outerRadius = 5.;

    bool m_locationsSet = false;
    GLint m_innerRadiusLoc;
    GLint m_outerRadiusLoc;
};
#endif