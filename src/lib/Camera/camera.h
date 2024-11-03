#include <glm/vec3.hpp>

#ifndef CAMERA_DEFINED
#define CAMERA_DEFINED
#include "../Object/object.h"

class Camera: public Object {
public:
    Camera();
    Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 right);
    Camera(glm::vec3 pos);

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

    void rotateAround(float angle, glm::vec3 point = glm::vec3(0., 0., 0.), glm::vec3 axis = glm::vec3(0., 1., 0.));

private:
    glm::vec3 m_forward{0.f, 0.f, -1.f};
    glm::vec3 m_right{1.f, 0.f, 0.f};
    glm::vec3 m_up{0.f, 1.f, 0.f};
};
#endif
