#ifndef UTILS_H
#define UTILS_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <sstream>

glm::vec3 rotateVector(float angle, glm::vec3 vec, glm::vec3 axis = glm::vec3(0., 1., 0.));
std::string vec3ToString(glm::vec3 vec);
std::string vec2ToString(glm::vec2 vec);
#endif
