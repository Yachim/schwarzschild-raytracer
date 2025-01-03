#ifndef OBJECT_H
#define OBJECT_H

#include <GLFW/glfw3.h>
#include <string>

enum ObjectType {
    UNKNOWN = -3,
    CAMERA = -2,
    LIGHT = -1,
    SPHERE,
    PLANE,
    DISK,
    HOLLOW_DISK,
    LATERAL_CYLINDER,
    RECTANGLE,
    BOX
};

class Object {
public:
    Object();

    virtual void loadShader(GLuint program, std::string prefix);
    virtual ObjectType getType() const;
};
#endif
