#ifndef DISK_H
#define DISK_H

#include "../Plane/plane.h"
#include <glm/vec3.hpp>

class Disk : public Plane {
public:
    Disk();
    Disk(glm::vec3 pos);

    float getRadius();
    void setRadius(float radius);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    float m_radius = 1.;

    bool m_locationsSet = false;
    GLint m_radiusLoc;
};
#endif