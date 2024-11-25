#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include "../Object/object.h"

class Transform : public Object {
public:
    Transform();
    Transform(glm::vec3 pos);

    glm::vec3 getPos();
    void setPos(glm::vec3 newPos);

    glm::mat3 getAxes();
    void setAxes(glm::mat3 newAxes);

    glm::vec3 getForward();
    void setForward(glm::vec3 newForward);

    glm::vec3 getRight();
    void setRight(glm::vec3 newRight);

    glm::vec3 getUp();
    void setUp(glm::vec3 newUp);

    // calculates from the other two
    void calculateForward();
    void calculateRight();
    void calculateUp();

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

protected:
    glm::vec3 m_pos{ 0.f, 0.f, 0.f };
    glm::mat3 m_axes{
        glm::vec3(1., 0., 0.),
        glm::vec3(0., 1., 0.),
        glm::vec3(0., 0., 1.)
    };

    // uniform locations
    GLint m_posLoc;
    GLint m_axesLoc;
};
#endif
