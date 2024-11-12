#ifndef SPHERE_H
#define SPHERE_H

#include <glm/vec3.hpp>
#include "../Object/object.h"
#include "../Material/material.h"

class Sphere : public Object {
public:
    Sphere();
    Sphere(glm::vec3 pos);
    Sphere(glm::vec3 pos, float radius);
    Sphere(glm::vec3 pos, float radius, Material mat);

    float getRadius();
    void setRadius(float radius);

    Material getMaterial();
    void setMaterial(Material mat);

private:
    float m_radius = 1.;
    Material m_material = Material();
};
#endif