#ifndef OBJECT_H
#define OBJECT_H

#include <GLFW/glfw3.h>

#include <string>

enum ObjectType {
    SPHERE,
    PLANE,
    DISK,
    HOLLOW_DISK,
    LATERAL_CYLINDER,
    RECTANGLE
};

class Object {
public:
    Object();

    virtual void setupShader(GLuint program, std::string prefix);
    virtual void loadShader();
};
#endif
