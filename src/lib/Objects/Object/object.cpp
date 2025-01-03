#include "object.h"

Object::Object() {}

void Object::loadShader(GLuint program, std::string prefix) {}

ObjectType Object::getType() const {
    return ObjectType::UNKNOWN;
}
