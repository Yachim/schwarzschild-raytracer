#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Object/object.h"

class Transform : public virtual Object {
public:
    Transform();
    Transform(glm::vec3 pos);

    glm::vec3 getPos() const;
    void setPos(glm::vec3 newPos);

    glm::mat3 getAxes() const;
    void setAxes(glm::mat3 newAxes);
    void setAxes(glm::quat rot);

    glm::vec3 getForward() const;
    void setForward(glm::vec3 newForward);

    glm::vec3 getRight() const;
    void setRight(glm::vec3 newRight);

    glm::vec3 getUp() const;
    void setUp(glm::vec3 newUp);

    // calculates from the other two
    void calculateForward();
    void calculateRight();
    void calculateUp();

    void loadShader(GLuint program, std::string prefix) override;

protected:
    glm::vec3 m_pos{ 0.f, 0.f, 0.f };
    glm::mat3 m_axes{
        glm::vec3(1., 0., 0.),
        glm::vec3(0., 1., 0.),
        glm::vec3(0., 0., 1.)
    };

private:
    // uniform locations
    bool m_locationsSet = false;
    GLint m_posLoc;
    GLint m_axesLoc;
};
#endif
