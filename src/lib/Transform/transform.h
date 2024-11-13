#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include "../Object/object.h"

class Transform : public Object {
public:
    Transform();
    Transform(glm::vec3 pos);
    glm::vec3 getPos();
    void setPos(glm::vec3 newPos);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

protected:
    glm::vec3 m_pos{ 0.f, 0.f, 0.f };

    // uniform locations
    GLint m_posLoc;
};
#endif
