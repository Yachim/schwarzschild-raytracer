#ifndef OBJECT_H
#define OBJECT_H

#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>

class Object {
public:
    Object();
    Object(glm::vec3 pos);
    glm::vec3 getPos();
    void setPos(glm::vec3 newPos);

    virtual void setupShader(GLuint program, int i) {};
    virtual void loadShader() {};

protected:
    glm::vec3 m_pos{ 0.f, 0.f, 0.f };

    // uniform locations
    GLint m_posLoc;
    GLint m_opaqueLoc;
    GLint m_materialColorLoc;
    GLint m_materialAmbientLoc;
    GLint m_materialDiffuseLoc;
    GLint m_materialSpecularLoc;
    GLint m_materialShininessLoc;
};
#endif
