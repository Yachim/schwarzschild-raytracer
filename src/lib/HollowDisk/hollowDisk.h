#ifndef HOLLOW_DISK_H
#define HOLLOW_DISK_H

#include "../Material/material.h"
#include "../Plane/plane.h"
#include <glm/vec3.hpp>

class HollowDisk : public Plane {
public:
    HollowDisk();
    HollowDisk(glm::vec3 pos);
    HollowDisk(glm::vec3 pos, glm::vec3 normal, Material mat);

    float getInnerRadius();
    void setInnerRadius(float innerRadius);

    float getOuterRadius();
    void setOuterRadius(float outerRadius);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    float m_innerRadius = 2.5;
    float m_outerRadius = 5.;

    GLint m_innerRadiusLoc;
    GLint m_outerRadiusLoc;
};
#endif