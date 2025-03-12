#ifndef CURVE_H
#define CURVE_H

#include "../MaterialObject/materialObject.h"
#include "../Transform/transform.h"
#include <vector>
#include <glm/vec3.hpp>

class Curve : public MaterialObject {
public:
    Curve();

    float getRadius() const;
    void setRadius(float radius);

    std::vector<glm::vec3> getPoints() const;
    void setPoints(std::vector<glm::vec3> points);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    std::vector<glm::vec3> m_points;
    float m_radius = 0.025;

    bool m_locationsSet = false;
    GLint m_radiusLoc;
    GLint m_numPointsLoc;
};
#endif