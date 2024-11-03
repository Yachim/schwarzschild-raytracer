#include "utils.h"
#include <glm/geometric.hpp>

// https://math.stackexchange.com/a/1432182/1276440
// a: v, b: axis
glm::vec3 rotateVector(float angle, glm::vec3 vec, glm::vec3 axis) {
    glm::vec3 perpPart = dot(vec, axis) / dot(axis, axis) * axis;
    glm::vec3 orthPart = vec - perpPart;
    float orthPartLength = orthPart.length();

    glm::vec3 w = cross(axis, orthPart);

    float x1 = cos(angle) / orthPartLength;
    float x2 = sin(angle) / w.length();

    glm::vec3 newOrthPart = orthPartLength * (x1 * orthPart + x2 * w);
    return newOrthPart + perpPart;
}
