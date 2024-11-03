#include "object.h"

Object::Object() {}

Object::Object(glm::vec3 pos) {
    m_pos = pos;
}

glm::vec3 Object::getPos() {
    return m_pos;
}

void Object::setPos(glm::vec3 newPos) {
    m_pos = newPos;
}