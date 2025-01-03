#ifndef SPHERE_H
#define SPHERE_H

#include "../MaterialObject/materialObject.h"
#include "../Transform/transform.h"

class Sphere : public MaterialObject, public Transform {
public:
    Sphere();
    Sphere(glm::vec3 pos);
    Sphere(glm::vec3 pos, float radius);
    Sphere(glm::vec3 pos, float radius, Material mat);

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