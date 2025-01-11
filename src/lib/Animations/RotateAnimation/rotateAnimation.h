#ifndef ROTATE_ANIMATION_H
#define ROTATE_ANIMATION_H
#include "../../Objects/Transform/transform.h"
#include "../Animation/animation.h"
#include <glm/vec3.hpp>

class RotateAnimation : public virtual Animation {
private:
    Transform* m_object;
    glm::vec3 m_axis = { 0., 1., 0. };
    float m_startAngle = 0.;
    float m_endAngle = 2. * M_PI;
public:
    RotateAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    RotateAnimation(const RotateAnimation& animation);

    Transform* getObject() const;
    void setObject(Transform* object);

    glm::vec3 getAxis() const;
    void setAxis(glm::vec3 axis);

    float getStartAngle() const;
    void setStartAngle(float startAngle);

    float getEndAngle() const;
    void setEndAngle(float endAngle);

    void animate(double t) override;
};
#endif
