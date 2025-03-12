#ifndef CURVE_VISUALIZATION_H
#define CURVE_VISUALIZATION_H
#include <vector>
#include <glm/vec3.hpp>

class CurveVisualization {
public:
    virtual std::vector<glm::vec3> getPoints(int n = 100) const = 0;
};
#endif