#ifndef DISK_H
#define DISK_H

#include "../Material/material.h"
#include "../Plane/plane.h"
#include <glm/vec3.hpp>

class Disk : public Plane {
public:
    Disk();
    Disk(glm::vec3 pos);

    float getRadius();
    void setRadius(float radius);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    float m_radius = 1.;

    GLint m_radiusLoc;
};
#endif