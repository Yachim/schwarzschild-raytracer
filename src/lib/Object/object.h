#ifndef OBJECT_H
#define OBJECT_H

#include <glm/vec3.hpp>

class Object {
public:
    Object();
    Object(glm::vec3 pos);
    glm::vec3 getPos();
    void setPos(glm::vec3 newPos);

protected:
    glm::vec3 m_pos{ 0.f, 0.f, 0.f };
};
#endif
