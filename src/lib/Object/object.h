#ifndef OBJECT_H
#define OBJECT_H

#include <GLFW/glfw3.h>

#include <string>

class Object {
public:
    Object();

    virtual void setupShader(GLuint program, std::string prefix);
    virtual void loadShader();
};
#endif
