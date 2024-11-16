#ifndef CAMERA_H
#define CAMERA_H

#include "../Transform/transform.h"
#include <glm/geometric.hpp>

const float DEFAULT_FOV = 90.;

const float HYPERBOLIC_TRAJECTORY_DURATION = 5.;
const glm::vec3 HYPERBOLIC_TRAJECTORY_BASE_X = glm::vec3(0., 0., -1.);
const glm::vec3 HYPERBOLIC_TRAJECTORY_BASE_Y = glm::vec3(cos(M_PI / 10.), sin(M_PI / 10.), 0.);

class Camera : public Transform {
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

    void setFov(float fov);
    float getFov();

    void hyperbolicTrajectory(float initialDistance, float closestDistance, float time);
    void lookAt(glm::vec3 point = glm::vec3(0., 0., 0.));

    void setupShader(GLuint program);
    void loadShader() override;

private:
    glm::vec3 m_forward{ 0.f, 0.f, -1.f };
    glm::vec3 m_right{ 1.f, 0.f, 0.f };
    glm::vec3 m_up{ 0.f, 1.f, 0.f };
    float m_fov = DEFAULT_FOV;

    // uniform locations
    GLint m_forwardLoc;
    GLint m_rightLoc;
    GLint m_upLoc;
    GLint m_fovLoc;

    void setupShader(GLuint program, std::string prefix) override {};
};
#endif
