#ifndef CAMERA_H
#define CAMERA_H

#include "../Transform/transform.h"
#include <glm/geometric.hpp>

const float DEFAULT_FOV = 90.;
const float DEFAULT_ORTHOGRAPHIC_WIDTH = 15.;

const float HYPERBOLIC_TRAJECTORY_DURATION = 5.;
const glm::vec3 HYPERBOLIC_TRAJECTORY_BASE_X = glm::vec3(0., 0., -1.);
const glm::vec3 HYPERBOLIC_TRAJECTORY_BASE_Y = glm::vec3(cos(M_PI / 10.), sin(M_PI / 10.), 0.);

enum RaytraceType {
    CURVED,
    FLAT,
    HALF_WIDTH,
    HALF_HEIGHT
};

class Camera : public Transform {
public:
    Camera();
    Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 right);
    Camera(glm::vec3 pos);

    void setFov(float fov);
    float getFov() const;

    void hyperbolicTrajectory(float initialDistance, float closestDistance, float time);
    void lookAt(glm::vec3 point = glm::vec3(0., 0., 0.));

    void loadShader(GLuint program);

    ObjectType getType() const override;

private:
    float m_fov = DEFAULT_FOV;

    // uniform locations
    bool m_locationsSet = false;
    GLint m_fovLoc;

    void loadShader(GLuint program, std::string prefix) override {};
};
#endif
